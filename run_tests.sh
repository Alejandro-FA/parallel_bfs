#!/bin/bash

# Base directory for the output problems
problems_base="../problems"

cd cmake-build-release || (echo "'cmake-build-release' not found" && exit 1)

# Find directories that contain problem (.yaml) files directly under them
mapfile -t yaml_dirs < <(find "$problems_base" -type f -name "*.yaml" -exec dirname {} \; | sort -u)

# Solve all problems with delay 0
for problem_dir in "${yaml_dirs[@]}"; do
  ./main.out --solve -d 0 "$problem_dir"
done

# Solve small problems with multiple delays
for delay in {1..20..1}; do
  ./main.out --solve -d "$delay" "$problems_base/small"
done
