#include <cstdlib>    //std::exit
#include <iostream>
#include <string>
#include <algorithm>

#define EXIT_UNEXPECTED_ARGS 2

#include "proto.hpp"

#include "dep/rang.hpp"

using namespace rang;

std::size_t braceParenCount(std::string code) {
    std::size_t count{ 0 };
    auto braceParenCounter = [&count](char c) {
        if (c == '(' || c == '{') count++;
        if ((c == ')' || c == '}') && count != 0) count--;
        //if the count is 0, let the interpreter throw an error
    };
    std::for_each(code.begin(), code.end(), braceParenCounter);
    return count;
}

void repl(Proto& proto) {
    std::string line;
    std::string code;

    while (std::cout << fgB::green <<"proto> " << fg::reset, std::getline(std::cin, line)) {

        code += line;
        while (braceParenCount(code) != 0) {
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