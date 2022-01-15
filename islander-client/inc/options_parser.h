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
    [[nodiscard]] bool get_detach_flag() const { return detach_flag; };
    [[nodiscard]] bool get_ps_flag() const { return ps_flag; };
    [[nodiscard]] bool get_netns_flag() const { return netns_flag; };

    int get_port();

    std::string get_bin();
    std::string get_name();
    std::string get_delete();
    std::string get_attach();

    std::string get_address();

    void parse(int ac, char **av);

    std::string parse_number_with_unit(std::string& search_param, std::string default_val);
    std::string parse_number_param(std::string& search_param, std::string default_val);
    std::string parse_vector_param(std::string& search_param);
    std::string get_memory_in_bytes();
    std::string get_device_read_bps();
    std::string get_device_write_bps();
    std::string get_cpu_shares();
    std::string get_cpu_period();
    std::string get_cpu_quota();
    std::string get_mount();
    std::string get_volume();
    std::string parse_tmpfs();

private:
    bool help_flag = false;
    bool detach_flag = false;
    bool ps_flag = false;
    bool netns_flag = false;

    void init_opt_description();

    boost::program_options::variables_map var_map{};
    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tclient [--port]"};
};

#endif //MYCAT_CONFIG_FILE_H

