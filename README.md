# Parallel BFS

Simple framework to test and benchmark C++ parallelization techniques applied to Best First Search problems.
The end goal is to port the results to [BFGP++](https://github.com/jsego/bfgp-pp). 

## Usage

To compile the program, simply execute the following command:

```bash
./scripts/compile.sh
```

Then you can run it as follows:

```bash
./main.out
```

The program itself will display benchmarks about the execution time. Alternatively, **you can also compile with CMake**
if you prefer. Several profiles are provided in `.idea/cmake.xml`.

### Use Google Sanitizers to check for multiple errors

You can compile the program with sanitizers to check for harder to detect bugs. For example, if you want to check for
memory errors ([AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)), compile and run the
program as follows:

```bash
./scripts/compile.sh --asan
./main.out
```

There are equivalent options for the [ThreadSanitizer](https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual)
(`--tsan`), which allows to check for race conditions, and the
[UndefinedBehaviourSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) (`--ubsan`), that looks for
code with undefined behaviour.

> **NOTE**: You may also need to install the `libasan`, `libasan` and `libubsan` libraries. In a RedHat-based
> distribution, you can do so with:
> ```bash
> sudo dnf install libtsan libasan libubsan
> ```
> I guess that in a Debian-based distribution you can use `apt` to install these libraries in a similar way (untested).


For more information you can check the [Google documentation](https://github.com/google/sanitizers/wiki),
the [`clang` documentation](https://clang.llvm.org/docs/UsersManual.html#controlling-code-generation) or the
[`gcc` documentation](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fsanitize_003daddress). If
you use CLion, it is also useful to check [their documentation](https://www.jetbrains.com/help/clion/google-sanitizers.html)
for more information about how to integrate this tools with the IDE.
