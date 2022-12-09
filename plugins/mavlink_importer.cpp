#include <qprogressdialog.h>
#include <qelapsedtimer.h>
#include <qapplication.h>

#include "mavlink_importer.hpp"


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
    // Iterate through each byte
    for (const char byte : bytes)
    {
        if (message.processByte(byte))
        {
            messageCount++;
            // TODO: Actually "do" something with the processes message
        }
    }
}


MavlinkLogMessage::MavlinkLogMessage()
{
    reset();
}


/*
 * Reset the log message parser to an initial state
 */
void MavlinkLogMessage::reset()
{
    data.clear();
    state = HEAD_1;
}


/*
 * Process a single byte, and return true if a valid message was completed
 */
bool MavlinkLogMessage::processByte(const char &byte)
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
        msgId = (uint8_t) byte;
        state = MSG_TYPE;
        break;
    case MSG_TYPE:
        msgType = (uint8_t) byte;
        state = MSG_LENGTH;
        break;
    case MSG_LENGTH:
        msgLength = (uint8_t) byte;
        data.clear();
        state = MSG_DATA;
        break;
    case MSG_DATA:
        data.append(byte);

        if (data.length() >= msgLength)
        {
            result = true;

            // Reset state machine
            state = HEAD_1;
        }
        break;
    }

    return result;
}
