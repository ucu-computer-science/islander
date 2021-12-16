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

    [[nodiscard]] std::vector<std::string> get_filenames() const { return filenames; };

    void parse(int ac, char **av);

    boost::program_options::variables_map var_map{};

private:
    std::vector<std::string> filenames;

    void init_opt_description();


    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tislander [-h|--help] [--memory-in-bytes] [--cpu-shares] [--cpu-period] [--cpu-quota] [--device-read-bps] [--device-write-bps]"};
};

#endif //MYCAT_CONFIG_FILE_H

