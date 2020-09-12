#pragma once
#include "Callable.hpp"
#include "Statements.hpp"
#include "Environment.hpp"


class ProtoFunction : public Callable {
private:
	Token m_name;
	std::vector<Token> m_params;
	Stmts m_body;
public:
	ProtoFunction(Token name, const std::vector<Token>& params, Stmts body);
	virtual int arity() override;
	virtual std::string info() override;
	virtual Value call(const Values& args) override;
};