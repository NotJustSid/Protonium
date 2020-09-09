#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

enum class TokenType {

    LPAREN, RPAREN,
    LBRACE, RBRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    PRODUCT,
    DIVISON,
    SEMICOLON,

    NOT, NOT_EQUAL,
    EQUAL, EQ_EQUAL,
    GREATER, GT_EQUAL,
    LESS, LT_EQUAL,

    BT_EQUAL,
    PROD_EQUAL,
    DIV_EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,

    IDENTIFIER, STRING, NUMBER,

    AND,
    OR,
    PRINT,
    TRUE,
    FALSE,
    FUNCTION,
    THIS,
    RETURN,
    WHILE,
    FOR,
    CLASS,
    IF,
    ELSE,

    NIX,
    EOF_
};

const std::unordered_map<TokenType, std::string_view> typeStr = { 
    {TokenType::AND, "AND" },
    {TokenType::CLASS, "CLASS"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DIVISON, "DIVISON"},
    {TokenType::DOT, "DOT"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::EOF_, "EOF"}, 
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::EQ_EQUAL, "EQ_EQUAL"},
    {TokenType::FALSE, "FALSE"},
    {TokenType::FOR, "FOR"},
    {TokenType::FUNCTION, "FUNCTION"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::GT_EQUAL, "GT_EQUAL"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::IF, "IF"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::LESS, "LESS"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::LT_EQUAL, "LT_EQUAL"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::NIX, "NIX"},
    {TokenType::NOT, "NOT"},
    {TokenType::NOT_EQUAL, "NOT_EQUAL"},
    {TokenType::NUMBER, "NUMBER"},
    {TokenType::OR, "OR"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::PRINT, "PRINT"},
    {TokenType::PRODUCT, "PRODUCT"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::STRING, "STRING"},
    {TokenType::THIS, "THIS"},
    {TokenType::TRUE, "TRUE"},
    {TokenType::WHILE, "WHILE"}
};

const std::unordered_map<std::string, TokenType> keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"if", TokenType::IF},
    {"false", TokenType::FALSE},
    {"true", TokenType::TRUE},
    {"nix", TokenType::NIX},
    {"fn", TokenType::FUNCTION},
    {"for", TokenType::FOR},
    {"or", TokenType::OR},
    {"this", TokenType::THIS},
    {"while", TokenType::WHILE},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}
};

enum class LiteralType {
    NONE, //Not a literal
    STR,
    NUM,
    NIX,
    TRUE,
    FALSE,
};

const std::unordered_map<LiteralType, std::string_view> ltypeStr = {
    {LiteralType::NONE, "NONE"},    //Not a literal
    {LiteralType::NUM, "NUM"},
    {LiteralType::STR, "STR"},
    {LiteralType::NIX, "NIX"},
    {LiteralType::TRUE, "TRUE"},
    {LiteralType::FALSE, "FALSE"}
};

class Token {
private:
    TokenType m_type;
    std::string m_lexeme;
    std::size_t m_line;
    LiteralType m_ltype; //Literal Type
public:
    Token() = delete;
    Token(TokenType type, std::string lexeme, std::size_t line, LiteralType ltype) 
        : m_type(type), m_lexeme(lexeme), m_line(line), m_ltype(ltype) {}
    friend std::ostream& operator<<(std::ostream& os, const Token& t);
    std::string str() const;
    std::string_view typeAsStr() const;
    std::string_view ltypeAsStr() const;
    TokenType getType() const;
    LiteralType getlType() const;
    std::size_t getLine() const;
};