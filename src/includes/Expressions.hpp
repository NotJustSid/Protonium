#pragma once
#include <variant>
#include <vector>
#include <sstream>
#include <string>
#include <memory>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

#include "Token.hpp"

class Binary;
class Unary;
class ParenGroup;
class Literal;
class Variable;
class Logical;
class Assign;

class ExprVisitor {
//! Note that this is just using overloading and no dynamic binding stuff
public:
	virtual void visit(const Binary&) = 0;
	virtual void visit(const Unary&) = 0;
	virtual void visit(const ParenGroup&) = 0;
	virtual void visit(const Literal&) = 0;
	virtual void visit(const Variable&) = 0;
	virtual void visit(const Logical&) = 0;
	virtual void visit(const Assign&) = 0;
};

class Expr {
public:
	virtual void accept(ExprVisitor* visitor) const = 0;
};

using Expr_ptr = std::shared_ptr<Expr>;

class Binary : public Expr {
public:
	Expr_ptr m_left;
	Token m_op;
	Expr_ptr m_right;
public:
	Binary() = delete;
	Binary(Expr_ptr l, Token op, Expr_ptr r);
	virtual void accept(ExprVisitor* visitor) const override;
};

class Unary : public Expr {
public:
	Token m_op;
	Expr_ptr m_right;
public:
	Unary(Token op, Expr_ptr r);
	virtual void accept(ExprVisitor* visitor) const override;
};

class ParenGroup : public Expr {
public:
	Expr_ptr m_enclosedExpr;
public:
	ParenGroup(Expr_ptr enclosed);
	virtual void accept(ExprVisitor* visitor) const override;
};

using Value = std::variant<std::string, long double, void*, bool>;

class Literal : public Expr {
public:
	Value m_val;
	LiteralType m_literalType;
public:
	Literal(Token literal);
	virtual void accept(ExprVisitor* visitor) const override;
};

class Variable : public Expr {
public:
	Token m_name;
public:
	Variable(Token name);
	virtual void accept(ExprVisitor* visitor) const override;
};

class Logical : public Expr {
public:
	Expr_ptr m_left;
	Token m_op;
	Expr_ptr m_right;
public:
	Logical(Expr_ptr left, Token op, Expr_ptr right);
	virtual void accept(ExprVisitor* visitor)const override;
};

class Assign : public Expr {
public:
	Token m_name;
	Token m_op;
	Expr_ptr m_val;
public:
	Assign(Token name, Token op, Expr_ptr val);
	virtual void accept(ExprVisitor* visitor) const override;
};

//! Outdated Tree printer

//class TreePrinter : public ExprVisitor {
//private:
//	std::string m_repr;
//public:
//	const std::string& tree(const Expr& expr) {
//		expr.accept(this);
//		return m_repr;
//	}
//	virtual void visit(const Binary& bin) override {
//		parenthesize(bin.m_op.str(), {bin.m_left.get(), bin.m_right.get()});
//	}
//	virtual void visit(const Unary& un) override {
//		parenthesize(un.m_op.str(), { un.m_right.get() });
//	}
//	virtual void visit(const ParenGroup& group) override {
//		parenthesize("group", { group.m_enclosedExpr.get() });
//	}
//	virtual void visit(const Literal& lit) override {
//		std::stringstream ss;
//		if (lit.m_literalType == LiteralType::NUM) {
//			ss << std::get<long double>(lit.m_val);
//		}
//		if (lit.m_literalType == LiteralType::STR) {
//			ss << "\"" << std::get<std::string>(lit.m_val) << "\"";
//		}
//		if (lit.m_literalType == LiteralType::NIX) {
//			ss << "nix";
//		}
//		if (lit.m_literalType == LiteralType::TRUE) {
//			ss << "true";
//		}
//		if (lit.m_literalType == LiteralType::FALSE) {
//			ss << "false";
//		}
//		m_repr += ss.str();
//	}
//
//private:
//	void parenthesize(std::string name, std::vector<Expr*>&& exprs) {
//		m_repr += "(" + name;
//		for (auto& expr : exprs) {
//			m_repr += " ";
//			expr->accept(this);
//		}
//		m_repr += ")";
//	}
//};

const auto epsilon = std::numeric_limits<long double>::epsilon();
const auto maxPrecision = std::numeric_limits<long double>::digits10 + 1;