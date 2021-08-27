#ifndef CEDAT_IMPORTER_HPP
#define CEDAT_IMPORTER_HPP


#include "data_source.hpp"


class CEDATImporter : public FileDataSource
{
    Q_OBJECT

public:
    CEDATImporter();

    virtual ~CEDATImporter();

    virtual QString getFileDescription(void) const override { return "CEDAT files"; }
    virtual QStringList getSupportedFileTypes(void) const override;
    virtual bool loadDataFromFile(QStringList& errors) override;
};


#endif // CEDAT_IMPORTER_HPP
