
# Lumberjack

No log is too tough!

<img style="float: right;" src="logo/lumberjack.png" alt="Lumberjack" width="200"/>

Lumberjack is a multi-platform log parsing and data plotting tool. Designed to quickly parse and navigate large data logs, Lumberjack provides simple, repeatable management of your logs.

Designed around a configurable plugin system, Lumberjack is designed to be extensible. Do you use a proprietary file format or need to perform a particular pre-processing step? Plugins can be easily designed to meet your needs.

## Architecture

Written in C++ and supported by the Qt / Qwt ecosystem, Lumberjack is blazingly fast, allowing rapid exploration of large data sets.

### Scripting

An integrated Python scripting console provides access to loaded data, allowing easy hacking and adjusting of data as required.

## Dependencies

### Qt

Qt 5.12.2

### Qwt

Lumberjack makes use of the [Qt Widgets for Technical Applications (QWT) framework](https://qwt.sourceforge.io/).

To compile the Lumberjack program you will need to download [qwt-6.1.4](https://sourceforge.net/projects/qwt/files/qwt/6.1.4/) from the [qwt download page](https://sourceforge.net/projects/qwt/files/qwt/).

Extract the qwt source files into the top level Lumberjack directory (e.g. ~/lumberjack/qwt-6.1.4).

## Unit Testing

Unit testing uses the [QTestLib framework](https://doc.qt.io/qt-5/qtest-overview.html).
