## Building Lumberjack

## Prerequisites

The build guide assumes that you have the following installed:

- [Qt](https://www.qt.io/)
- [Qt Creator](https://www.qt.io/product/development-tools)
- A compatible C++ compiler and build system (e.g. [MinGW](https://www.mingw-w64.org/), [MSVC](https://visualstudio.microsoft.com/), [GCC](https://gcc.gnu.org/))

Ensure that all these tools are installed, and the required paths are added to your system's PATH environment variable.

### Submodules

Lumberjack uses a number of submodules to provide additional functionality. To ensure that these are downloaded, clone the repository with the `--recurse-submodules` flag:

- [Qwt](https://qwt.sourceforge.io/)
- [Simple-FFT](https://github.com/d1vanov/Simple-FFT)

Note: These submodules are pulled down when cloning the repository, and do not need to be manually downloaded.

## Get Source Code

Clone the Lumberjack repository from GitHub:

```bash
git clone https://github.com/SchrodingersGat/lumberjack.git --recurse-submodules
```

### Build Qwt

If this is the first time you are building the project, you will need to build the Qwt library. 

First, `cd` into the `qwt` directory:

```bash
cd lumberjack/qwt
```

Next, follow the [Qwt build instructions](https://qwt.sourceforge.io/qwtinstall.html) to build the library.

Skip to the *Build and installation* section, and follow the instructions for your platform.

Note: You must ensure that you build the Qwt library with the same compiler you will be using to build Lumberjack! Otherwise, the library will not be compatible.

## Open Project

You should now be able to simply open the `lumberjack.pro` file in Qt Creator, configure a build kit, and build the project.

## Build Issues

Report any build issues to the [issue tracker](https://github.com/SchrodingersGat/lumberjack/issues).
