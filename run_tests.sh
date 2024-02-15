cd cmake-build-release || echo "'cmake-build-release' not found" && exit 1
for delay in 0 10 100; do
  ./main.out --solve ../problems/simple -d $delay;
  ./main.out --solve ../problems/medium -d $delay;
  ./main.out --solve ../problems/deep -d $delay;
  ./main.out --solve ../problems/unbalanced -d $delay;
  ./main.out --solve ../problems/complex -d $delay;
done
