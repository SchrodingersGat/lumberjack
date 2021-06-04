## Qt

Qt 5.12.2

## Qwt

Lumberjack makes use of the [Qt Widgets for Technical Applications (QWT) framework](https://qwt.sourceforge.io/).

To compile the Lumberjack program you will need to download [qwt-6.1.4](https://sourceforge.net/projects/qwt/files/qwt/6.1.4/) from the [qwt download page](https://sourceforge.net/projects/qwt/files/qwt/).

Extract the qwt source files into the top level Lumberjack directory (e.g. ~/lumberjack/qwt-6.1.4).

## Unit Testing

Unit testing uses the [googletest](https://google.github.io/googletest/) framework.

To build and run unit tests:

```
cd test
cmake -S . -B build
cmake --build build
cd build
ctest
```
