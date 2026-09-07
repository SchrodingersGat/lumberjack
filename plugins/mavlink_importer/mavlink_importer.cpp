#include <QFile>
#include <QFileInfo>

#include "mavlink_importer.hpp"


/*
 * Construct a MavlinkFormatMessage object from a series of bytes
 */
MavlinkFormatMessage::MavlinkFormatMessage(QByteArray bytes)
{
    // Extract message type
    if (bytes.length() > 0)
    {
        type = bytes.at(0) & 0xFF;
    }

    // Extract message length
    if (bytes.length() > 1)
    {
        length = bytes.at(1) & 0xFF;
    }

    // Extract message name
    name = QString(bytes.mid(2, 4));

    // Extract message format specifier
    formatString = QString(bytes.mid(6, 16));

    // Extract data labels
    for (auto label : QString(bytes.mid(22, 64)).split(","))
    {
        if (!label.isEmpty())
        {
            labels.append(label);
        }
    }
}


MavlinkImporter::MavlinkImporter() : ImportPlugin()
{
}



/*
 * Return a list of support file extensions for this importer class
 */
QStringList MavlinkImporter::supportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes << "bin";

    return fileTypes;
}


/*
 * Specify mavlink import options
 */
bool MavlinkImporter::beforeImport()
{
    // TODO: Maybe a custom dialog here?
    return true;
}


/*
 * Read mavlink data from the selected file
 */
bool MavlinkImporter::importData(QStringList &errors)
{
    QFileInfo fi(m_filename);

    if (!fi.exists() || !fi.isFile())
    {
        errors.append(tr("File does not exist"));
        return false;
    }

    m_bytesRead = 0;
    m_fileSize = fi.size();

    if (m_fileSize < 100)
    {
        errors.append(tr("File size is too small"));
        return false;
    }

    QFile f(m_filename);

    // Attempt to open the file
    if (!f.exists() || !f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
    {
        errors.append(tr("Could not open file for reading"));
        f.close();
        return false;
    }

    f.seek(0);

    QByteArray bytes;

    // Check the header bytes
    bytes = f.read(2);

    if ((bytes.at(0) != HEAD_BYTE_1) ||
        (bytes.at(1) != HEAD_BYTE_2))
    {
        errors.append(tr("Header bytes do not match expected format"));
        return false;
    }

    f.seek(0);

    m_isImporting = true;


    while (!f.atEnd() && m_isImporting)
    {
        // Read a chunk of bytes
        bytes = f.read(2048);

        processChunk(bytes);

        m_bytesRead += bytes.length();
    }

    // Ensure the file is closed
    f.close();

    qDebug() << "Decoded" << messageCount << "messages from" << m_filename;

    m_isImporting = false;

    return true;
}


/*
 * Process a chunk of data from the file
 */
void MavlinkImporter::processChunk(const QByteArray &bytes)
{
    MavlinkFormatMessage format;

    // Iterate through each byte
    for (const char byte : bytes)
    {
        if (processByte(byte))
        {
            messageCount++;

            switch (messageId)
            {
            case MavlinkImporter::MSG_ID_FORMAT:
                format = MavlinkFormatMessage(data);

                // Validate the format
                if (validateFormatMessage(format))
                {
                    // Store this format message for later on
                    // Use the messageType identifier for lookup
                    messageFormats[format.type] = format;
                }
                break;
            default:
                // Check if we know about this message
                if (messageFormats.contains(messageId))
                {
                    format = messageFormats[messageId];

                    // Check there is enough data
                    if (data.count() < format.length)
                    {
                        qWarning() << format.name << "missing data:" << data.count() << format.length;
                    }
                    else
                    {
                        processData(messageFormats[messageId], data);
                    }
                }
                break;
            }
        }
    }
}


/*
 * Check if a decoded format message is valid:
 * - Name must be non-zero length
 * - Format must be non-zero length
 * - Labels must be non-zero length
 * - Format length must match labels length
 */
bool MavlinkImporter::validateFormatMessage(MavlinkFormatMessage &format)
{
    if (format.length == 0)
    {
        qWarning() << "Invalid format message - zero length name";
        return false;
    }

    if (format.formatString.length() == 0)
    {
        qWarning() << format.formatString << "Invalid format message - zero length format";
        return false;
    }

    if (format.labels.length() == 0)
    {
        qWarning() << format.labels << "Invalid format message - zero length labels";
        return false;
    }

    if (format.labels.length() != format.formatString.length())
    {
        qWarning() << format.name << "Invalid format message - mismatch between format and label length";
        qDebug() << format.formatString << format.labels;

        return false;
    }

    return true;
}


/*
 * Decode a "data" packet, with a provided format
 */
void MavlinkImporter::processData(const MavlinkFormatMessage &format, QByteArray &bytes)
{
    QString graphName;

    double timestamp = 0;
    double value = 0;

    QSharedPointer<DataSeries> series;

    int n = format.labels.count();

    for (int idx = 0; idx < n; idx++)
    {
        graphName = format.name + ":" + format.labels[idx];

        char fmt = format.formatString[idx].toLatin1();

        switch (fmt)
        {
        case 'a':   // int16_t[32]
            // Currently unimplemented
            continue;
        case 'b':   // int8_t
            value = extractInt8(bytes);
            break;
        case 'B':   // uint8_t
            value = extractUInt8(bytes);
            break;
        case 'h':   // int16_t
            value = extractInt16(bytes);
            break;
        case 'H':   // uint16_t
            value = extractUInt16(bytes);
            break;
        case 'i':   // int32_t
            value = extractInt32(bytes);
            break;
        case 'I':   // uint32_t
            value = extractUInt32(bytes);
            break;
        case 'f':   // float
            value = extractFloat(bytes);
            break;
        case 'd':   // double
            value = extractDouble(bytes);
            break;
        case 'n':   // char[4]
            // Currently unimplemented
            continue;
        case 'N':   // char[16]
            // Currently unimplemented
            continue;
        case 'Z':   // char[16]
            // Currently unimplemented
            continue;
        case 'c':   // int16_t * 100
            value = (double) extractInt16(bytes) / 100;
            break;
        case 'C':   // uint16_t * 100
            value = (double) extractUInt16(bytes) / 100;
            break;
        case 'e':   // int32_t * 100
            value = (double) extractInt32(bytes) / 100;
            break;
        case 'E':   // uint32_t * 100
            value = (double) extractUInt32(bytes) / 100;
            break;
        case 'L':   // int32_t lat/lng
            // Currently unimplemented
            continue;
        case 'M':   // uint8_t flight mode
            // Currently unimplemented
            continue;
        case 'q':   // int64_t
            value = extractInt64(bytes);
            break;
        case 'Q':   // uint64_t
            value = extractUInt64(bytes);
            break;
        default:
            qWarning() << graphName << "invalid format:" << fmt;
            return;
        }

        // First element is always the timestamp
        if (idx == 0x00)
        {
            // Convert from microseconds to seconds
            timestamp = value / 1000 / 1000;
        }
        else
        {
            if (m_columnMap.contains(graphName))
            {
                series = m_columnMap.value(graphName);
            }
            else
            {
                series = QSharedPointer<DataSeries>::create(graphName);
                m_columnMap.insert(graphName, series);
            }

            if (!series.isNull())
            {
                series->addData(timestamp, value);
            }
        }
    }
}


/*
 * Reset the log message parser to an initial state
 */
void MavlinkImporter::reset()
{
    data.clear();
    state = HEAD_1;
}


void MavlinkImporter::afterImport(void)
{
}


void MavlinkImporter::cancelImport(void)
{
    m_isImporting = false;
}


uint8_t MavlinkImporter::getImportProgress(void) const
{
    if (!m_isImporting) return 0;

    if (m_bytesRead == 0 || m_fileSize == 0) return 0;

    float progress = (float) m_bytesRead / (float) m_fileSize;

    return (uint8_t) (progress * 100);
}


/**
 * Return the list of imported data series
 */
QList<QSharedPointer<DataSeries>> MavlinkImporter::getDataSeries(void) const
{
    return m_columnMap.values();
}


/*
 * Process a single byte, and return true if a valid message was completed
 */
bool MavlinkImporter::processByte(const char byte)
{
    bool result = false;

    switch (state)
    {
    default:
    case HEAD_1:
        if (byte == HEAD_BYTE_1) state = HEAD_2;
        break;
    case HEAD_2:
        if (byte == HEAD_BYTE_2) state = MSG_ID;
        break;
    case MSG_ID:
        messageId = (uint8_t) byte;
        state = MSG_DATA;

        data.clear();

        state = MSG_LENGTH;
        break;

        //         // Determine the "expected length" based on the msgId type
        //         switch (messageId)
        //         {
        //         case MavlinkImporter::MSG_ID_FORMAT:
        //             // Format message defines a "new packet format"
        //             // Length = 1 + 1 + 4 + 16 + 64
        //             expectedLength = 86;
        //             break;
        //         default:
        //             // Check if this message is one we have heard about
        //             if (messageFormats.contains(messageId))
        //             {
        //                 expectedLength = messageFormats[messageId].length;
        //             }
        //             else
        //             {
        // //                qWarning() << "Unknown message:" << messageId;
        //                 reset();
        //             }
        //             break;
        //         }

        break;
    case MSG_LENGTH:
        expectedLength = byte;
        state = MSG_DATA;
        break;
    case MSG_DATA:
        data.append(byte);

        if (data.length() >= expectedLength)
        {
            // result = true;
            state = MSG_CRC;
        }
        break;
    case MSG_CRC:
        qDebug() << "pkt:" << messageId << expectedLength;

        QString msg;

        for ( auto d : data)
        {
            msg += "0x" + QString::number((uint8_t) d, 16).rightJustified(2, '0').toUpper() + " ";
        }

        qDebug() << "data:" << msg;
        qDebug() << "CRC:" << (uint8_t) byte << "->" << messageChecksum();
        state = HEAD_1;

        result = true;
        break;
    }

    return result;
}


/*
 * Calculate XOR checksum across the message
 * - Message type
 * - Expected Length
 * - Message data
 */
uint8_t MavlinkImporter::messageChecksum(void) const
{
    uint8_t chk = messageId;

    chk ^= expectedLength;

    for (int ii = 0; ii < data.length() && ii < expectedLength; ii++)
    {
        chk ^= (uint8_t) data[ii];
    }

    return chk;
}


/*
 * Extract a 64-bit number from the front of the data array.
 * Will remove the bytes from the array also
 */
uint64_t MavlinkImporter::extractUInt64(QByteArray &data, int byteCount)
{
    // Extract first bytes from data, and remove from original data
    QByteArray bytes = data.mid(0, byteCount);
    data.remove(0, byteCount);

    uint64_t value = 0;

    int n = bytes.length();

    for (int idx = 0; idx < n && idx < 8; idx++)
    {
        uint64_t v = bytes[idx] & 0xFF;
        v <<= (idx * 8);

        value += v;
    }

    return value;
}


/*
 * Extract a signed 64 bit value from the data array
 */
int64_t MavlinkImporter::extractInt64(QByteArray &data)
{
    uint64_t v = extractUInt64(data);

    return *(int64_t*)&v;
}


/*
 * Extract an unsigned 32 bit value from the data array
 */
uint32_t MavlinkImporter::extractUInt32(QByteArray &data)
{
    return (uint32_t) extractUInt64(data, 4);
}


/*
 * Extract a signed 32 bit value from the data array
 */
int32_t MavlinkImporter::extractInt32(QByteArray &data)
{
    uint32_t v = extractUInt32(data);

    return *(int32_t*) &v;
}


/*
 * Extract an unsigned 16 bit value from the data array
 */
uint16_t MavlinkImporter::extractUInt16(QByteArray &data)
{
    return (uint16_t) extractUInt64(data, 2);
}


/*
 * Extract a signed 16 bit value from the data array
 */
int16_t MavlinkImporter::extractInt16(QByteArray &data)
{
    uint16_t v = extractUInt16(data);

    return *(int16_t*) &v;
}


/*
 * Extract an unsigned 8 bit value from the data array
 */
uint8_t MavlinkImporter::extractUInt8(QByteArray &data)
{
    return (uint8_t) extractUInt64(data, 1);
}


/*
 * Extract a signed 8 bit value from the data array
 */
int8_t MavlinkImporter::extractInt8(QByteArray &data)
{
    uint8_t v = extractUInt8(data);

    return *(int8_t*) &v;
}


/*
 * Extract a float value from the data array
 */
float MavlinkImporter::extractFloat(QByteArray &data)
{
    uint32_t v = extractUInt32(data);

    return *(float*) &v;
}


/*
 * Extract a double value from the data array
 */
double MavlinkImporter::extractDouble(QByteArray &data)
{
    uint64_t v = extractUInt64(data);

    return *(double*) &v;
}
