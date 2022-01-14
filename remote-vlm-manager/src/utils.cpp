#include "../inc/utils.h"


std::string exec_bash_command(const std::string &cmd, int& out_exitStatus) {
    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    if (pPipe == nullptr) {
        throw std::runtime_error("Cannot open pipe");
    }

    std::array<char, 256> buffer{};
    std::string result;

    while (not std::feof(pPipe))
    {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if (WIFEXITED(rc)) {
        out_exitStatus = WEXITSTATUS(rc);
    }

    return result;
}

