#include <qprogressdialog.h>
#include <qelapsedtimer.h>
#include <qapplication.h>

#include "mavlink_importer.hpp"


uint8_t MavlinkFormatMessage::messageType()
{
    if (dataBytes.length() > 0)
    {
        return dataBytes.at(0) & 0xFF;
    }
    else
    {
        return 0;
    }
}


/*
 * Return the expected data length for a particular message type
 */
uint8_t MavlinkFormatMessage::messageLength()
{
   if (dataBytes.length() > 1)
   {
       return dataBytes.at(1) & 0xFF;
   }
   else
   {
       return 0;
   }
}


/*
 * Return the "name" of the packet from a format message.
 * This is the first four bytes of the format payload
 */
QString MavlinkFormatMessage::messageName()
{
   return QString(dataBytes.mid(2, 4));
}


/*
 * Return a list of message formats associated with this message format
 */
QList<char> MavlinkFormatMessage::messageFormats()
{
    QList<char> formats;

    for (auto byte : dataBytes.mid(6, 16))
    {
        char fmt = (char) (byte & 0xFF);

        if (fmt != 0x00)
        {
            formats.append(fmt);
        }
    }

    return formats;
}


/*
 * Return a list of labels associated with this message format
 */
QStringList MavlinkFormatMessage::messageLabels()
{
    QStringList labels;

    for (auto label: QString(dataBytes.mid(22, 64)).split(","))
    {
        if (!label.isEmpty())
        {
            labels.append(label);
        }
    }

    return labels;
}



MavlinkImporter::MavlinkImporter() : FileDataSource("Mavlink Importer")
{
    // TODO
}


MavlinkImporter::~MavlinkImporter()
{
    // TODO
}


/*
 * Return a list of support file extensions for this importer class
 */
QStringList MavlinkImporter::getSupportedFileTypes() const
{
    QStringList fileTypes;

    fileTypes << "bin";

    return fileTypes;
}


/*
 * Specify mavlink import options
 */
bool MavlinkImporter::setImportOptions()
{
    // TODO: Maybe a custom dialog here?
    return true;
}


/*
 * Read mavlink data from the selected file
 */
bool MavlinkImporter::loadDataFromFile(QStringList &errors)
{
    QFile f(filename);

    qint64 fileSize = getFileSize();

    if (fileSize < 100)
    {
        errors.append(tr("File size is too small"));
        return false;
    }

    // Attempt to open the file
    if (!f.exists() || !f.open(QIODevice::ReadOnly) || !f.isOpen() || !f.isReadable())
    {
        errors.append(tr("Could not open file for reading"));
        f.close();
        return false;
    }

    QElapsedTimer elapsed;
    QProgressDialog progress;

    progress.setWindowTitle(tr("Reading file"));
    progress.setLabelText(tr("Reading data - ") + filename);

    progress.setMinimum(0);
    progress.setMaximum(fileSize);
    progress.setValue(0);

    elapsed.restart();
    progress.show();

    f.seek(0);

    QByteArray bytes;

    int64_t byteCount = 0;

    while (!f.atEnd() && !progress.wasCanceled())
    {
        // Read a chunk of bytes
        bytes = f.read(2048);

        processChunk(bytes);

        byteCount += bytes.length();

        // Update the progress bar periodically
        if (elapsed.elapsed() > 250)
        {
            progress.setValue(byteCount);
            QApplication::processEvents();
            elapsed.restart();
        }
    }

    // Ensure the file is closed
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

    qDebug() << "Processed" << byteCount << "bytes from" << filename;
    qDebug() << "Found" << messageCount << "messages";

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
                    messageFormats[format.messageType()] = format;
                }
                break;
            default:
                // Check if we know about this message
                if (messageFormats.contains(messageId))
                {
                    format = messageFormats[messageId];

                    // Check there is enough data
                    if (data.count() < format.messageLength())
                    {
                        qWarning() << format.messageName() << "missing data:" << data.count() << format.messageLength();
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
    if (format.messageName().length() == 0)
    {
        qWarning() << "Invalid format message - zero length name";
        return false;
    }

    if (format.messageFormats().length() == 0)
    {
        qWarning() << format.messageName() << "Invalid format message - zero length format";
        return false;
    }

    if (format.messageLabels().length() == 0)
    {
        qWarning() << format.messageName() << "Invalid format message - zero length labels";
        return false;
    }

    if (format.messageLabels().length() != format.messageFormats().length())
    {
        qWarning() << format.messageName() << "Invalid format message - mismatch between format and label length";
        qDebug() << format.messageFormats() << format.messageLabels();

        return false;
    }

    return true;
}


/*
 * Decode a "data" packet, with a provided format
 */
void MavlinkImporter::processData(MavlinkFormatMessage &format, QByteArray bytes)
{
    // Extract data from the message
    QStringList labels = format.messageLabels();
    QList<char> formats = format.messageFormats();

    QString msgName = format.messageName();
    QString graphName;

    double timestamp = 0;
    double value = 0;
    QSharedPointer<DataSeries> series;

    for (int idx = 0; idx < labels.count(); idx++)
    {
        graphName = msgName + ":" + labels.at(idx);

        char fmt = formats.at(idx);

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
            // Find graph matching each label
            series = getSeriesByLabel(graphName);

            if (series.isNull())
            {
                addSeries(graphName);
                series = getSeriesByLabel(graphName);
            }

            series->addData(timestamp, value);
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


/*
 * Process a single byte, and return true if a valid message was completed
 */
bool MavlinkImporter::processByte(const char &byte)
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

        // Determine the "expected length" based on the msgId type
        switch (messageId)
        {
        case MavlinkImporter::MSG_ID_FORMAT:
            // Format message defines a "new packet format"
            // Length = 1 + 1 + 4 + 16 + 64
            expectedLength = 86;
            break;
        default:
            // Check if this message is one we have heard about
            if (messageFormats.contains(messageId))
            {
                expectedLength = messageFormats[messageId].messageLength();
            }
            else
            {
//                qWarning() << "Unknown message:" << messageId;
                reset();
            }
            break;
        }

        break;
    case MSG_DATA:
        data.append(byte);

        if (data.length() >= expectedLength)
        {
            result = true;

            // Reset state machine
            state = HEAD_1;
        }

        break;
    }

    return result;
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

    for (int idx = 0; idx < bytes.length() && idx < 8; idx++)
    {
        uint64_t v = bytes.at(idx) & 0xFF;
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
