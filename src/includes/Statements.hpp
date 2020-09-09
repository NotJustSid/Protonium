#pragma once
#include "Expressions.hpp"

class Expression;
class Print;
class Var;
class Block;
class If;
class While;

class StmtVisitor {
public:
	virtual void visit(const Expression&) = 0;
	virtual void visit(const Print&) = 0;
	virtual void visit(const Var&) = 0;
	virtual void visit(const Block&) = 0;
	virtual void visit(const If&) = 0;
	virtual void visit(const While&) = 0;
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

class Print : public Stmt {
public:
	Expr_ptr m_expr;
public:
	Print(Expr_ptr expr);
	virtual void accept(StmtVisitor* visitor) const override;
};

class Var : public Stmt {
public:
	Token m_name;
	Expr_ptr m_initializer;
public:
	Var(Token name, Expr_ptr init);
	virtual void accept(StmtVisitor* visitor) const override;
};

class Block : public Stmt {
public:
	std::vector<Stmt_ptr> m_stmts;
public:
	Block(std::vector<Stmt_ptr> stmts);
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