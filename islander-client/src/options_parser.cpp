// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <filesystem>
#include "../inc/options_parser.h"
#include "../inc/exceptions/parser_exeption.h"
#include "../inc/defined_vars.h"
#include "../inc/utils.h"

namespace po = boost::program_options;

command_line_options::command_line_options() {
    init_opt_description();
}

void command_line_options::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av).options(opt_conf).run();
        store(parsed, var_map);
//        filenames = collect_unrecognized(parsed.options, po::include_positional);
        help_flag = var_map.count("help");
        notify(var_map);
    } catch (std::exception &E) {
        std::cerr << E.what() << std::endl;
        throw OptionsParseException();
    }
}

int command_line_options::get_port() {
    if (var_map.find("port") != var_map.end()) {
        std::string string_val = var_map["port"].as<std::string>();
        if (is_number(string_val)) {
            auto int_val = atoi(string_val.c_str());
            if (int_val < MIN_PORT || int_val > MAX_PORT) {
                std::cout << "Port not in range from " << MIN_PORT << " to " << MAX_PORT << std::endl;
                return NO_PORT;
            }
            return int_val;
        }
        std::cout << "Port is not a number" << std::endl;
        return NO_PORT;
    }
    return NO_PORT;
}

std::string command_line_options::get_bin() {
    if (var_map.find("bin") != var_map.end()) {
        std::string string_val = var_map["bin"].as<std::string>();
        // Need to check if exists
        return string_val;
    }
    return "";
}

std::string command_line_options::parse_number_with_unit(std::string& search_param, std::string default_val) {
    if (var_map.find(search_param) != var_map.end()) {
        std::string string_val = var_map[search_param].as<std::string>();
        std::string unit;
        unit += toupper(string_val.back());
        std::string number = string_val.substr(0, string_val.size()-1);

        // Check number
        if (!is_number(number)) {
            std::cout << "Provided " << search_param << " param is not a number!" << std::endl;
            return TYPE_ERROR;
        }

        // Check unit
        std::map<std::string, int> unit_map = {
                {"B", 1},
                {"K", 1'000},
                {"M", 1'000'000},
                {"G", 1'000'000'000},
        };

        if (unit_map.find(unit) == unit_map.end()) {
            std::cout << "Provided " << search_param << " unit is not any of b, k, m, or g!" << std::endl;
            return WRONG_UNIT;
        }

        return string_val;
    }
    return default_val;
}

std::string command_line_options::parse_number_param(std::string& search_param, std::string default_val) {
    if (var_map.find(search_param) != var_map.end()) {
        std::string string_val = var_map[search_param].as<std::string>();

        // Check number
        if (!is_number(string_val)) {
            std::cout << "Provided " << search_param << " param is not a number!" << std::endl;
            return TYPE_ERROR;
        }

        return string_val;
    }
    return default_val;
}

std::string command_line_options::get_memory_in_bytes() {
    std::string param = "memory-in-bytes";
    return parse_number_with_unit(param, DEFAULT_MEMORY_IN_BYTES);
}

std::string command_line_options::get_device_read_bps() {
    std::string param = "device-read-bps";
    return parse_number_with_unit(param, DEFAULT_DEVICE_READ_BPS);
}

std::string command_line_options::get_device_write_bps() {
    std::string param = "device-write-bps";
    return parse_number_with_unit(param, DEFAULT_DEVICE_WRITE_BPS);
}

std::string command_line_options::get_cpu_shares() {
    std::string param = "cpu-shares";
    return parse_number_param(param, DEFAULT_CPU_SHARES);
}

std::string command_line_options::get_cpu_period() {
    std::string param = "cpu-period";
    return parse_number_param(param, DEFAULT_CPU_PERIOD);
}

std::string command_line_options::get_cpu_quota() {
    std::string param = "cpu-quota";
    return parse_number_param(param, DEFAULT_CPU_QUOTA);
}

std::string command_line_options::parse_vector_param(std::string& search_param) {
    std::string validated_mount = " ";
    if (var_map.find(search_param) != var_map.end()) {
        std::vector<std::string> values = var_map[search_param].as<std::vector<std::string>>();

        if (values.size() < 2 || values.size() % 2 != 0) {
            std::cout << "Wrong number of parameters for mount option. It should be even. "
                         "Please specify src dst for each pair of mounts" << std::endl;
            return WRONG_ARGUMENTS;
        }

        for (int i = 0; i < values.size(); i++) {
            if (i % 2 == 0) {
                validated_mount += " --" + search_param + " src ";
            } else {
                validated_mount += " dst ";
            }
            validated_mount += values[i];
        }

        return validated_mount;
    }
    return "";
}

std::string command_line_options::get_mount() {
    std::string param = "mount";
    return parse_vector_param(param);
}

std::string command_line_options::get_volume() {
    std::string param = "volume";
    return parse_vector_param(param);
}

void command_line_options::init_opt_description() {
    opt_conf.add_options()
        ("help,h", "Show help message");
    opt_conf.add_options()
            ("port", po::value<std::string>(), "Set server port");
    opt_conf.add_options()
            ("bin", po::value<std::string>(), "Set binary path");

    // Limit configs
    opt_conf.add_options()
            ("memory-in-bytes", po::value<std::string>(), "Set memory-in-bytes");
    opt_conf.add_options()
            ("cpu-shares", po::value<int>(), "Set cpu-shares");
    opt_conf.add_options()
            ("cpu-period", po::value<int>(), "Set cpu-period");
    opt_conf.add_options()
            ("cpu-quota", po::value<int>(), "Set cpu-quota");

    opt_conf.add_options()
            ("device-read-bps", po::value<std::string>(), "Set device-read-bps");
    opt_conf.add_options()
            ("device-write-bps", po::value<std::string>(), "Set device-write-bps");

    // Other configs
    opt_conf.add_options()
            ("mount", po::value<std::vector<std::string>>()->multitoken(), "Set mount");
    opt_conf.add_options()
            ("volume", po::value<std::vector<std::string>>()->multitoken(), "Set volume");
}