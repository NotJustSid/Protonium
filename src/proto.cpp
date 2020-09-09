#include "proto.hpp"

#include <filesystem>
#include <fstream>

#include "includes/Lexer.hpp"
#include "includes/Parser.hpp"
#include "includes/Interpreter.hpp"

#include "dep/rang.hpp"
using namespace rang;

Proto& Proto::getInstance() {
    static Proto proto;
    return proto;
}

void Proto::run(std::string src, bool allowExpr) {
    auto lexer = Lexer(std::move(src));
    auto& tokens = lexer.scanTokens(*this);
    auto parser = Parser(tokens, allowExpr);
    auto parsedOut = parser.parse();

    if (hadError()) return; //stop if there was an error
    if (std::holds_alternative<Stmts>(parsedOut)) {
        Interpreter::getInstance().interpret(std::get<Stmts>(parsedOut));
    }
    else {
        auto result = Interpreter::getInstance().interpret(std::get<Expr_ptr>(parsedOut));
        if (result != "") {
            std::cout << result << '\n';
        }
    }
}

void Proto::runFile(std::string_view path) {
    namespace fs = std::filesystem;
    fs::path loc{ path.data() };

    if (!fs::is_regular_file(loc)) {
        std::cerr << fgB::red << "[ERR] Path not recognized as a file." << fg::reset;
        std::exit(64);
    }
    else {
        std::string src;
        std::ifstream file{ path.data() };

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
    std::cerr << fgB::red  << "[ERROR | Line " << line << "]: " << fg::reset << style::dim << msg << snippet << style::reset << '\n';
    setErr(true);
}

void Proto::runtimeError(const RuntimeError& error) {
    std::cerr << fgB::red << "[RUNTIME ERROR | Line " << error.getToken().getLine() << "]: " << fg::reset << style::dim << error.what() << style::reset << '\n';
}
