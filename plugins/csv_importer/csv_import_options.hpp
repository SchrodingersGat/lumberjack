#ifndef CSV_IMPORT_OPTIONS_HPP
#define CSV_IMPORT_OPTIONS_HPP

#include <QString>


/**
 * @brief The CSVImportOptions class defines various import options for CSV files
 */
struct CSVImportOptions
{
    enum TimestampFormat {
        SECONDS = 0,
        MILLISECONDS = 1,
        HHMMSS = 2,
    };

    enum DelimiterType {
        COMMA,
        TAB,
        COLON,
        SEMICOLON,
        PIPE,
        SPACE,
    };

    /* Import Options */

    //! Offset timestamps such that first timestamp = zero
    bool zeroTimestamp = false;

    bool hasTimestamp = true;
    bool hasHeaders = true;
    bool hasUnits = false;

    int colTimestamp = 0;
    int rowHeaders = 0;
    int rowUnits = 0;

    TimestampFormat timestampFormat = TimestampFormat::SECONDS;
    DelimiterType delimeter = DelimiterType::COMMA;

    QString ignoreRowsStartingWith;
    QString ignoreColsStartingWith;

};

#endif // CSV_IMPORT_OPTIONS_HPP
