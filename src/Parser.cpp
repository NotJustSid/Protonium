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
		if (match({ TokenType::PRINT })) {
			return printstmt();
		}
		if (match({ TokenType::LBRACE })) {
			return std::make_shared<Block>(block());
		}
		if (match({ TokenType::IF })) {
			return ifstmt();
		}
		if (match({ TokenType::WHILE })) {
			return whilestmt();
		}
		return exprstmt();
	}
	catch (const ParseError&) {
		sync();
		return nullptr;
	}
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

Stmt_ptr Parser::ifstmt() {
	matchWithErr(TokenType::LPAREN, "Expected a '(' after 'if'.");
	auto condition = expression();
	matchWithErr(TokenType::RPAREN, "Expected a ')' after if condition.");

	auto then = statement();
	Stmt_ptr elseBranch{ nullptr };
	if (match({ TokenType::ELSE })) {
		elseBranch = statement();
	}

	return std::make_shared<If>(condition, then, elseBranch);
}

Stmt_ptr Parser::whilestmt() {
	matchWithErr(TokenType::LPAREN, "Expected a '(' after 'while'.");
	auto condition = expression();
	matchWithErr(TokenType::RPAREN, "Expected a ')' after while condition.");

	auto body = statement();

	return std::make_shared<While>(condition, body);
}

Stmt_ptr Parser::exprstmt() {
	auto expr = expression();
	if (m_allowExpr && isAtEnd()) m_foundExpr = true;
	else matchWithErr(TokenType::SEMICOLON, "Expected a ';' after expression.");
	return std::make_shared<Expression>(expr);
}

Expr_ptr Parser::expression() {
	return assignment();
}

Expr_ptr Parser::assignment() {
	auto expr = lor();

	if (match({ TokenType::EQUAL, TokenType::BT_EQUAL })) {
		Token op = previous();
		Expr_ptr val = assignment();

		if (std::static_pointer_cast<Variable>(expr)) {
			auto name = std::static_pointer_cast<Variable>(expr)->m_name;
			return std::make_shared<Assign>(name, op, val);
		}

		error(op, "Invalid assignment location.");
	}

	return expr;
}

Expr_ptr Parser::lor() {
	auto expr = land();

	while (match({ TokenType::OR })) {
		Token op = previous();
		auto right = land();
		expr = std::make_shared<Logical>(expr, op, right);
	}

	return expr;
}

Expr_ptr Parser::land() {
	auto expr = equality();

	while (match({ TokenType::AND })) {
		Token op = previous();
		auto right = equality();
		expr = std::make_shared<Logical>(expr, op, right);
	}

	return expr;
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
