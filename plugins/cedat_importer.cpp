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
uint16_t  fletcher_encode(uint8_t *data, uint32_t len);

uint16_t fletcher_encode(uint8_t *data, uint32_t len)
{

    uint32_t i = 0;
    unsigned char c0 = 0;
    unsigned char c1 = 0;
    uint16_t Checksum;

    // Calculate checksum intermediate bytes
    for( i = 0; i < len; i++)
    {
        c0 = (unsigned char)(c0 + (unsigned char) data[i]);
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


CEDATPacket::CEDATPacket(uint8_t *buffer, int length)
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

    int offset = 0;

    if (length < 6)
    {
        qWarning() << "CEDATPacket constructor called with only " + QString::number(length) + " bytes";
        return;
    }

    // Extract packet type and packet flags
    pktType = buffer[0] & 0xFF;
    pktFlags = buffer[1] & 0xFF;

    // How many timestamp bytes?
    int nTsBytes = pktFlags & 0x0F;

    if (length < (6 + nTsBytes))
    {
        qWarning() << "CEDATPacket called with insufficient bytes after timestamp (" + QString::number(length) + ")";
        return;
    }

    for (int ii = 0; ii < nTsBytes; ii++)
    {
        uint8_t tsByte = buffer[2 + ii];

        // Timestamp is "little endian" encoded
        pktTimestamp += ((uint64_t) tsByte & 0xFF) << (8 * ii);
    }

    offset = nTsBytes + 2;

    // Packet length is "big endian" encoded
    pktLength  = (uint16_t) (buffer[offset++] & 0xFF) << 8;
    pktLength += (uint16_t) (buffer[offset++] & 0xFF);

    if (length < (6 + nTsBytes + pktLength))
    {
        qWarning() << "CEDATPacket called with insufficient bytes after length (" + QString::number(length) + ")";
        return;
    }

    for (int idx = 0; idx < pktLength; idx++)
    {
        pktData[idx] = buffer[offset++] & 0xFF;
    }

    // Calculate the checksum across number of bytes received so far
    uint16_t checksum = fletcher_encode(buffer, offset);

    // Calculate checksum - "big endian" encoded
    pktChecksum  = (uint16_t) (buffer[offset++] & 0xFF) << 8;
    pktChecksum += (uint16_t) (buffer[offset++] & 0xFF);

    valid = pktChecksum == checksum;

    if (!valid)
    {
        qWarning() << "Checksum mismatch:" << QString::number(pktChecksum, 16) << QString::number(checksum, 16);

        QByteArray pkt((char*) buffer, length);
        qWarning() << "Payload:" << pkt.toHex(' ');
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

    qint64 fileSize = getFileSize();

    // Open the file object
    if (!f.exists() || !f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
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

    QByteArray chunk;

    // Read out data a chunk at a time
    while (!f.atEnd() && !progress.wasCanceled())
    {
        chunk = f.read(CHUNK_SIZE);

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
        errors.append(tr("File import was cancelled"));
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
    // Allocate a buffer for decoding cobs data
    static uint8_t block_buffer[CEDAT_PKT_SIZE_MAX];

    // Ignore empty block
    if (blockData.isEmpty()) return;


    // Cobbs-decode the data
    auto result = cobsr_decode(
                block_buffer,
                CEDAT_PKT_SIZE_MAX,
                reinterpret_cast<const uint8_t*>(blockData.constData()),
                blockData.count()
    );

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

    processPacket(block_buffer, result.out_len);
}


void CEDATImporter::processPacket(uint8_t *buffer, int length)
{
    // Convert the QByteArray data into a structured packet
    CEDATPacket packet(buffer, length);

    // Various data structures
    CEDAT_NewLogVariable_t newVariable;
    CEDAT_TimestampedData_t newDataFloat;
    CEDAT_TimestampedBooleanData_t newDataBoolean;

    if (!packet.valid)
    {
        QByteArray pkt((char*) buffer, length);
        qWarning() << "Invalid packet:" << pkt.toHex(' ');
        return;
    }

    // CEDAT packet timestamps are specified in milliseconds
    double timestamp = (double) packet.pktTimestamp / 1000;

    if (newVariable.decode(&packet))
    {
        QString group = QString(newVariable.ownerName);

        // Add a new variable
        DataSeries *series = new DataSeries();

        QString label = QString(newVariable.ownerName) + "_" + QString::number(newVariable.ownerId) + ":" + QString(newVariable.title);

        series->setGroup(group);
        series->setLabel(label);
        series->setUnits(newVariable.units);

        // Store a local copy of this data series, for "immediate" access
        variableMap[newVariable.variableId] = QSharedPointer<DataSeries>(series);

        addSeries(series);
    }
    else if (newDataFloat.decode(&packet))
    {
        if (variableMap.contains(newDataFloat.variableId))
        {

            auto series = variableMap[newDataFloat.variableId];

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

            auto series = variableMap[newDataBoolean.variableId];

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


