// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <dirent.h>
#include "../include/utils.hpp"
#include "../include/defined_vars.hpp"
#include "../include/server.h"

#define RELATIVE_BIN_FOLDER_PATH "../external_commands/binary"
#define BUFFER_SIZE 512
#define DETACHED_MODE_CHAR "&"


/** This global variable used to use dup2() for children to write their output into pipe from which parent can read it */
int get_write_pipe_fd(bool set_var, int new_write_pipe_fd) {
    static int write_pipe_fd = 4;
    if (set_var) {
        write_pipe_fd = new_write_pipe_fd;
    }
    std::cout << "write_pipe_fd " << write_pipe_fd << std::endl;
    return write_pipe_fd;
}

int get_read_pipe_fd(bool set_var, int new_read_pipe_fd) {
    static int read_pipe_fd = 4;
    if (set_var) {
        read_pipe_fd = new_read_pipe_fd;
    }
    std::cout << "read_pipe_fd " << read_pipe_fd << std::endl;
    return read_pipe_fd;
}


/** This global variable used to say parent process about the piped kind of command.
 * This is needed for ex. for reading from pipe with outputs of children in parent process */
bool cmd_is_pipe(bool set_var, bool new_is_pipe) {
    static bool is_pipe;
    if (set_var) {
        is_pipe = new_is_pipe;
    }
    return is_pipe;
}


/** This global variable used to say parent process about the external kind of command.
 * This is needed for ex. for reading from pipe with outputs of children in parent process */
bool cmd_is_external(bool set_var, bool new_is_external) {
    static bool is_external;
    if (set_var) {
        is_external = new_is_external;
    }
    return is_external;
}


/** This global variable used to say parent process about the redirected kind of command.
 * This is needed for ex. for reading from pipe with outputs of children in parent process */
bool cmd_is_redirect(bool set_var, bool new_is_redirect) {
    static bool is_redirect;
    if (set_var) {
        is_redirect = new_is_redirect;
    }
    return is_redirect;
}


/** This global variable used to say parent process about the enclosed kind of command.
 * This is needed for ex. for reading from pipe with outputs of children in parent process */
bool cmd_is_enclosed(bool set_var, bool new_is_enclosed) {
    static bool is_enclosed;
    if (set_var) {
        is_enclosed = new_is_enclosed;
    }
    return is_enclosed;
}


/** Check if command is enclosed command */
bool check_if_enclosed(std::string cmd) { // copy cmd to protect changing of the command in a lot of util functions
    command_line_input cmd_input;
//    process_command_line(cmd, cmd_input);

    std::string cleaned_cmd_line;
    transform_and_check_detached_mode(cleaned_cmd_line, cmd, cmd_input); // also clean comments and strip

    // Check if a cmd input is an enclosed command
    std::string enclosed_cmd = get_enclosed_cmd(cleaned_cmd_line);
    if (!enclosed_cmd.empty()) {
        cmd_is_enclosed(true, true);
        return true;
    }
    return false;
}


/** Add the whole path to the binary folder (that contains external commands) to the $PATH var. */
void set_path_var(char *command_call) {
    // Get current
    const int BUF_SIZE = 512;
    char cwd[BUF_SIZE];
    std::string cwd_path = getcwd(cwd, BUF_SIZE);

    auto path_ptr = getenv("PATH");
    std::string path_var;
    if(path_ptr != nullptr)
        path_var = path_ptr;

    // Split the relative path to myshell by '/'.
    std::vector<std::string> dirs;
    boost::split(dirs, command_call, boost::is_any_of("\\/"));
    // Get rid of '.' at the front and 'myshell' at the back to get the exact relative path to myshell.
    dirs.pop_back();
    std::vector<std::string> new_dirs = std::vector<std::string>(dirs.begin() + 1, dirs.end());

    // Concatenate the whole path as cwd + call-dirs + relative-bin-folder-path.
    std::string path_to_binary = cwd_path + "/";
    for(auto &dir: new_dirs)
        path_to_binary += dir + "/";

    path_var += ":" + path_to_binary;
    setenv("PATH", path_var.c_str(), 1);
}


/* Add the whole path to the binary folder (that contains external commands) to the $PATH var.
 * command_call: relative path to 'myshell' from the folder where 'myshell' is called. */
void set_path_to_binary(char *command_call) {
    // Split the relative path to myshell by '/'.
    std::vector<std::string> dirs;
    boost::split(dirs, command_call, boost::is_any_of("\\/"));
    // Get rid of '.' at the front and 'myshell' at the back to get the exact relative path to myshell.
    dirs.pop_back();
    std::vector<std::string> new_dirs = std::vector<std::string>(dirs.begin() + 1, dirs.end());

    // Get cwd.
    std::vector<char> buffer(BUFFER_SIZE);
    std::string cwd = getcwd(buffer.data(), BUFFER_SIZE);

    // Concatenate the whole path as cwd + call-dirs + relative-bin-folder-path.
    std::string path_to_binary = cwd + "/";
    for(auto &dir: new_dirs)
        path_to_binary += dir + "/";
    path_to_binary += RELATIVE_BIN_FOLDER_PATH;

    // Update the $PATH variable by adding path to the binary folder.
    auto path_ptr = getenv("PATH");
    std::string path_var;
    if(path_ptr != nullptr)
        path_var = path_ptr;
    path_var += ":" + path_to_binary;
    setenv("PATH", path_var.c_str(), 1);
}


void strip_line(const std::string &line, std::string &new_line) {
    size_t start_idx = 0, end_idx = line.size() - 1;
    for (size_t i = 0; i < line.size(); i++) {
        if (!isspace(line[i])) {
            start_idx = i;
            break;
        }
    }

    for (size_t i = line.size() - 1; i > 0; i--) {
        if (!isspace(line[i])) {
            end_idx = i;
            break;
        }
    }

    if (start_idx == 0 && end_idx == line.size() - 1) {
        new_line = line;
    } else {
        new_line = line.substr(start_idx, end_idx - start_idx + 1);
    }
}


/** Save args from split command line in input_struct.args */
void get_args(const std::vector<std::string> &arr, size_t start_pos, command_line_input &input_struct) {
    for (size_t i = start_pos; i < arr.size(); i++) {
        if (arr[i][0] != '-') {
            input_struct.args.push_back(arr[i]);
        } else {
            input_struct.flags.push_back(arr[i]);
        }
    }
}


void get_command_options(command_line_input &input_struct, std::vector<std::string> &words, int argc) {
    // need to cast vector of strings to char**
    std::vector<char*> cstrings{};

    cstrings.reserve(words.size());
    for(auto& string : words)
        cstrings.push_back(&string.front());

    // parse arguments
    command_line_options config;
    config.parse(argc, cstrings.data());

    input_struct.help_flag = config.get_help_flag();
}


void process_command_line(const std::string &command_line, command_line_input &input_struct) {
    std::string striped_command_line, cleaned_command_line;
    cleaned_command_line = clean_from_comments(command_line);

    strip_line(cleaned_command_line, striped_command_line);

    std::vector<std::string> words;
    std::string delim = " ";
    boost::split(words, striped_command_line, boost::is_any_of("\t\n\v\r "), boost::algorithm::token_compress_on);
    input_struct.command = words[0];

    input_struct.args.reserve(words.size() - 1);
    get_args(words, 1, input_struct); // find args (not flags) among words from command line
    get_command_options(input_struct, words, words.size());

    // get filenames
    input_struct.args = get_matching_filenames(input_struct.args);
}


std::vector<command_line_input> process_pipe_command_line(const std::string &command_line, bool detached_mode) {
    std::vector<std::string> commands;
    boost::split(commands, command_line, boost::is_any_of("|"), boost::algorithm::token_compress_on);

    std::vector<command_line_input> pipe_commands{};
    for (auto &command: commands) {
        command_line_input input_struct{};
        process_command_line(command, input_struct);
        input_struct.detached_mode = detached_mode;
        pipe_commands.push_back(input_struct);
    }
    return pipe_commands;
}


void split_first(std::vector<std::string> &tokens, const std::string &line, const std::string &split_char) {
    const auto first_char_idx = line.find(split_char);
    if (first_char_idx != std::string::npos) {
        tokens.push_back(line.substr(0, first_char_idx));
        tokens.push_back(line.substr(first_char_idx + split_char.length()));
    }
    else {
        tokens.push_back(line);
    }
}


std::string clean_from_comments(const std::string &command_line) {
    size_t found_idx = command_line.find('#');
    if (found_idx == std::string::npos) {
        return command_line;
    }

    //! remove comment characters after #
    size_t end_line_idx = command_line.size();
    for (size_t i = found_idx; i < command_line.size(); i++) {
        if (command_line[i] == '#') {
            end_line_idx = i;
            break;
        }
    }

    return command_line.substr(0, end_line_idx);
}


bool transform_and_check_detached_mode(std::string &transformed_command_line, const std::string &command_line, command_line_input &input_struct) {
    std::string cleaned_command_line = clean_from_comments(command_line);
    strip_line(cleaned_command_line, transformed_command_line);

    std::vector<std::string> tokens;
    boost::split(tokens, transformed_command_line, boost::is_any_of("\t\n\v\r "), boost::algorithm::token_compress_on);

    bool detached_mode_error = false;
    for (int i = tokens.size() - 1; i >= 0; i--) {
        if (tokens[i] == DETACHED_MODE_CHAR) {
            if (i == tokens.size() - 1) {
                input_struct.detached_mode = true;
                break;
            } else {
                encrypted_cerr_if_needed("Detached mode char (&) should be the last character in the command,\n if after it there are other characters, this is syntax error. Errno ");
                detached_mode_error = true;
                return detached_mode_error;
            }
        }
    }

    if (input_struct.detached_mode) {
        std::string new_line;
        new_line.reserve(command_line.length());
        new_line = tokens[0];
        // avoid the last char DETACHED_MODE_CHAR
        for (int i = 1; i < tokens.size() - 1; i++) {
            new_line.append(" " + tokens[i]);
        }
        transformed_command_line = new_line;
    }
    return detached_mode_error;
}


void process_redirect_command_line(command_line_input &input_struct, std::string &target_file, const std::string &command_line,
                                   const std::string &redirect_type) {
    std::string striped_command_line;
    const std::string cleaned_command_line = clean_from_comments(command_line);
    strip_line(cleaned_command_line, striped_command_line);
#ifdef DEBUG_MODE
    cout << "process_redirect_command_line(): striped_command_line -- " << striped_command_line << endl;
#endif

    std::vector<std::string> commands;
    if (redirect_type == "2>&1") { // for 2>&1 we need to make split_first by >
        std::string split_char = ">";
        split_first(commands, striped_command_line, split_char);
    } else {
        split_first(commands, striped_command_line, redirect_type);
    }

    process_command_line(commands[0], input_struct);
#ifdef DEBUG_MODE
    cout << "commands[0] -- " << commands[0] << endl;
    cout << "commands[1] -- " << commands[1] << endl;
#endif

    // we are certain that redirect_type is in our command_line before calling this function
    std::vector<std::string> tokens;
    std::string part_line;
    strip_line(commands[1], part_line);
    boost::split(tokens, part_line, boost::is_any_of("\t\n\v\r "), boost::algorithm::token_compress_on);
    target_file = (tokens[0] != ">") ? tokens[0] : tokens[1];
#ifdef DEBUG_MODE
    cout << "target_file -- " << target_file << endl;
#endif
}


bool is_number(const std::string &str) {
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}


bool has_prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}


bool wild_match(const std::string &str, const std::string &pat) {
    std::string::const_iterator str_it = str.begin();
    for (std::string::const_iterator pat_it = pat.begin(); pat_it != pat.end();
         ++pat_it) {
        switch (*pat_it) {
            case '?':
                if (str_it == str.end()) {
                    return false;
                }

                ++str_it;
                break;
            case '*': {
                if (pat_it + 1 == pat.end()) {
                    return true;
                }

                const size_t max = strlen(&*str_it);
                for (size_t i = 0; i < max; ++i) {
                    if (wild_match(&*(str_it + i), &*(pat_it + 1))) {
                        return true;
                    }
                }

                return false;
            }
            case '[': {
                std::vector<char> possible_chars;

                // accumulate possible chars
                while (true) {
                    if (pat_it + 1 == pat.end()) return false;
                    ++pat_it;
                    if (*pat_it == ']') break;
                    possible_chars.push_back(*pat_it);
                }

                if (str_it == str.end()) {
                    return false;
                }

                // current character should match possible chars list
                if (std::find(possible_chars.begin(), possible_chars.end(), *str_it) == possible_chars.end()) {
                    return false;
                }
                ++str_it;
                break;
            }
            default:
                if (*str_it != *pat_it) {
                    return false;
                }

                ++str_it;
        }
    }

    return str_it == str.end();
}


std::vector<std::string> get_matching_filenames(std::vector<std::string> &args) {
    std::vector<std::string> files;
    bool matched;

    for (auto argument : args) {
        std::string path = "./";
        matched = false;
        std::string file_name = argument;

        // Check if wildcard detected:
        if (argument.find('?') == std::string::npos && argument.find('*') == std::string::npos && argument.find('[') == std::string::npos) {
            files.push_back(argument);
            continue;
        }

        // If passed argument contains path (contains a slash)
        if (argument.find('/') != std::string::npos) {
            std::vector<std::string> split_file;
            boost::split(split_file, argument, boost::is_any_of("/"));

            // The string before last slash is a filename or wildcard
            file_name = split_file[split_file.size()-1];
            // Everything else is a path
            path = "";
            for (size_t i = 0; i < split_file.size() - 1; i++) { path += split_file[i] + "/"; }
        } else {
            // Treat argument as filename with current dir path
            file_name = argument;
        }

        // Try to open the directory by using the provided path
        DIR *dir = opendir(path.c_str());

        if (dir == nullptr) {
            encrypted_cout_if_needed("Unable to open dir");
            files.push_back(argument);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // Allow only files
            if (entry->d_type != DT_REG) {
                continue;
            }
            // Iterate through files in directory and try to match one.
            bool match = wild_match(entry->d_name, file_name);
            if (match) {
                files.push_back(path + entry->d_name);
                matched = true;
            }
        }
        closedir(dir);

        // if nothing matched, return wild card as is
        if (!matched) {
            files.push_back(argument);
        }
    }

    return files;
}


void create_history_file(const std::string &history_filename) {
    // create a file to save history between sessions
    std::fstream history_file;

    history_file.open(history_filename, std::fstream::in | std::fstream::out | std::fstream::app);

    // If file does not exist, create new file
    if (!history_file ) {
        encrypted_cout_if_needed("Cannot open file, file does not exist. Creating new file..");

        history_file.open(history_filename,  std::fstream::in | std::fstream::out | std::fstream::trunc);
        history_file <<"\n";
        history_file.close();
    }
}


/* Return number of pipes if the input command is pipe. Otherwise, return 0. */
size_t is_pipe(const std::string &command_line) {
    std::vector<std::string> words;
    boost::split(words, command_line, boost::is_any_of("|"), boost::algorithm::token_compress_on);

    if (words.size() > 1) {
        cmd_is_pipe(true, true);
        return words.size() - 1;
    }
    else return 0;
}


// Retrieves the first command that is enclosed in $(...)
std::string get_enclosed_cmd(const std::string &command_line) {
    std::string target_cmd;

    std::vector<std::string> strings;
    iter_split(strings, command_line, boost::algorithm::first_finder("$("));

    if (strings.size() > 1) {
        // Erase first part
        strings.erase(strings.begin());

        for (const auto& split : strings) {
            // Get the finish idx
            auto split_idx = split.find_first_of(')');
            if (split_idx != std::string::npos) {
                target_cmd = split.substr(0, split_idx);
                break;  // Reads only first occurrence of $()
            } else {
                // No enclosing bracket, search continues
                target_cmd = "";
            }
        }
    }
    return target_cmd;
}


// Replaced the first command that is enclosed in $(...) with a value
std::string replace_input_with_cmd_res(const std::string &command_line, const std::string &replace_val) {
    std::string new_string;

    std::vector<std::string> strings;
    iter_split(strings, command_line, boost::algorithm::first_finder("$("));

    if (strings.size() > 1) {
        new_string = strings[0];
        // Erase first part
        strings.erase(strings.begin());

        bool append_split = false;
        for (const auto& split : strings) {
            if (append_split) {  // If the value has been inserted and we need to fill the rest of splits
                new_string += "$(" + split;
                continue;
            }
            // Get the finish idx
            auto split_idx = split.find_first_of(')');
            if (split_idx != std::string::npos) {
                new_string += replace_val + split.substr(split_idx + 1);
                append_split = true;
            } else {
                // No enclosing bracket, search continues
                new_string += "$(" + split;
            }
        }
    }

    // Replace newline characters with spaces
    std::replace(new_string.begin(), new_string.end(), '\n', ' ');

    return new_string;
}


std::string is_redirect(const std::string &command_line) {
    std::vector<std::string> words;
    std::size_t found;

    std::vector<std::string> redirect_types = {"2>&1", "&>", "2>", "<", ">"};
    for (auto &redirect: redirect_types) {
        found = command_line.find(redirect);
        if (found != std::string::npos) {
            if (redirect != "<") { // for in-redirect we need to redirect its output to client
                cmd_is_redirect(true, true);
            }
            return redirect;
        }
    }
    return NOT_REDIRECT;
}


std::string is_redirect(command_line_input &cmd) {
    std::vector<std::string> redirect_types = {"2>&1", "&>", "2>", "<", ">"};
    for (auto &redirect: redirect_types) {
        if (std::find(cmd.args.begin(), cmd.args.end(), redirect) != cmd.args.end()) {
            if (redirect != "<") { // for in-redirect we need to redirect its output to client
                cmd_is_redirect(true, true);
            }
            return redirect;
        }
    }
    return NOT_REDIRECT;
}


std::string concat_command(command_line_input &cmd) {
    std::string command{cmd.command};

    for (auto &flag: cmd.flags) {
        command += " " + flag;
    }

    for (auto &arg: cmd.args) {
        command += " " + arg;
    }

    return command;
}


/* Logs the provided message and prepends it with time and client information  */
void log_action(sockaddr_in* client_info, const std::string& message) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    // do not use encryption here as we use it just for debug
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "        " <<
              inet_ntoa(client_info->sin_addr) << ":" << (int) ntohs(client_info->sin_port)
              << " -- " << message << std::endl;
}


/* Function to exit the main server process.
 * It kills all children processes that were created by the server and it also closes the parent socket fd */
void exit_gracefully(const std::vector<pid_t>& children_pids, int socket_desc, int exit_code) {
    cout << "\nServer shutting down...\n" << endl;
    for (auto child_pid : children_pids) kill(child_pid, SIGTERM);  // Kill all children :=(
    close(socket_desc);  // Close parent socket fd
    exit(exit_code);  // Goodbye!
}
