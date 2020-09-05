#include "includes/Token.hpp"

std::string Token::str() const {
    return m_lexeme;
}

std::string_view Token::typeAsStr() const {
    return typeStr.at(m_type);
}

std::string_view Token::ltypeAsStr() const {
    return ltypeStr.at(m_ltype);
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "[LEXEME: '" <<t.str() << "', TYPE: " << t.typeAsStr() << ", LITERAL_TYPE: " << t.ltypeAsStr() << "]";
    return os;
}

TokenType Token::getType() const {
    return m_type;
}

LiteralType Token::getlType() const {
    return m_ltype;
}

std::size_t Token::getLine() const {
    return m_line;
}
