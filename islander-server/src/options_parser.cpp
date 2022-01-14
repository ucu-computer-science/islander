// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <filesystem>
#include "options_parser.hpp"
#include "exceptions/parser_exeption.h"
#include "defined_vars.hpp"
#include "utils.hpp"

namespace po = boost::program_options;

command_line_options::command_line_options() {
    init_opt_description();
}

void command_line_options::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av).options(opt_conf).allow_unregistered().run();
        store(parsed, var_map);
        help_flag = var_map.count("help");
        notify(var_map);
    } catch (std::exception &E) {
        encrypted_cerr_if_needed(E.what());
        throw OptionsParseException();
    }
}

int command_line_options::get_port() {
    if (var_map.find("port") != var_map.end()) {
        std::string string_val = var_map["port"].as<std::string>();
        if (is_number(string_val)) {
            auto int_val = atoi(string_val.c_str());
            if (int_val < MIN_PORT || int_val > MAX_PORT) {
                std::string msg =  "Port not in range from " + std::to_string(MIN_PORT) + " to " + std::to_string(MAX_PORT);
                encrypted_cout_if_needed(msg);
                return NO_PORT;
            }
            return int_val;
        }
        encrypted_cout_if_needed("Port is not a number");
        return NO_PORT;
    }
    return NO_PORT;
}

void command_line_options::init_opt_description() {
    opt_conf.add_options()
            ("help,h", "Show help message");
    opt_conf.add_options()
            ("server", "Launch in server mode");
    opt_conf.add_options()
            ("port", po::value<std::string>(), "Set server port");
    opt_conf.add_options()
            ("encrypt,e", "Enable encrypted communication");
}
