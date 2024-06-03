#!/bin/bash

# Base directory for the configuration files
config_base="../config"

# Base directory for the output problems
problems_base="../problems"

# Loop through each .yaml file in the config directory structure
cd cmake-build-release || (echo "'cmake-build-release' not found" && exit 1)
find "$config_base" -name "*.yaml" | while read -r config_file; do
    # Extract the relative path of the config file
    relative_path="${config_file#"$config_base/"}"

    # Create the corresponding output directory path
    output_dir="$problems_base/${relative_path%.yaml}"

    # Execute the command
    ./main.out --generate -n 50 --config "$config_file" "$output_dir"
done
