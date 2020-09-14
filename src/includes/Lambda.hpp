#pragma once
#include "Statements.hpp"
#include "Expressions.hpp"

class Lambda : public Expr{
public:
	std::vector<Token> m_params;
	Stmts m_body;
public:
	Lambda(const std::vector<Token>& params, const Stmts& body) : m_params(params), m_body(body) {

	}
	virtual void accept(ExprVisitor* visitor) const override {
		visitor->visit(*this);
	}
};