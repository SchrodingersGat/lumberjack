#include <qfile.h>
#include <qfileinfo.h>
#include <qprogressdialog.h>
#include <qelapsedtimer.h>
#include <qapplication.h>

#include "cobsr/cobsr.h"

#include "CEDATProtocol.hpp"
#include "CEDATPackets.hpp"

#include "cedat_importer.hpp"

// Simple fletcher checksum algorithm
uint16_t  fletcher_encode(QByteArray data, uint32_t len);

uint16_t fletcher_encode(QByteArray data, uint32_t len)
{

    uint32_t i = 0;
    unsigned char c0 = 0;
    unsigned char c1 = 0;
    uint16_t Checksum;

    if (data.count() < len) len = data.count();

    // Calculate checksum intermediate bytes
    for( i = 0; i < len; i++)
    {
        c0 = (unsigned char)(c0 + (unsigned char) data.at(i));
        c1 = (unsigned char)(c1 + c0);
    }// for all the bytes

    // Assemble the 16-bit checksum value
    Checksum = (unsigned char)(c0 - c1);
    Checksum = Checksum << 8;		// MS byte
    Checksum |= (unsigned char)(c1 - 2*c0);	// LS byte

    return Checksum;
}


CEDATPacket::CEDATPacket()
{
    reset();
}


CEDATPacket::CEDATPacket(QByteArray payload)
{
    /*
     * Construct a "CEDATPacket" object from incoming bytes:
     *
     * Packet format is as follows:
     *
     * - Packet ID          (1 bytes)
     * - Packet Flags       (1 bytes)
     * - Timestamp          ({0-8} bytes) - Length denoted in packet flags
     * - Packet Length      (2 bytes)
     * - Packet Data        ({0-1024} bytes) - Length denoted by "packet length"
     * - Packet Checksum    (2 bytes)
     */

    reset();

    const int n = payload.count();

    int offset = 0;

    if (n < 6)
    {
        qWarning() << "CEDATPacket constructor called with only " + QString::number(n) + " bytes";
        return;
    }

    // Extract packet type and packet flags
    pktType = payload[0] & 0xFF;
    pktFlags = payload[1] & 0xFF;

    // How many timestamp bytes?
    int nTsBytes = pktFlags & 0x0F;

    if (n < (6 + nTsBytes))
    {
        qWarning() << "CEDATPacket called with insufficient bytes after timestamp (" + QString::number(n) + ")";
        return;
    }

    for (int ii = 0; ii < nTsBytes; ii++)
    {
        uint8_t tsByte = payload[2 + ii];

        // Timestamp is "little endian" encoded
        pktTimestamp += ((uint64_t) tsByte & 0xFF) << (8 * ii);
    }

    offset = nTsBytes + 2;

    // Packet length is "big endian" encoded
    pktLength  = (uint16_t) (payload[offset++] & 0xFF) << 8;
    pktLength += (uint16_t) (payload[offset++] & 0xFF);

    if (n < (6 + nTsBytes + pktLength))
    {
        qWarning() << "CEDATPacket called with insufficient bytes after length (" + QString::number(n) + ")";
        return;
    }

    for (int idx = 0; idx < pktLength; idx++)
    {
        pktData[idx] = payload[offset++] & 0xFF;
    }

    // Calculate the checksum across number of bytes received so far
    uint16_t checksum = fletcher_encode(payload, offset);

    // Calculate checksum - "big endian" encoded
    pktChecksum  = (uint16_t) (payload[offset++] & 0xFF) << 8;
    pktChecksum += (uint16_t) (payload[offset++] & 0xFF);

    valid = pktChecksum == checksum;

    if (!valid)
    {
        qWarning() << "Checksum mismatch:" << QString::number(pktChecksum, 16) << QString::number(checksum, 16);
        qWarning() << "Payload:" << payload.toHex(' ');
    }
}


void CEDATPacket::reset()
{
    memset(pktData, 0, sizeof(pktData));
    pktLength = 0;
    pktFlags = 0;
    pktType = 0;
    pktChecksum = 0;
    pktTimestamp = 0;
    valid = false;
}



CEDATImporter::CEDATImporter() : FileDataSource("CEDAT Importer")
{

}


CEDATImporter::~CEDATImporter()
{
    qDebug() << "~CEDatImporter";

    blockData.clear();
    variableMap.clear();

    // TODO - Any custom cleanup?
}


QStringList CEDATImporter::getSupportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes.append("cedat");

    return fileTypes;
}


bool CEDATImporter::loadDataFromFile(QStringList &errors)
{
    QFile f(filename);

    QFileInfo fileInfo(filename);

    qint64 fileSize = fileInfo.size();

    // Open the file object
    if (!f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
    {
        errors.append(tr("Could not open file for reading"));
        f.close();
        return false;
    }

    QElapsedTimer elapsed;
    QProgressDialog progress;

    progress.setWindowTitle(tr("Reading File"));
    progress.setLabelText(tr("Reading data - ") + filename);

    progress.setMinimum(0);
    progress.setMaximum(fileSize);
    progress.setValue(0);

    elapsed.restart();

    progress.show();

    // Ensure we are at the start of the file!
    f.seek(0);

    const int CHUNK_SIZE = 4096;

    qint64 byte_count = 0;
    qint64 chunk_count = 0;

    QByteArray chunk;

    // Read out data a chunk at a time
    while (!f.atEnd() && !progress.wasCanceled())
    {
        chunk = f.read(CHUNK_SIZE);

        chunk_count += 1;
        byte_count += chunk.size();

        if (chunk.size() > 0)
        {
            processChunk(chunk);
        }

        // Update progress bar
        if (elapsed.elapsed() > 250)
        {
            progress.setValue(byte_count);

            QApplication::processEvents();

            elapsed.restart();
        }
    }

    // Ensure the file object is closed
    f.close();

    if (progress.wasCanceled())
    {
        qDebug() << "file import cancelled";
        return false;
    }
    else
    {
        progress.close();
    }

    return getSeriesCount() > 0;
}


/**
 * @brief CEDATImporter::processChunk - Process a chunk of data (raw bytes) from the file
 * @param chunk
 */
void CEDATImporter::processChunk(const QByteArray& chunk)
{
    for (auto& byte : chunk)
    {
        if (byte == 0x00)
        {
            processBlock();
            blockData.clear();
        }
        else
        {
            blockData.append(byte);
        }
    }
}


/**
 * @brief CEDATImporter::processBlock - Process a single data block from the file
 * Data blocks are designated by zero (0x00) bytes (cobbs encoded)
 */
void CEDATImporter::processBlock()
{
    // Ignore empty block
    if (blockData.isEmpty()) return;

    // Construct an empty byte array for cobsr decoded data
    QByteArray data(blockData.count(), 0x00);

    // Cobbs-decode the data
    auto result = cobsr_decode(
                reinterpret_cast<uint8_t*>(data.data()),
                data.count(),
                reinterpret_cast<const uint8_t*>(blockData.constData()),
                blockData.count()
    );

    data.truncate(result.out_len);

    if (result.status != COBSR_DECODE_OK)
    {
        qWarning() << "cobsr_decode returned status " + QString::number(result.status);
        return;
    }

    if (result.out_len < 6)
    {
        qWarning() << "cobsr_decode returned only " + QString::number(result.out_len) + " bytes";
        return;
    }

    processPacket(data);
}


void CEDATImporter::processPacket(const QByteArray &packetData)
{
    // Convert the QByteArray data into a structured packet
    CEDATPacket packet(packetData);

    // Various data structures
    CEDAT_NewLogVariable_t newVariable;
    CEDAT_TimestampedData_t newDataFloat;
    CEDAT_TimestampedBooleanData_t newDataBoolean;

    if (!packet.valid)
    {
        qWarning() << "Invalid packet:" << packetData.toHex(' ');
        return;
    }

    // CEDAT packet timestamps are specified in milliseconds
    double timestamp = (double) packet.pktTimestamp / 1000;

    if (newVariable.decode(&packet))
    {
        QString group = QString(newVariable.ownerName) + ":" + QString::number(newVariable.ownerId);
        QString title = QString(newVariable.title);

        // Add a new variable
        DataSeries *series = new DataSeries();

        QString label = QString(newVariable.ownerName) + ":" + QString::number(newVariable.ownerId);

        series->setGroup(group);
        series->setLabel(title);
        series->setUnits(newVariable.units);

        variableMap[newVariable.variableId] = title;

        addSeries(series);
    }
    else if (newDataFloat.decode(&packet))
    {
        if (variableMap.contains(newDataFloat.variableId))
        {
            QString label = variableMap[newDataFloat.variableId];

            auto series = getSeriesByLabel(label);

            if (!series.isNull())
            {
                series->addData(timestamp, newDataFloat.value, false);
            }

        }
        else
        {
            qWarning() << "New float data received with unknown id " + QString::number(newDataFloat.variableId);
        }
    }
    else if (newDataBoolean.decode(&packet))
    {
        if (variableMap.contains(newDataBoolean.variableId))
        {
            QString label = variableMap[newDataBoolean.variableId];

            auto series = getSeriesByLabel(label);

            if (!series.isNull())
            {
                series->addData(timestamp, newDataBoolean.value ? 1 : 0, false);
            }
        }
        else
        {
            qWarning() << "New bool data received with unknown id " + QString::number(newDataBoolean.variableId);
        }
    }
}


/*
 * Wrapper functions for ProtoGen data access.
 * See function definitions in CEDATProtocol.hpp
 *
 * Each function is passed a pointer to a CEDATPacket class
 */
uint8_t* getCEDATPacketData(void* pkt)
{
    CEDATPacket* ptr = (CEDATPacket*) pkt;

    return (uint8_t*) ptr->pktData;
}


const uint8_t* getCEDATPacketDataConst(const void* pkt)
{
    CEDATPacket* ptr = (CEDATPacket*) pkt;
    return (const uint8_t*) ptr->pktData;
}


void finishCEDATPacket(void* pkt, int size, uint32_t packetId)
{
    CEDATPacket* ptr = (CEDATPacket*) pkt;

    ptr->pktLength = size;
    ptr->pktType = packetId;
}


int getCEDATPacketSize(const void* pkt)
{
    CEDATPacket* ptr = (CEDATPacket*) pkt;

    return ptr->pktLength;
}


uint32_t getCEDATPacketID(const void* pkt)
{
    CEDATPacket* ptr = (CEDATPacket*) pkt;

    return ptr->pktType;
}


