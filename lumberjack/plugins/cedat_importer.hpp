#ifndef CEDAT_IMPORTER_HPP
#define CEDAT_IMPORTER_HPP

#include <qmap.h>

#include "data_source.hpp"

#include "CEDATProtocol.hpp"


class CEDATPacket
{
public:
    CEDATPacket();
    CEDATPacket(QByteArray payload);

    uint8_t pktData[1040];
    uint8_t pktType = 0;
    uint8_t pktFlags = 0;
    uint16_t pktLength = 0;
    uint16_t pktChecksum = 0;
    uint64_t pktTimestamp = 0;

    bool valid = false;

protected:
    void reset();
};


class CEDATImporter : public FileDataSource
{
    Q_OBJECT

public:
    CEDATImporter();

    virtual ~CEDATImporter();

    virtual QString getFileDescription(void) const override { return "CEDAT files"; }
    virtual QStringList getSupportedFileTypes(void) const override;
    virtual bool loadDataFromFile(QStringList& errors) override;

protected:
    void processChunk(const QByteArray& chunk);
    void processBlock();
    void processPacket(const QByteArray& packet);

    // A single "block" of data (separated in the file by zero)
    QByteArray blockData;

    // Keep an internal map of variable IDs for matching to existing data series
    QMap<int, QString> variableMap;
};


#endif // CEDAT_IMPORTER_HPP
