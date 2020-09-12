#include <cstdlib>    //std::exit
#include <iostream>
#include <string>

#define EXIT_UNEXPECTED_ARGS 2

#include "proto.hpp"

#include "dep/rang.hpp"

using namespace rang;

bool requireNewLine(const std::string& code) {
    std::size_t count{ 0 };
    bool withinQuotes{ false };

    for (auto it = code.begin(); it != code.end(); ++it) {
        if (*it == '"') {
            if (withinQuotes) {
                if (*(it - 1) != '\\') {
                    withinQuotes = !withinQuotes;
                }
            }
            else withinQuotes = !withinQuotes;
        }
        
        if (!withinQuotes) {
            if (*it == '(' || *it == '{') count++;
            if ((*it == ')' || *it == '}') && count != 0) count--;
        }
    }

    return count == 0 && withinQuotes;
}

void repl(Proto& proto) {
    std::string line;
    std::string code;

    while (std::cout << fgB::green <<"proto> " << fg::reset, std::getline(std::cin, line)) {

        code += line;
        while (requireNewLine(code)) {
            std::cout <<"       ";
            std::getline(std::cin, line);
            code += '\n';
            code += line;
        }

        proto.run(code, true);
        
        proto.setErr(false); //set error to false for each repl thingy
        std::cout << '\n'; //print a new line before repeating
        code = "";
    }
}

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << fgB::blue << "Usage:" << fgB::green << " proto " << fg::reset << style::dim << "[source]" << style::reset;
        std::exit(EXIT_UNEXPECTED_ARGS);
    }
    else if (argc == 2) {
        auto& proto = Proto::getInstance();
        proto.runFile(argv[1]);
    }
    else {
        auto& proto = Proto::getInstance();
        repl(proto);
    }
}