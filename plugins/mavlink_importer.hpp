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
 * Class representing a "format group" within the log structure.
 * Each "format group" has information about the data variables within that group.
 */
class MavlinkGroup
{

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

    enum FileParsingState
    {
        INIT = 0,       // Initially loading file
    };

};


#endif // MAVLINK_IMPORTER_HPP
