#ifndef CEDAT_IMPORTER_HPP
#define CEDAT_IMPORTER_HPP


#include "data_source.hpp"

#include "CEDATProtocol.hpp"


class CEDATPacket
{
public:
    CEDATPacket();
    CEDATPacket(QByteArray payload);

    // Maximum packet size of 1024 bytes cannot be exceeded
    uint8_t pktData[1024];
    uint8_t pktType = 0;
    uint16_t pktLength = 0;
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

    // A single "block" of data (separated in the file by zero)
    QByteArray block;
};


#endif // CEDAT_IMPORTER_HPP
