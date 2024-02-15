#include <filesystem>
#include <optional>
#include <iostream>
#include <unordered_set>
#include <string>
#include "../include/generator_config.h"
#include "../include/generate.h"
#include "../include/solve.h"


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
    "  -d, --workload-delay=TIME Artificial delay (in microseconds) when checking goal to simulate workload.\n"
    "  -h, --help                Display this help and exit.\n\n"

    "Examples:\n"
    "  " << program_name << " -c myconf.yaml data_dir   Use 'myconf.yaml' config file and perform --generate and --solve on 'data_dir'.\n"
    "  " << program_name << " --generate -n 10 .        Generate 10 problems in the current directory.\n"
    "  " << program_name << " --solve dir1 dir2         Solve problems in directories 'dir1' and 'dir2'.\n";
}


struct Arguments {
    std::unordered_set<std::filesystem::path> directories;
    std::optional<unsigned int> num_problems;
    std::optional<std::chrono::microseconds> workload_delay;
    std::optional<BasicTreeGeneratorConfig> config;
    bool call_generate = false;
    bool call_solve = false;
    bool show_help = false;
};


std::pair<std::string, std::optional<std::string>> key_value_split(const std::string &arg) {
    std::string::size_type equal_pos = arg.find('=');
    if (arg.starts_with("--") && equal_pos != std::string::npos)
        return {arg.substr(0, equal_pos), arg.substr(equal_pos + 1)};
    return {arg, std::nullopt};
}


void check_directory(const std::filesystem::path &path) noexcept(false) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    } else if (!std::filesystem::is_directory(path)) {
        throw std::filesystem::filesystem_error{
            "cannot parse directory",
            path,
            std::make_error_code(std::errc::not_a_directory)
        };
    }
}


Arguments parse_arguments(int argc, char** argv) noexcept(false) {
    Arguments args;

    // NOTE: If the same option is specified multiple times, the last one will be used.
    for (int i = 1; i < argc; ++i) {
        const std::string full_arg{argv[i]};
        auto [arg_name, arg_value] = key_value_split(full_arg);

        if (full_arg == "--help" || full_arg == "-h") args.show_help = true;

        else if (full_arg.front() != '-') args.directories.insert(std::filesystem::weakly_canonical(arg_name));

        else if (arg_name == "--config" || arg_name == "-c") {
            std::string config_file;
            if (arg_value.has_value()) config_file = arg_value.value();
            else if (i + 1 < argc) config_file = argv[++i];
            else throw std::runtime_error{"No config file specified for " + arg_name};

            args.config = parse_config(config_file);
        }

        else if (full_arg == "--generate" || full_arg == "-g") args.call_generate = true;

        else if (full_arg == "--solve" || full_arg == "-s") args.call_solve = true;

        else if (arg_name == "--num-problems" || arg_name == "-n") {
            std::string n;
            if (arg_value.has_value()) n = arg_value.value();
            else if (i + 1 < argc) n = argv[++i];
            else throw std::runtime_error{"No number specified for " + arg_name};

            args.num_problems = std::stoi(n);
        }

        else if (arg_name == "--workload-delay" || arg_name == "-d") {
            std::string delay;
            if (arg_value.has_value()) delay = arg_value.value();
            else if (i + 1 < argc) delay = argv[++i];
            else throw std::runtime_error{"No delay specified for " + arg_name};

            args.workload_delay = std::chrono::microseconds{std::stoi(delay)};
        }

        else throw std::runtime_error{"Unknown argument: " + full_arg};
    }

    // Set default values
    if (args.directories.empty()) args.directories.insert(std::filesystem::current_path());
    if (!(args.call_generate || args.call_solve)) args.call_generate = args.call_solve = true;

    return args;
}


void validate_arguments(const Arguments &args) noexcept(false) {
    std::ranges::for_each(args.directories, check_directory);

    if (args.workload_delay.has_value() && !args.call_solve)
        throw std::runtime_error{"Workload delay specified but no solving requested"};

    if (args.config.has_value() && !args.call_generate)
        throw std::runtime_error{"Config file specified but no generation requested"};

    if (args.workload_delay.has_value() && args.workload_delay.value().count() > 100)
        throw std::runtime_error{"Workload delay too high (max 100 microseconds)"};
}


int main(int argc, char** argv) {
    Arguments args;

    try {
        args = parse_arguments(argc, argv);
        validate_arguments(args);
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    if (args.show_help) {
        show_help(argv[0]);
        return 0;
    }

    try {
        if (args.call_generate)
            std::ranges::for_each(args.directories, [args](const auto &p) {generate(p, args.num_problems, args.config); });

        if (args.call_solve)
            std::ranges::for_each(args.directories, [args](const auto &p) {solve(p, args.num_problems, args.workload_delay); });

    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
