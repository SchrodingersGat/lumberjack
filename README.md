
# Lumberjack

No log is too tough!

<img style="float: right;" src="logo/lumberjack.png" alt="Lumberjack" width="200"/>

Lumberjack is a multi-platform log parsing and data plotting tool. Designed to quickly parse and navigate large data logs, Lumberjack provides simple, repeatable management of your logs.

Designed around a configurable plugin system, Lumberjack is designed to be extensible. Do you use a proprietary file format or need to perform a particular pre-processing step? Plugins can be easily designed to meet your needs.

## Architecture

Written in C++ and supported by the Qt / Qwt ecosystem, Lumberjack is blazingly fast, allowing rapid exploration of large data sets.

### Plugins

TODO: Plugin support

### Scripting

An integrated Python scripting console provides access to loaded data, allowing easy hacking and adjusting of data as required.

## Installing

### Windows

TODO: Windows installer

### Linux

TODO: Linux package

### MacOS

TODO: MacOS package

## Dependencies

### Qt

Lumberjack is built using the [Qt framework](https://www.qt.io/). To compile the Lumberjack program you will first need to download and install the Qt framework from the [Qt download page](https://www.qt.io/download).

### Qwt

Lumberjack makes use of the [Qt Widgets for Technical Applications (QWT) framework](https://qwt.sourceforge.io/).

### Simple-FFT

We integrate the [Simple-FFT](https://github.com/d1vanov/Simple-FFT) library for fast Fourier transform (FFT) support.

### Python

Lumberjack uses Python for scripting. A Python interpreter is embedded in the Lumberjack application.
