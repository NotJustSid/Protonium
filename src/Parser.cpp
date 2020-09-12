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

bool Parser::match(const std::list<TokenType>& types) {
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
			return;
		}

		advance();
	}
}

//! Production rules

Stmt_ptr Parser::statement() {
	try {
		if (match({ TokenType::LBRACE })) {
			return std::make_shared<Block>(block());
		}
		if (match({ TokenType::IF })) {
			return ifstmt();
		}
		if (match({ TokenType::WHILE })) {
			return whilestmt();
		}
		if (match({ TokenType::FOR })) {
			return forstmt();
		}
		return exprstmt();
	}
	catch (const ParseError&) {
		sync();
		return nullptr;
	}
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

Stmt_ptr Parser::forstmt() {
	matchWithErr(TokenType::LPAREN, "Expected a '(' after 'for'.");
	
	Stmt_ptr init;
	if (match({ TokenType::SEMICOLON })) {
		init = nullptr;
	}
	else {
		init = exprstmt();
	}

	Expr_ptr condition{ nullptr };
	if (!isNextType(TokenType::SEMICOLON)) {
		//if the condition clause hasn't been omitted.
		condition = expression();
	}
	matchWithErr(TokenType::SEMICOLON, "Expected a ';' after for-loop condition.");

	Expr_ptr increment{ nullptr };
	if (!isNextType(TokenType::RPAREN)) {
		//if the increment clause hasn't been omitted.
		increment = expression();
	}
	matchWithErr(TokenType::RPAREN, "Expected a ')' after for-loop clauses.");

	Stmt_ptr body = statement();
	
	//desugarize and make this a while loop

	if (increment) {
		Stmts stmts;
		stmts.push_back(body);
		stmts.push_back(std::make_shared<Expression>(increment));
		body = std::make_shared<Block>(stmts);
	}

	if (condition == nullptr) condition = std::make_shared<Literal>(Token(TokenType::TRUE, "true", 0, LiteralType::TRUE));
	body = std::make_shared<While>(condition, body);

	if (init) {
		Stmts stmts;
		stmts.push_back(init);
		stmts.push_back(body);
		body = std::make_shared<Block>(stmts);
	}

	return body;
}

Stmt_ptr Parser::exprstmt() {
	auto expr = expression();
	if (m_allowExpr && isAtEnd() && !std::dynamic_pointer_cast<Call>(expr)) m_foundExpr = true;
	else matchWithErr(TokenType::SEMICOLON, "Invalid Syntax. Did you miss a ';' after the expression?");
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

		if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
			auto name = var->m_name;
			return std::make_shared<Assign>(name, op, val);
		}

		error(op, "Invalid assignment location.");
	}

	if (match({ TokenType::PROD_EQUAL, TokenType::DIV_EQUAL, TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL })) {
		Token op = previous();
		auto val = assignment();

		/*
		*	desugaring:
			a += 2 => a `= a + 2
			a -= 2 => a `= a - 2
			a *= 2 => a `= a * 2
			a /= 2 => a `= a / 2
		*/

		if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
			
			switch (op.getType()) {
			case TokenType::PLUS_EQUAL:
				op = Token(TokenType::PLUS, "+", op.getLine(), LiteralType::NONE);
				val = std::make_shared<Binary>(var, op, val);
				break;
			case TokenType::MINUS_EQUAL:
				op = Token(TokenType::MINUS, "-", op.getLine(), LiteralType::NONE);
				val = std::make_shared<Binary>(var, op, val);
				break;
			case TokenType::PROD_EQUAL:
				op = Token(TokenType::PRODUCT, "*", op.getLine(), LiteralType::NONE);
				val = std::make_shared<Binary>(var, op, val);
				break;
			case TokenType::DIV_EQUAL:
				op = Token(TokenType::DIVISON, "/", op.getLine(), LiteralType::NONE);
				val = std::make_shared<Binary>(var, op, val);
				break;
			}

			op  = Token(TokenType::BT_EQUAL, "`=", op.getLine(), LiteralType::NONE);
			return std::make_shared<Assign>(var->m_name, op, val);
		}
		else {
			error(op, "Invalid assignment location.");
		}
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

	return exponentiation();
}

Expr_ptr Parser::exponentiation() {
	auto base = call();
	if (match({ TokenType::EXPONENTATION })) {
		Token op = previous();
		auto power = exponentiation();
		base = std::make_shared<Binary>(base, op, power);
	}
	return base;
}

Expr_ptr Parser::call() {
	Expr_ptr expr = primary();

	while (true) {
		if (match({ TokenType::LPAREN })) {
			std::vector<Expr_ptr> args;
			if (!isNextType(TokenType::RPAREN)) {
				do {
					if (args.size() >= 127)
						Proto::getInstance().error(peek().getLine(), "Cannot have more than 127 arguments.");
					args.push_back(expression());
				} 
				while (match({TokenType::COMMA}));
			}

			matchWithErr(TokenType::RPAREN, "Expected a ')' after function arguments.");
			auto paren = previous();
			expr = std::make_shared<Call>(expr, paren, args);
		}
		else break;
	}

	return expr;
}

Expr_ptr Parser::primary() {
	if (match({ TokenType::TRUE, TokenType::FALSE, TokenType::NIX, TokenType::NUMBER, TokenType::STRING })) {
		return std::make_shared<Literal>(previous());
	}

	if (match({ TokenType::LPAREN })) {
		auto expr = expression();
		matchWithErr(TokenType::RPAREN, "Expected ')' after expression.");
		return std::make_shared<ParenGroup>(expr);
	}

	if (match({ TokenType::IDENTIFIER })) {
		return std::make_shared<Variable>(previous());
	}

	throw error(peek(), "Invalid Syntax.");
}
