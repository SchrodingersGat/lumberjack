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
