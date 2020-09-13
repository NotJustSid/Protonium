#pragma once
#include "Expressions.hpp"

class Expression;
class Block;
class If;
class While;
class Func;
class Return;

class StmtVisitor {
public:
	virtual void visit(const Expression&) = 0;
	virtual void visit(const Block&) = 0;
	virtual void visit(const If&) = 0;
	virtual void visit(const While&) = 0;
	virtual void visit(const Func&) = 0;
	virtual void visit(const Return&) = 0;
};

class Stmt {
public:
	virtual void accept(StmtVisitor* visitor) const = 0;
};

using Stmt_ptr = std::shared_ptr<Stmt>;
using Stmts = std::vector<Stmt_ptr>;

class Expression : public Stmt {
public:
	Expr_ptr m_expr;
public:
	Expression(Expr_ptr expr);
	virtual void accept(StmtVisitor* visitor) const override;
};

class Block : public Stmt {
public:
	Stmts m_stmts;
public:
	Block(const Stmts& stmts);
	virtual void accept(StmtVisitor* visitor) const override;
};

class If : public Stmt {
public:
	Expr_ptr m_condition;
	Stmt_ptr m_thenBranch;
	Stmt_ptr m_elseBranch;
public:
	If(Expr_ptr condition, Stmt_ptr thenBranch, Stmt_ptr elseBranch);
	virtual void accept(StmtVisitor* visitor) const override;
};

class While : public Stmt {
public:
	Expr_ptr m_condition;
	Stmt_ptr m_body;
public:
	While(Expr_ptr condition, Stmt_ptr body);
	virtual void accept(StmtVisitor* visitor) const override;
};

class Func : public Stmt {
public:
	Token m_name;
	std::vector<Token> m_params;
	Stmts m_body;
public:
	Func(Token name, const std::vector<Token>& params, const Stmts& body);
	virtual void accept(StmtVisitor* visitor) const override;
};

class Return : public Stmt {
public:
	Token m_keyword; //for error reporting
	Expr_ptr m_val;
public:
	Return(Token keyword, Expr_ptr val);
	virtual void accept(StmtVisitor* visitor) const override;
};