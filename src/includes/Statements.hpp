#pragma once
#include "Expressions.hpp"

class Expression;
class Print;

class StmtVisitor {
public:
	virtual void visit(const Expression&) = 0;
	virtual void visit(const Print&) = 0;
};

class Stmt {
public:
	virtual void accept(StmtVisitor* visitor) const = 0;
};

using Stmt_ptr = std::shared_ptr<Stmt>;

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