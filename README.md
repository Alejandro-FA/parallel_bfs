# Parallel BFS

Simple framework to test and benchmark C++ parallelization techniques applied to Best First Search problems.
The end goal is to port the results to [BFGP++](https://github.com/jsego/bfgp-pp). 

## Usage

### Compilation

The easiest way to compile the project is using CMake. You can install `cmake` using a package manager (like `apt` in
Ubuntu and [`macports`](https://www.macports.org/) or [`homebrew`](https://brew.sh/) in macOS). You can also install it
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
2. Then use `cmake` to configure the project. At this point you can use certain flags to specify how do you want the
project to be built, like the *compiler* to use or the *build type*. To use the default values and compile in `Release`
mode, simply execute the following command (the two dots `..` are important):

    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

    > If you want to specify which compiler to use, you can use the `CMAKE_CXX_COMPILER` flag. For example, if you
    have installed `clang` with `macports`:
   > ```bash
   > cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/opt/local/bin/clang++-mp-17
   > ```
After this one-time step, everytime that you want to recompile the project you simply need to execute this:

```bash
cmake --build .
```

> If you are no longer inside the build folder, you can change the command to `cmake --build <path-to-build-folder>`

### Run the program

When compiling with `cmake`, the executable will be placed inside the build folder that you created in the first step.
You can simply execute it as follows:

```bash
./parallel_bfs
```

### Test for bugs using Google Sanitizers

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
> separately. For example, you can [install `clang` with `macports`](https://ports.macports.org/search/?q=clang&name=on).
> You can also disable leak detection by changing `ASAN_OPTIONS=detect_leaks` to `0`.
