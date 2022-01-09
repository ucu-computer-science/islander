//
// Created by myralllka on 9/12/20.
//

#ifndef MYCAT_PARSER_EXEPTION_H
#define MYCAT_PARSER_EXEPTION_H

#include <exception>

class OptionsParseException : public std::exception {
public:
    [[nodiscard]] const char *what() const noexcept override {
        return "Invalid configuration arguments!";
    }
};

#endif //MYCAT_PARSER_EXEPTION_H
