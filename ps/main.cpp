#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <ftw.h>
#include <csignal>
#include <cstdio>
#include <boost/algorithm/string.hpp>


#define MAX_FD_NUM 20

/* Structure to contain all the necessary information about the isles (containers). */
struct islenode {
    std::string name;
    std::string pid;
    std::string time;
    std::string path;
};

std::vector<std::string> filenames;


/* Function called by nftw for each file it encounters.
 * fpath: The path name of the object.
 * sb: Pointer to a stat buffer containing information on the object.
 * tflag: An integer giving additional information about the files encountered.
 * ftwbuf: Pointer to FTW structure that contains base and level of the file. */
static int get_file_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    // File cannot be accessed due to the lack of permissions.
    if (tflag == FTW_NS || tflag == FTW_DNR) {
        std::string warning_msg = "Warning! Can't access file " + std::string(fpath) + " due to lack of permissions";
        perror(warning_msg.c_str());
        return 1;
    }
    // Add file information to the buffer.
    std::string file_name{fpath};
    filenames.push_back(file_name);
    return 0; // To tell nftw() to continue
}


/* Go through all the directories recursively, adding all the information about
 * directories to dirs buffer. Print all the information similarly to ls -l.
 * starting_directory: Directory from which we start our recursion.
 * flags: All the flags that nftw uses inside. */
void rls(std::string &starting_directory, int flags) {
    if (nftw(starting_directory.c_str(), get_file_info, MAX_FD_NUM, flags) == -1) {
        perror("Error while calling nftw");
        exit(0);
    }
}


/* Parse all the information regarding isles (containers), called islenodes,
 * such as PID, Name, Time created. */
std::vector<struct islenode> parse_files(std::vector<std::string> &files, std::string &path_to_files) {
    std::vector<struct islenode> islenodes{};
    for (auto &filepath: files) {
        // Read the whole file at once.
        std::ifstream t(filepath);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        // Split the file by \n
        std::vector<std::string> params;
        boost::split(params, str,boost::is_any_of("\n"));
        // Save the params.
        struct islenode isle_info{};
        isle_info.pid = params[0];
        isle_info.name = params[1];
        isle_info.time = params[2];
        isle_info.path = filepath;

        islenodes.push_back(isle_info);
    }
    return islenodes;
}


int main() {
    std::string path = "../isle/islenodes";
    rls(path, FTW_PHYS);

    // Skip the directory.
    std::vector<std::string> new_files = std::vector<std::string>(filenames.begin() + 1, filenames.end());
    // Get info about each isle (pid, name, time created).
    std::vector<struct islenode> islenodes = parse_files(new_files, path);

    // Split isles on active and dead. Islenodes of dead isles have to be deleted.
    std::vector<struct islenode> alive_islenodes{};
    for (auto &isle_node: islenodes) {
        int pid = stoi(isle_node.pid);
        // Send 0 signal to check if such PID exists.
        if (kill(pid, 0) == 0) {
            alive_islenodes.push_back(isle_node);
        } else {
            // All files associated with dead islenodes have to be deleted.
            if (remove(isle_node.path.c_str()) != 0)
                perror(("islander ps: Failed to remove islenode for <" + isle_node.name + "> isle").c_str());
        }
    }

    // Show all existing isles.
    printf("%-7s %-20s %-25s\n", "PID", "ISLE NAME", "TIME STARTED");
    for (auto &isle_node: alive_islenodes) {
        printf("%-7s %-20s %-25s\n", isle_node.pid.c_str(), isle_node.name.c_str(), isle_node.time.c_str());
    }
    return 0;
}
