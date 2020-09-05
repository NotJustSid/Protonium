#include "proto.hpp"

#include <filesystem>
#include <fstream>

#include "includes/Lexer.hpp"
#include "includes/Parser.hpp"

Proto& Proto::getInstance() {
    static Proto proto;
    return proto;
}

void Proto::run(std::string src) {
    auto lexer = Lexer(std::move(src));
    auto& tokens = lexer.scanTokens(*this);
    auto parser = Parser(tokens);
    auto parsedExpr = parser.parse();

    if (hadError()) return; //stop if there was an error
    
    //For now, print the AST
    /*TreePrinter printer;
    std::cout <<  printer.tree(*parsedExpr);*/

    Interpreter::getInstance().interpret(parsedExpr);

    ////For now, print the tokens
    //Those were the days when I just had a lexer
    /*for (auto token : tokens) {
        std::cout << token << " ";
    }*/
}

void Proto::runFile(std::string_view path) {
    namespace fs = std::filesystem;
    fs::path loc{ path.data() };

    if (!fs::is_regular_file(loc)) {
        std::cerr << "[ERR] Path not recognized as a file.";
        std::exit(64);
    }
    else {
        std::string src;
        std::ifstream file{ path.data() };
        
        //TODO look at this later
        /*src.reserve(fs::file_size(loc));*/ 
        //! Don't know about this; we lose information if the file size is
        //! larger than the sizeof(unsigned int).

        src.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        run(src);
        if (hadError()) std::exit(65);
    }
}

void Proto::setErr(bool val) {
    m_hitError = val;
}

void Proto::setRuntimeError(bool val) {
    m_hitRuntimeError = val;
}

bool Proto::hadError() const {
    return m_hitError;
}

bool Proto::hadRuntimeError() const {
    return m_hitRuntimeError;
}

void Proto::error(std::size_t line, std::string_view msg, std::string snippet) {
    std::cerr << "[ERR Line " << line << "]: " << msg << snippet << '\n';
    setErr(true);
}

void Proto::runtimeError(const RuntimeError& error) {
    std::cerr << "[Runtime ERR Line " << error.getToken().getLine() << "]: " << error.what() << '\n';
}
