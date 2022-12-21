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
                        qDebug() << format.messageName() << "missing data:" << data.count() << format.messageLength();
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
void MavlinkImporter::processData(MavlinkFormatMessage &format, QByteArray &bytes)
{
    // Check that the

    // Extract data from the message
    QStringList labels = format.messageLabels();
    QList<char> formats = format.messageFormats();

    QString msgName = format.messageName();
    QString graphName;

    double timestamp;
    double value;
    QSharedPointer<DataSeries> series;

    // Extract timestamp (64-bit, microsecond units)


    for (int idx = 0; idx < labels.count(); idx++)
    {
        graphName = msgName + ":" + labels.at(idx);

        // Find graph matching each label
        series = getSeriesByLabel(graphName);

        if (series.isNull())
        {
            addSeries(graphName);
            series = getSeriesByLabel(graphName);
        }

        char fmt = formats.at(idx);
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
