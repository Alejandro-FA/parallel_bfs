# Specify possible argument values
release="--release"
asan="--asan"
tsan="--tsan"
ubsan="--ubsan"

# Coloured output constants
GREEN='\033[1;32m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Abort script if something fails
set -e

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
    printf "%bWarning: using clang++-mp-17 as the compiler. Ensure that you have it installed with macports (https://ports.macports.org/port/clang-17/).%b\n" "$CYAN" "$NC"
    compiler="clang++-mp-17 -Wthread-safety"
  else
    compiler="clang++ -Wthread-safety"
  fi
fi

# Compile differently depending on the mode selected
san_options=""
if [ "$mode" == $release ]; then
  printf "%bCompiling test bench in release mode...%b\n" "$CYAN" "$NC"
  $compiler -Wall -O3 -std=c++2b  src/main.cpp -o main.out
elif [ "$mode" == $tsan ]; then
  printf "%bCompiling test bench with ThreadSanitizer...%b\n" "$CYAN" "$NC"
  $compiler -Wall -Wextra -fsanitize=thread -g -O2 -std=c++2b  src/main.cpp -o main.out
  san_options="TSAN_OPTIONS=halt_on_error=1"
elif [ "$mode" == $asan ]; then
  printf "%bCompiling test bench with AddressSanitizer (and LeakSanitizer)...%b\n" "$CYAN" "$NC"
  $compiler -Wall -Wextra -fsanitize=address -fno-omit-frame-pointer -g -Og -std=c++2b  src/main.cpp -o main.out
  san_options="ASAN_OPTIONS=detect_stack_use_after_return=false:detect_leaks=1"
elif [ "$mode" == $ubsan ]; then
  printf "%bCompiling test bench with UndefinedBehaviourSanitizer...%b\n" "$CYAN" "$NC"
  $compiler -Wall -Wextra -fsanitize=undefined -fno-omit-frame-pointer -g -Og -std=c++2b  src/main.cpp -o main.out
  san_options="UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
fi

# Run program
command="./main.out"
if [[ "$san_options" != "" ]]; then
  command="${san_options} ${command}"
fi
printf "\n%bExecuting '%b%s%b'...%b\n\n" "$CYAN" "$GREEN" "$command" "$CYAN" "$NC"
eval "$command"