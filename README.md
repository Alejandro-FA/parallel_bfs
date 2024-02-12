# Parallel BFS

Simple framework to test and benchmark C++ parallelization techniques applied to Best First Search problems.
The end goal is to port the results to [BFGP++](https://github.com/jsego/bfgp-pp).

## Dependencies

This project tests different parallelization techniques available with the latest C++ standards. Unfortunately, the
level of support for these features is not the same in all compilers. Furthermore, some features are not yet
standardized and only available through external libraries. The following is a list of the main dependencies of
the project:

- **C++20**: The project takes advantage of new concurrency features available with C++20, so you
  will need a compiler that has good support for C++20. The project has been tested to work with `gcc 13.2`, but
  `clang 17.0.1` does not work because it does not support the `std::jthread` class. You can check support for other
  compilers in [cppreference.com](https://en.cppreference.com/w/cpp/compiler_support).


- **Intel OneTBB**: The parallel algorithms from C++17 are not yet widely supported by compilers. In particular, the
  `std::execution::par` policy is not yet implemented in `gcc` and `clang` (it defaults to a sequential policy).
  To use parallel algorithms with `gcc` and `clang`, you must use the `Intel OneTBB` library. For OSX you can install
  it using [macports](https://ports.macports.org/port/onetbb/). For other platforms check the
  [official documentation](https://www.intel.com/content/www/us/en/docs/onetbb/get-started-guide/2021-6/install-onetbb-on-linux-os.html)
  for detailed instructions. Ensure you install the `devel` version of the package, as it contains the headers and
  the static libraries needed to compile the project. Also, don't forget to [set the environment variables](https://www.intel.com/content/www/us/en/docs/onetbb/get-started-guide/2021-6/overview.html#BEFORE-YOU-BEGIN)
  after installing oneTBB (this last step is not needed if you have used macports).

  > If you want to always have the environment variables set, you can add the following lines to your `.bash_profile`,
  > `.zprofile`, or `.zshenv` file (assuming that you have installed `Intel OneTBB` in the default location in Linux):
  > ```bash
  > source /opt/intel/oneapi/tbb/latest/env/vars.sh
  > ```


- **_[Optional]_ Google Sanitizers**: The project comes with some built-in tests to check for memory errors, undefined
  behaviour errors and data races. Both `clang` and `gcc` support `x86_64` processors, but `gcc` does not currently
  support Google Sanitizers for `arm64-apple-darwin` processors (see
  [this discussion](https://github.com/orgs/Homebrew/discussions/3384) for more information).

  > **NOTE**: In some platforms you may need to install the `libasan`, `libtsan` and `libubsan` libraries separately.
  > In a RedHat-based distribution, you can do so with:
  > ```bash
  > sudo dnf install libasan libtsan libubsan
  > ```

  > **NOTE 2**: Apple Clang (the compiler that comes with XCode by default in macOS) does not support memory leak
  > detection with the `AddressSanitizer`, so I recommend installing another compiler separately. Alternatively, you can
  > also disable leak detection by changing `ASAN_OPTIONS=detect_leaks` to `0`.
  
  

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
./main.out "problems" --num-problems 1 --config "../config/simple_tree.yaml"
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
