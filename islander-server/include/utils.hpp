#ifndef MYSHELL_UTILS_HPP
#define MYSHELL_UTILS_HPP

#include <boost/algorithm/string.hpp>
#include <options_parser.hpp>
#include <cstring>
#include <vector>
#include <sstream>
#include <deque>

#include "base_header.hpp"
#include "server.h"

int get_write_pipe_fd(bool set_var, int new_write_pipe_fd);

int get_read_pipe_fd(bool set_var, int new_read_pipe_fd);

bool cmd_is_pipe(bool set_var, bool new_is_pipe);

bool cmd_is_external(bool set_var, bool new_is_external);

bool cmd_is_redirect(bool set_var, bool new_is_redirect);

bool cmd_is_enclosed(bool set_var, bool new_is_enclosed);

bool check_if_enclosed(std::string cmd);

void set_path_var(char *command_call);

void set_path_to_binary(char *command_call);

void strip_line(const std::string &line, std::string &new_line);

void get_args(const std::vector<std::string> &arr, size_t start_pos, command_line_input &input_struct);

void get_command_options(command_line_input &input_struct, std::vector<std::string> &words, int argc);

void process_command_line(const std::string &command_line, command_line_input &input_struct);

std::vector<command_line_input> process_pipe_command_line(const std::string &command_line, bool detached_mode);

void split_first(std::vector<std::string> &tokens, const std::string &line, const std::string &split_char);

std::string clean_from_comments(const std::string &command_line);

bool transform_and_check_detached_mode(std::string &striped_command_line, const std::string &command_line, command_line_input &input_struct);

void process_redirect_command_line(command_line_input &input_struct, std::string &target_file, const std::string &command_line,
                                   const std::string &redirect_type);

bool is_number(const std::string& str);

bool has_prefix(const char *pre, const char *str);

bool wild_match(const std::string &str, const std::string &pat);

std::vector<std::string> get_matching_filenames(std::vector<std::string> &args);

void create_history_file(const std::string &history_filename);

size_t is_pipe(const std::string &command_line);

std::string get_enclosed_cmd(const std::string &command_line);

std::string replace_input_with_cmd_res(const std::string &command_line, const std::string &replace_val);

std::string is_redirect(const std::string &command_line);

std::string is_redirect(command_line_input &cmd);

std::string concat_command(command_line_input &cmd);

void log_action(sockaddr_in* client_info, const std::string& message);

void exit_gracefully(const std::vector<pid_t>& children_pids, int socket_desc, int exit_code);

#endif //MYSHELL_UTILS_HPP
