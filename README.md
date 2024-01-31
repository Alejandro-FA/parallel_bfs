# Parallel BFS

Simple framework to test and benchmark C++ parallelization techniques applied to Best First Search problems.
The end goal is to port the results to [BFGP++](https://github.com/jsego/bfgp-pp). 

## Usage

### Compiling the project

The easiest way to compile the project is using CMake. You can install `cmake` using a package manager (like `apt` in
Ubuntu and [macports](https://www.macports.org/) or [homebrew](https://brew.sh/) in macOS). You can also install it
directly from the [official site](https://cmake.org/). If you have never used `cmake`, you can check the official
[getting started tutorial](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html).

The first time that you open the project you will need to **configure** it. To do so you only need to follow these steps
in a terminal:

1. In the root directory of the project, create a folder where all your binaries and build files will be placed.
Then change directory to this folder.

    ```bash
    mkdir cmake-build
    cd cmake-build
    ```
2. Then use `cmake` to configure the project. Most of the configuration is already specified in the `CmakeLists.txt` file
at the root of the project, but you can override some settings, like which *compiler* to use or the *build type*.
To use the default values and compile in `Release` mode, simply execute the following command:

    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

    >    If you want to specify which compiler to use, you can use the `CMAKE_CXX_COMPILER` flag. In particular, I
   > recommend to use `gcc` instead of `clang` because it has better support for C++20 and C++23 features. This is usually
   > the default for Linux systems, but in macOS you may need to install `gcc` with `macports` or `homebrew`. For example,
   > if you have installed `GCC 13` with `macports` you can use the following command to configure the project:
   >    ```bash
   >    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/opt/local/bin/g++-mp-13
   >    ```
   > For other compilers or platforms simply change the compiler path.

3. At this point, you only need to compile the project. To do so, simply execute the following command:

    ```bash
    cmake --build .
    ```

    If you have changed the configuration of the project, you will need to reconfigure it again (step 2).

### Running the program

When compiling with `cmake`, the executable will be placed inside the build folder that you created in the first step.
You can simply execute it as follows:

```bash
./main.out
```

### Testing for bugs using Google Sanitizers

This project comes with some built-in tests to check for memory errors, undefined behaviour errors and data races. To
check if the program has any bugs, you can simply run the following command:

```bash
ctest --output-on-failure
```

For more information on how sanitizers work, you can check the [Google documentation](https://github.com/google/sanitizers/wiki),
the [`clang` documentation](https://clang.llvm.org/docs/UsersManual.html#controlling-code-generation) or the
[`gcc` documentation](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fsanitize_003daddress).

> **NOTE**: In some platforms you may need to install the `libasan`, `libtsan` and `libubsan` libraries. In a RedHat-based
> distribution, you can do so with:
> ```bash
> sudo dnf install libasan libtsan libubsan
> ```
> I guess that in a Debian-based distribution you can use `apt` to install these libraries in a similar way (untested).

> **NOTE 2**: Apple Clang (the compiler that comes with XCode by default in macOS) does not currently support memory leak
> detection with the `AddressSanitizer`. If you want to use the first test in macOS, I recommend to install another compiler
> separately. Alternatively, you can also disable leak detection by changing `ASAN_OPTIONS=detect_leaks` to `0`.

> **NOTE 3**: `gcc` does not currently support Google Sanitizers for arm64 processors.