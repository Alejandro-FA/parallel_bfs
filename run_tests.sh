#!/bin/bash

# Base directory for the output problems
problems_base="../problems"

cd cmake-build-release || (echo "'cmake-build-release' not found" && exit 1)

# Find directories that contain problem (.yaml) files directly under them
mapfile -t yaml_dirs < <(find "$problems_base" -type f -name "*.yaml" -exec dirname {} \; | sort -u)

for delay in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20; do
  for problem_dir in "${yaml_dirs[@]}"; do
      # Execute the command
      ./main.out --solve -d "$delay" "$problem_dir"
  done
done
