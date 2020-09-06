#include <string_view>

#include "includes/Parser.hpp"
#include "proto.hpp"

Parser::Parser(std::vector<Token>& tokens) : m_tokens(tokens), m_current(0) {

}

std::vector<Stmt_ptr> Parser::parse() {
	std::vector<Stmt_ptr> statements;
	/*try {
		return expression();
	}
	catch (const ParseError& error) {
		return nullptr;
	}*/
	while (!isAtEnd()) {
		statements.push_back(statement());
	}
	return statements;
}

bool Parser::isAtEnd() {
	return peek().getType() == TokenType::EOF_;
}

Token Parser::peek() {
	return m_tokens.at(m_current);
}

Token Parser::previous() {
	return m_tokens.at(m_current - 1);
}

bool Parser::isNextType(TokenType type) {
	if (isAtEnd()) return false;
	return peek().getType() == type;
}

Token Parser::advance() { 
	if (!isAtEnd()) m_current++;
	return previous();
}

bool Parser::match(std::list<TokenType> types) {
	for (auto type : types) {
		if (isNextType(type)) {
			advance();
			return true;
		}
	}

	return false;
}

ParseError Parser::error(Token t, std::string_view msg){
	Proto::getInstance().error(t.getLine(), msg);
	return ParseError{ msg };
}

bool Parser::matchWithErr(TokenType type, std::string_view err) {
	if (isNextType(type)) {
		advance();
		return true;
	}

	throw error(peek(), err);
}

void Parser::sync() {
	advance(); //consume the erroneous token 

	while (!isAtEnd()) {
		//We consume all tokens when encountered, but we make sure 
		//that we didn't consume a semi-colon. If we did, that's it 
		//for the consumption marathon.

		if (previous().getType() == TokenType::SEMICOLON) break;

		switch (peek().getType()) {
		case TokenType::CLASS:
		case TokenType::IF:
		case TokenType::WHILE:
		case TokenType::FOR:
		case TokenType::FUNCTION:
		case TokenType::RETURN:
		case TokenType::PRINT:
		case TokenType::VAR:
			return;
		}
	}
}

//! Production rules

Stmt_ptr Parser::statement() {
	if (match({ TokenType::PRINT })) {
		return printstmt();
	}
	return exprstmt();
}

Stmt_ptr Parser::printstmt() {
	auto expr = expression();
	matchWithErr(TokenType::SEMICOLON, "Expected a ';' after value.");
	return std::make_shared<Print>(expr);
}

Stmt_ptr Parser::exprstmt() {
	auto expr = expression();
	matchWithErr(TokenType::SEMICOLON, "Expected a ';' after expression.");
	return std::make_shared<Expression>(expr);
}

Expr_ptr Parser::expression() {
	return equality();
}

Expr_ptr Parser::equality() {
	auto expr = comparision();

	while (match({ TokenType::NOT_EQUAL, TokenType::EQ_EQUAL })) {
		Token op = previous();
		auto right = comparision();
		/*auto copy = new Binary(*expr, op, *right);
		delete expr;
		expr = copy;*/
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

Expr_ptr Parser::comparision() {
	auto expr = addition();

	while (match({ TokenType::GREATER, TokenType::GT_EQUAL, TokenType::LESS, TokenType::LT_EQUAL })) {
		Token op = previous();
		auto right = addition();
		expr = std::make_shared<Binary>(expr, op, right);
	}

	return expr;
}

Expr_ptr Parser::addition() {
	auto expr = product();

	while (match({ TokenType::PLUS, TokenType::MINUS })) {
		Token op = previous();
		auto right = product();
		expr = std::make_shared<Binary>(expr, op, right);
	}

	return expr;
}

Expr_ptr Parser::product() {
	auto expr = unary();

	while (match({ TokenType::PRODUCT, TokenType::DIVISON })) {
		Token op = previous();
		auto right = unary();
		expr = std::make_shared<Binary>(expr, op, right);
	}

	return expr;
}

Expr_ptr Parser::unary() {
	if (match({ TokenType::NOT, TokenType::MINUS })) {
		Token op = previous();
		auto right = unary();
		return std::make_shared<Unary>(op, right);
	}

	return primary();
}

Expr_ptr Parser::primary() {
	if (match({ TokenType::TRUE, TokenType::FALSE, TokenType::NIX, TokenType::NUMBER, TokenType::STRING })) {
		return std::make_shared<Literal>(previous());
	}

	if (match({ TokenType::LPAREN })) {
		auto expr = expression();
		matchWithErr(TokenType::RPAREN, "Expected ')' after expression.");
		return expr;
	}

	throw error(peek(), "Expected an expression.");
}
