/*
 * Data importer for mavlink log files (.bin)
 *
 * This plugin provides support for ardupilot/mavlink logs,
 * which would otherwise be viewed using mavexplorer.
 *
 * References:
 * - https://discuss.ardupilot.org/t/log-file-format/49089
 */

#ifndef MAVLINK_IMPORTER_HPP
#define MAVLINK_IMPORTER_HPP

#include "data_source.hpp"


/*
 * Generic message class
 */
class MavlinkLogMessage
{
public:
    MavlinkLogMessage();

    void reset();
    bool processByte(const char& byte);

    // Byte array containing data
    QByteArray data;

    uint8_t msgId = 0;
    uint8_t msgType = 0;
    uint8_t msgLength = 0;

protected:

    // Possible data states
    enum FileParsingState
    {
        HEAD_1,
        HEAD_2,
        MSG_ID,
        MSG_TYPE,
        MSG_LENGTH,
        MSG_DATA,
    };

    // Internal state variable
    uint16_t state = HEAD_1;

    // Expected header bytes (according to ArduPilot spec)
    const char HEAD_BYTE_1 = 0xA3;
    const char HEAD_BYTE_2 = 0x95;
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

    MavlinkLogMessage message;

    int messageCount = 0;
};


#endif // MAVLINK_IMPORTER_HPP
