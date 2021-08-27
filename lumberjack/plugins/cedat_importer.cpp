#include <qfile.h>

#include "CEDATProtocol.hpp"
#include "cedat_importer.hpp"


CEDATPacket::CEDATPacket()
{
    memset(pktData, 0, sizeof(pktData));
    pktLength = 0;
}


CEDATPacket::CEDATPacket(QByteArray payload)
{
    memset(pktData, 0, sizeof(pktData));

    for (int idx = 0; idx < payload.length(); idx++)
    {
        pktData[idx] = (uint8_t) payload.at(idx);
    }

    pktLength = payload.length();
}



CEDATImporter::CEDATImporter() : FileDataSource("CEDAT Importer")
{

}


CEDATImporter::~CEDATImporter()
{
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

    // Open the file object
    if (!f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
    {
        errors.append(tr("Could not open file for reading"));
        f.close();
        return false;
    }

    // Ensure we are at the start of the file!
    f.seek(0);

    const int CHUNK_SIZE = 4096;

    int byte_count = 0;
    int chunk_count = 0;

    QByteArray chunk;

    // Read out data a chunk at a time
    while (!f.atEnd())
    {
        chunk = f.read(CHUNK_SIZE);

        chunk_count += 1;
        byte_count += chunk.size();

        if (chunk.size() > 0)
        {
            processChunk(chunk);
        }
    }

    // Ensure the file object is closed
    f.close();

    qDebug() << "Read" << byte_count << "bytes in" << chunk_count << "chunks";

    return false;
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
            block.clear();
        }
        else
        {
            block.append(byte);
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
    if (block.isEmpty()) return;

    qDebug() << "block:" << block;
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


