/*
 * Data importer for mavlink log files (.bin)
 *
 * This plugin provides support for ardupilot/mavlink logs,
 * which would otherwise be viewed using mavexplorer.
 *
 * References:
 * - https://discuss.ardupilot.org/t/log-file-format/49089
 * - https://github.com/ArduPilot/ardupilot/blob/master/libraries/AP_Logger/LogStructure.h
 */

#ifndef MAVLINK_IMPORTER_HPP
#define MAVLINK_IMPORTER_HPP

#include "data_source.hpp"


/*
 * Class representing a "format" message
 */
class MavlinkFormatMessage
{
public:
    MavlinkFormatMessage() {}
    MavlinkFormatMessage(QByteArray bytes) : dataBytes(bytes) {}

    QString messageName();
    QString messageFormat();
    QString messageUnits();

    uint8_t messageType();
    uint8_t messageLength();

protected:
    QByteArray dataBytes;
};


class MavlinkImporter: public FileDataSource
{
    Q_OBJECT

public:
    MavlinkImporter();
    virtual ~MavlinkImporter();

    virtual QString getFileDescription(void) const override { return "Mavlink Log Files"; }
    virtual QStringList getSupportedFileTypes(void) const override;

    virtual bool setImportOptions(void) override;
    virtual bool loadDataFromFile(QStringList& errors) override;

protected:

    void processChunk(const QByteArray &bytes);
    void processData(MavlinkFormatMessage &format, QByteArray &bytes);

    int messageCount = 0;

    // Expected header bytes (according to ArduPilot spec)
    const char HEAD_BYTE_1 = 0xA3;
    const char HEAD_BYTE_2 = 0x95;

    // Possible data states
    enum FileParsingState
    {
        HEAD_1,
        HEAD_2,
        MSG_ID,
        MSG_DATA,
    };

    // Known message identifiers
    enum
    {
        MSG_ID_FORMAT = 128,
    };

    // Internal state variable
    uint16_t state = HEAD_1;

    uint8_t messageId = 0;

    int expectedLength = 0;

    // Byte array containing incoming data
    QByteArray data;

    void reset();
    bool processByte(const char& byte);

    QMap<int, MavlinkFormatMessage> messageFormats;
};


#endif // MAVLINK_IMPORTER_HPP
