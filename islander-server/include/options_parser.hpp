#ifndef MYCAT_CONFIG_FILE_H
#define MYCAT_CONFIG_FILE_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <fstream>


class command_line_options {
public:
    command_line_options();

    ~command_line_options() = default;

    [[nodiscard]] bool get_help_flag() const { return help_flag; };

    int get_port();

    void parse(int ac, char **av);

private:
    bool help_flag = false;

    void init_opt_description();

    boost::program_options::variables_map var_map{};
    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tmyshell [-h|--help] [--server] [--port] [--encrypt] <file1> <file2> ... <fileN>\n"};
};

#endif //MYCAT_CONFIG_FILE_H

