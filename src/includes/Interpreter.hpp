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
	void executeBlock(Stmts stmts, Env_ptr env);


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
	virtual void visit(const Logical& log) override {
		log.m_left->accept(this);

		if (log.m_op.getType() == TokenType::OR) {
			if (isTrue(m_val)) {
				//the left side is true
				m_val = true;
			}
			else {
				log.m_right->accept(this);
				if (isTrue(m_val)) {
					//the left is false and right side is true
					m_val = true;
				}
				else {
					//both the left and right side are false
					m_val = false;
				}
			}
		}
		else {
			//and
			if (!isTrue(m_val)) {
				//the left side is false
				m_val = false;
			}
			else {
				log.m_right->accept(this);
				if (isTrue(m_val)) {
					//both the left side and the right side are true
					m_val = true;
				}
				else {
					//the left side is true but the right side is false
					m_val = false;
				}
			}
		}
	}

	//Statements

	virtual void visit(const Print& print) override;
	virtual void visit(const Expression& expr) override;
	virtual void visit(const Var& stmt) override;
	virtual void visit(const Block& block) override;
	virtual void visit(const If& ifStmt) override;

	void interpret(const std::vector<Stmt_ptr>& stmts);
	std::string interpret(Expr_ptr expr);
};