# Specify possible argument values
release="--release"
asan="--asan"
tsan="--tsan"
ubsan="--ubsan"

# Set the default mode
mode=$release

# Check the number of command line arguments
if [ "$#" -gt 1 ]; then
  echo "Usage: $0 [$release|$tsan|$asan|$ubsan]"
  exit 1
fi

# Override the default mode if an argument is provided
if [ "$#" -eq 1 ]; then
  mode=$1

  # Check if the mode is either "debug" or "release"
  if [ "$mode" != $release ] && [ "$mode" != $tsan ] && [ "$mode" != $asan ] && [ "$mode" != $ubsan ]; then
    echo "Invalid mode: $mode. Use '$release', '$tsan', '$asan' or '$ubsan'."
    exit 1
  fi
fi

# Use different compiler depending on the OS
OS=$(uname)
compiler=g++
# Check if the OS is macOS
if [ "$OS" == "Darwin" ]; then
  if [ "$mode" != $release ]; then
    compiler="clang++-mp-17 -Wthread-safety"
  else
    compiler="clang++ -Wthread-safety"
  fi
fi

# Compile differently depending on the mode selected
if [ "$mode" == $release ]; then
  echo "Compiling test bench in release mode..."
  $compiler -Wall -O3 -std=c++2b  src/main.cpp -o main.out
elif [ "$mode" == $tsan ]; then
  echo "Compiling test bench in ThreadSanitizer..."
  $compiler -Wall -Wextra -fsanitize=thread -g -O2 -std=c++2b  src/main.cpp -o main.out
elif [ "$mode" == $asan ]; then
  echo "Compiling test bench with AddressSanitizer..."
  ASAN_OPTIONS=detect_stack_use_after_return=false,detect_leaks=1 $compiler -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer -g -Og -std=c++2b  src/main.cpp -o main.out
elif [ "$mode" == $ubsan ]; then
  echo "Compiling test bench with UndefinedBehaviourSanitizer..."
  UBSAN_OPTIONS=halt_on_error=1 $compiler -Wall -Wextra -fsanitize=undefined -g -Og -std=c++2b  src/main.cpp -o main.out
fi
