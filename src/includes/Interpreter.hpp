#pragma once
#include "Expressions.hpp"
#include "Statements.hpp"
#include "Environment.hpp"

class RuntimeError : std::exception {
private:
	std::string m_error;
	Token m_tok;
public:
	RuntimeError() = delete;
	RuntimeError(Token t, std::string err);
	virtual const char* what() const noexcept override;
	Token getToken() const;
};

class Interpreter : public ExprVisitor, public StmtVisitor {
private:
	Value m_val;
	Env_ptr m_env;	//the current environment
private:
	bool isNum(const Value& val);
	bool isNix(const Value& val);
	bool isStr(const Value& val);
	bool isBool(const Value& val);
	bool isTrue(const Value& val);
	bool isEqual(const Value& left, const Value& right);
	bool isEqual(long double left, long double right);

	std::string stringify(const Value& value, const char* strContainer = "");
	void execute(Stmt_ptr stmt);
	void executeBlock(std::vector<Stmt_ptr> stmts, Env_ptr env);


	Interpreter();
public:
	static Interpreter& getInstance();
	Interpreter(const Interpreter&) = delete;
	void operator=(const Interpreter&) = delete;
	
	virtual void visit(const Binary& bin) override;
	virtual void visit(const Unary& un) override;
	virtual void visit(const ParenGroup& group) override;
	virtual void visit(const Literal& lit) override;
	virtual void visit(const Variable& var) override;

	virtual void visit(const Print& print) override;
	virtual void visit(const Expression& expr) override;
	virtual void visit(const Var& stmt) override;
	virtual void visit(const Block& block) override;

	void interpret(const std::vector<Stmt_ptr>& stmts);
};