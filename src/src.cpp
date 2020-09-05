#include <cstdlib>    //std::exit
#include <iostream>
#include <string>
#include <list>

#define EXIT_UNEXPECTED_ARGS 2

#include "proto.hpp"

void repl(Proto& proto) {
    std::string line;

    while (std::cout << "proto> ", std::getline(std::cin, line)) {
        proto.run(line);
        proto.setErr(false); //set error to false for each repl thingy
        std::cout << '\n'; //print a new line before repeating
    }
}

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "Usage: proto [source]";
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