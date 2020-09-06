#pragma once
#include <list>
#include <vector>
#include <memory>
#include <exception>
#include <string>
#include <string_view>

#include "includes/Expressions.hpp"
#include "includes/Token.hpp"
#include "Statements.hpp"

class ParseError : std::exception {
private:
	std::string_view m_error;
public:
	ParseError() = delete;
	ParseError(std::string_view err) : m_error(err) {

	}
	virtual const char* what() const noexcept override {
		return m_error.data();
	}
};

class Parser {
private:
	std::vector<Token> m_tokens;
	std::size_t m_current;
public:
	Parser() = delete;
	Parser(std::vector<Token>& tokens);
	std::vector<Stmt_ptr> parse();
private:
//! Helpers

	bool isAtEnd();
	Token peek();
	Token previous();
	bool isNextType(TokenType type);
	Token advance(); //! Consume the next token
	
	//! Consume the next token if it matches any type of types
	bool match(std::list<TokenType> types);
	
	ParseError error(Token t, std::string_view msg);

	//! The next token is expected to be type. If it is not, we have 
	//! an error.
	bool matchWithErr(TokenType type, std::string_view err);

	void sync();

//! Production rules
	Stmt_ptr statement();
	Stmt_ptr printstmt();
	Stmt_ptr exprstmt();
	Expr_ptr expression();
	Expr_ptr equality();
	Expr_ptr comparision();
	Expr_ptr addition();
	Expr_ptr product();
	Expr_ptr unary();
	Expr_ptr primary();
};

