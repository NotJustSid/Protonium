#include <string_view>

#include "includes/Parser.hpp"
#include "proto.hpp"

Parser::Parser(std::vector<Token>& tokens, bool parseRepl) : m_tokens(tokens), m_current(0), m_allowExpr(parseRepl), m_foundExpr(false){

}

std::variant<Stmts, Expr_ptr> Parser::parse() {
	std::vector<Stmt_ptr> statements;
	while (!isAtEnd()) {
		statements.push_back(statement());

		if (m_foundExpr) {
			if (auto a = std::static_pointer_cast<Expression>(statements.back())) {
				return a->m_expr;
			}
		}
		m_allowExpr = false;
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
			return;
		}

		advance();
	}
}

//! Production rules

Stmt_ptr Parser::statement() {
	try {
		if (match({ TokenType::IDENTIFIER })) {
			if (match({ TokenType::EQUAL }))
				return vardefn();
			else m_current--;
		}
		if (match({ TokenType::PRINT })) {
			return printstmt();
		}
		if (match({ TokenType::LBRACE })) {
			return std::make_shared<Block>(block());
		}
		return exprstmt();
	}
	catch (const ParseError&) {
		sync();
		return nullptr;
	}
}

Stmt_ptr Parser::vardefn() {
	auto name = m_tokens.at(m_current - 2);
	Expr_ptr init;
		
	init = expression();

	matchWithErr(TokenType::SEMICOLON, "Expected a ';' after variable definition.");

	return std::make_shared<Var>(name, init);
}

Stmt_ptr Parser::printstmt() {
	auto expr = expression();
	matchWithErr(TokenType::SEMICOLON, "Expected a ';' after statement.");
	return std::make_shared<Print>(expr);
}

Stmts Parser::block() {
	std::vector<Stmt_ptr> statements;
	while (!isNextType(TokenType::RBRACE) && !isAtEnd()) {
		statements.push_back(statement());
	}

	matchWithErr(TokenType::RBRACE, "Expected a '}' at the end of the block.");
	return statements;
}

Stmt_ptr Parser::exprstmt() {
	auto expr = expression();
	if (m_allowExpr && isAtEnd()) m_foundExpr = true;
	else matchWithErr(TokenType::SEMICOLON, "Expected a ';' after expression.");
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

	if (match({ TokenType::IDENTIFIER })) {
		return std::make_shared<Variable>(previous());
	}

	throw error(peek(), "Expected an expression.");
}
