#include <filesystem>
#include <optional>
#include <iostream>
#include <unordered_set>
#include "generator_config.h"
#include "generate.h"
#include "solve.h"


void show_help(const std::string& program_name){
    std::cout << "Usage: " << program_name << " [OPTION]... [DIRECTORY]...\n"
    "Create a search problem, write it in DIRECTORY and solve it.\n"
    "With more than one DIRECTORY, repeat this action for each of them.\n\n"

    "With no DIRECTORY, or when DIRECTORY is ., use the current directory to save the problem files.\n\n"

    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -c, --config=FILE         Use the specified configuration file to modify generation behaviour.\n"
    "  -g, --generate            Generate problems but do not solve them, unless --solve is also specified.\n"
    "  -s, --solve               Solve problems but do not generate them, unless --generate is also specified.\n"
    "  -n, --num-problems=NUM    Number of problems to generate/solve.\n"
    "  -h, --help                Display this help and exit.\n\n"

    "Examples:\n"
    "  " << program_name << " -c myconf.yaml data_dir   Use 'myconf.yaml' config file and perform --generate and --solve on 'data_dir'.\n"
    "  " << program_name << " --generate -n 10 .        Generate 10 problems in the current directory.\n"
    "  " << program_name << " --solve dir1 dir2         Solve problems in directories 'dir1' and 'dir2'.\n";
}


std::filesystem::path parse_directory(const std::string &arg) noexcept(false) {
    if (!std::filesystem::exists(arg)) {
        try {
            std::filesystem::create_directories(arg);
        }
        catch (std::filesystem::filesystem_error &e) {
            throw e;
        }
    } else if (!std::filesystem::is_directory(arg)) {
        throw std::filesystem::filesystem_error{"cannot parse directory", arg, std::make_error_code(std::errc::not_a_directory)};
    }
    return std::filesystem::path{arg};
}


std::pair<std::string, std::optional<std::string>> key_value_split(const char *raw_arg) {
    std::string arg{raw_arg};
    std::string::size_type equal_pos = arg.find('=');
    if (arg.starts_with("--") && equal_pos != std::string::npos)
        return {arg.substr(0, equal_pos), arg.substr(equal_pos + 1)};
    return {arg, std::nullopt};
}


int main(int argc, char** argv) {
    std::unordered_set<std::string> directories;
    std::optional<unsigned int> num_problems;
    bool call_generate = false, call_solve = false;
    std::optional<BasicTreeGeneratorConfig> config;


    // Parse command line arguments
    // NOTE: If the same option is specified multiple times, the last one will be used.
    for (int i = 1; i < argc; ++i) {
        auto [arg_name, arg_value] = key_value_split(argv[i]);

        if (arg_name == "--help" || arg_name == "-h") {
            show_help(argv[0]);
            return 0;
        }

        else if (arg_name.front() != '-') directories.insert(arg_name);

        else if (arg_name == "--config" || arg_name == "-c") {
            std::string config_file;
            if (arg_value.has_value()) config_file = arg_value.value();
            else if (i + 1 < argc) config_file = argv[++i];
            else { std::cerr << "No config file specified for " << arg_name << "\n"; return 1; }

            try { config = parse_config(config_file); }
            catch (const std::exception &e) { std::cerr << e.what() << "\n"; return 1; }
        }

        else if (arg_name == "--generate" || arg_name == "-g") call_generate = true;

        else if (arg_name == "--solve" || arg_name == "-s") call_solve = true;

        else if (arg_name == "--num-problems" || arg_name == "-n") {
            std::string n;
            if (arg_value.has_value()) n = arg_value.value();
            else if (i + 1 < argc) n = argv[++i];
            else { std::cerr << "No number specified for " << arg_name << "\n"; return 1; }

            try { num_problems = std::stoi(n); }
            catch (const std::exception &e) { std::cerr << e.what() << "\n"; return 1; }
        }

        else {
            std::cerr << "Unknown argument: " << arg_name << "\n";
            return 1;
        }
    }

    std::vector<std::filesystem::path> paths;
    std::ranges::transform(directories, std::back_inserter(paths), parse_directory);

    if (paths.empty()) paths.push_back(std::filesystem::current_path());

    if (!(call_generate || call_solve)) call_generate = call_solve = true;

    if (call_generate)
        std::ranges::for_each(paths, [num_problems, config](const auto &p) {generate(p, num_problems, config); });

    if (call_solve)
        std::ranges::for_each(paths, [num_problems](const auto &p) {solve(p, num_problems); });

    return 0;
}
