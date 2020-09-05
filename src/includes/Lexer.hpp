#pragma once
#include "Token.hpp"

#include <string>
#include <vector>

class Proto;

class Lexer {
private:
    std::string m_src;
    std::vector<Token> m_tokens;
    std::size_t m_start;    //position of the first character in the current lexeme
    std::size_t m_current;  //position of the character being scanned
    std::size_t m_line;     //line we're currently at
private:
    bool isAtEnd() const;
    void scanToken(Proto& p);
    char advance();
    void addToken(TokenType type, LiteralType ltype = LiteralType::NONE);
    // Is the next character equal to c? If yes, consume
    bool isNext(char c);
    bool isDigit(char c) const;
    bool isAlphaOrUnderscore(char c) const;
    bool isAlphaNumeric(char c) const;
    char peek() const;
    char peekby2() const;
    void string(Proto& p);
    void number(Proto& p);
    void identifierOrKeyword();
public:
    Lexer() = delete;
    Lexer(std::string&& src) : m_src(std::move(src)), m_start(0), m_current(0), m_line(1) {}
    std::vector<Token>& scanTokens(Proto& p);
};