# TODO: Use valgrind for Linux
./scripts/compile.sh
leaks -atExit -- ./main.bin
