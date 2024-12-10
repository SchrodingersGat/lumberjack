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
        COMMA = 0,
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

    QString getDelimiterString(void) const
    {
        switch (delimeter)
        {
        default:
        case DelimiterType::COMMA:
            return ",";
        case DelimiterType::TAB:
            return "\t";
        case DelimiterType::COLON:
            return ":";
        case DelimiterType::SEMICOLON:
            return ";";
        case DelimiterType::PIPE:
            return "|";
        case DelimiterType::SPACE:
            return " ";
        }
    }

    double getTimestampScaler(void) const
    {
        switch (timestampFormat)
        {
        default:
        case TimestampFormat::SECONDS:
        case TimestampFormat::HHMMSS:
            return 1.0;
        case TimestampFormat::MILLISECONDS:
            return 0.001;
        }
    }
};

#endif // CSV_IMPORT_OPTIONS_HPP
