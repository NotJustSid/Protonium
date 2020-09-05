#pragma once
#include "Token.hpp"
#include <variant>
#include <vector>
#include <sstream>
#include <string>
#include <memory>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

////TODO look into the constructor reference thing

class Binary;
class Unary;
class ParenGroup;
class Literal;

//! Abstract
class Visitor {
//! Note that this is just using overloading and no dynamic binding stuff
public:
	virtual void visit(const Binary&) = 0;
	virtual void visit(const Unary&) = 0;
	virtual void visit(const ParenGroup&) = 0;
	virtual void visit(const Literal&) = 0;
};

class Expr {
public:
	virtual void accept(Visitor* visitor) const = 0;
};

using Expr_ptr = std::shared_ptr<Expr>;

class Binary : public Expr {
public:
	Expr_ptr m_left;
	Token m_op;
	Expr_ptr m_right;
public:
	Binary() = delete;
	Binary(Expr_ptr l, Token op, Expr_ptr r) : m_left(l), m_op(op), m_right(r) {

	}
	virtual void accept(Visitor* vistor) const override {
		vistor->visit(*this);
	}
};

class Unary : public Expr {
public:
	Token m_op;
	Expr_ptr m_right;
public:
	Unary(Token op, Expr_ptr r) : m_op(op), m_right(r) {

	}
	virtual void accept(Visitor* vistor) const override {
		vistor->visit(*this);
	}
};

class ParenGroup : public Expr {
public:
	Expr_ptr m_enclosedExpr;
public:
	ParenGroup(Expr_ptr enclosed) : m_enclosedExpr(enclosed) {

	}
	virtual void accept(Visitor* vistor) const override {
		vistor->visit(*this);
	}
};

class Literal : public Expr {
public:
	std::variant<std::string, long double, void*, bool> m_val;
	LiteralType m_literalType;
public:
	Literal(Token literal) : m_literalType(literal.getlType()){
		switch (literal.getlType()) {
		case LiteralType::NUM: 
			m_val = std::stold(literal.str());
			break;
		case LiteralType::STR: 
			m_val = literal.str();
			break;
		case LiteralType::NIX:
			m_val = nullptr;
			break;
		case LiteralType::TRUE:
			m_val = true;
			break;
		case LiteralType::FALSE:
			m_val = false;
			break;
		case LiteralType::NONE: throw "ERR: Trying to instantiate a literal with no possible literal value";
		}
	}
	virtual void accept(Visitor* vistor) const override {
		vistor->visit(*this);
	}
};

//! Visitors

class TreePrinter : public Visitor {
private:
	std::string m_repr;
public:
	const std::string& tree(const Expr& expr) {
		expr.accept(this);
		return m_repr;
	}
	virtual void visit(const Binary& bin) override {
		parenthesize(bin.m_op.str(), {bin.m_left.get(), bin.m_right.get()});
	}
	virtual void visit(const Unary& un) override {
		parenthesize(un.m_op.str(), { un.m_right.get() });
	}
	virtual void visit(const ParenGroup& group) override {
		parenthesize("group", { group.m_enclosedExpr.get() });
	}
	virtual void visit(const Literal& lit) override {
		std::stringstream ss;
		if (lit.m_literalType == LiteralType::NUM) {
			ss << std::get<long double>(lit.m_val);
		}
		if (lit.m_literalType == LiteralType::STR) {
			ss << "\"" << std::get<std::string>(lit.m_val) << "\"";
		}
		if (lit.m_literalType == LiteralType::NIX) {
			ss << "nix";
		}
		if (lit.m_literalType == LiteralType::TRUE) {
			ss << "true";
		}
		if (lit.m_literalType == LiteralType::FALSE) {
			ss << "false";
		}
		m_repr += ss.str();
	}

private:
	void parenthesize(std::string name, std::vector<Expr*>&& exprs) {
		m_repr += "(" + name;
		for (auto& expr : exprs) {
			m_repr += " ";
			expr->accept(this);
		}
		m_repr += ")";
	}
};

using Value = std::variant<std::string, long double, void*, bool>;
const auto epsilon = std::numeric_limits<long double>::epsilon();
const auto maxPrecision = std::numeric_limits<long double>::digits10 + 1;

class RuntimeError : std::exception {
private:
	std::string_view m_error;
	Token m_tok;
public:
	RuntimeError() = delete;
	RuntimeError(Token t, std::string_view err) : m_error(err), m_tok(t)
	{

	}
	virtual const char* what() const noexcept override {
		return m_error.data();
	}
	Token getToken() const {
		return m_tok;
	}
};

class Interpreter : public Visitor {
private:
	Value m_val;
private:
	bool isNum(const Value& val) {
		return std::holds_alternative<long double>(val);
	}
	bool isNix(const Value& val) {
		return std::holds_alternative<void*>(val);
	}
	bool isStr(const Value& val) {
		return std::holds_alternative<std::string>(val);
	}
	bool isBool(const Value& val) {
		return std::holds_alternative<bool>(val);
	}
	bool isTrue(const Value& val) {
		//? return an error if the thingy is not a bool?
		if (isNix(val)) {
			return false;
		}
		if (isBool(val)) {
			return std::get<bool>(val);
		}
		return true;
	}
	bool isEqual(const Value& left, const Value& right) {
		if (isNum(left) && isNum(right)) {
			return isEqual(std::get<long double>(left), std::get<long double>(right));
		}
		return left == right;
	}
	bool isEqual(long double left, long double right) {
		return std::fabsl(left - right) < epsilon;
	}

	std::string stringify(const Value& value) {
		if (isNix(value)) return "nix";
		if (isNum(value)) {
			std::stringstream ss;
			ss << std::setprecision(maxPrecision) << std::get<long double>(value);
			return ss.str();
		}
		if (isStr(value)) {
			return "\"" + std::get<std::string>(value) + "\"";
		}
		if (isBool(value)) {
			return std::get<bool>(value) ? "true" : "false";
		}
	}

	Interpreter() = default;
public:
	static Interpreter& getInstance();
	Interpreter(const Interpreter&) = delete;
	void operator=(const Interpreter&) = delete;
	virtual void visit(const Binary& bin) override {

		bin.m_left->accept(this);
		auto left = m_val;
		bin.m_right->accept(this); //this changes m_val
		auto right = m_val;

		bool numOperands = isNum(left) && isNum(right);
		bool strOperands = isStr(left) && isStr(right);

		switch (bin.m_op.getType()) {
			case TokenType::PLUS:
				if (numOperands) {
					m_val = std::get<long double>(left) + std::get<long double>(right);
					return;
				}
				if (strOperands) {
					m_val = std::get<std::string>(left) + std::get<std::string>(right);
					return;
				}
				throw RuntimeError(bin.m_op, "Both of the operands must be numbers or strings.");
			case TokenType::MINUS:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = std::get<long double>(left) - std::get<long double>(right);
				return;
			case TokenType::PRODUCT:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = std::get<long double>(left) * std::get<long double>(right);
				return;
			case TokenType::DIVISON:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				if (isEqual(std::get<long double>(right), 0)) {
					throw RuntimeError(bin.m_op, "Cannot divide by 0!");
				}
				m_val = std::get<long double>(left) / std::get<long double>(right);
				return;

			//comparisions
			case TokenType::GT_EQUAL:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = isEqual(std::get<long double>(left), std::get<long double>(right)) ? true : std::get<long double>(left) > std::get<long double>(right) ? true : false;
				return;
			case TokenType::LT_EQUAL:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = isEqual(std::get<long double>(left), std::get<long double>(right)) ? true : std::get<long double>(left) < std::get<long double>(right) ? true : false;
				return;
			case TokenType::LESS:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = isEqual(std::get<long double>(left), std::get<long double>(right)) ? false : std::get<long double>(left) < std::get<long double>(right) ? true : false;
				return;
			case TokenType::GREATER:
				if (!numOperands) {
					throw RuntimeError(bin.m_op, "Operands must be numbers.");
				}
				m_val = isEqual(std::get<long double>(left), std::get<long double>(right)) ? false : std::get<long double>(left) > std::get<long double>(right) ? true : false;
				return;
			case TokenType::NOT_EQUAL:
				m_val = !isEqual(left, right);
				return;
			case TokenType::EQ_EQUAL:
				m_val = isEqual(left, right);
				return;

		}

		m_val = nullptr;
	}
	virtual void visit(const Unary& un) override {
		un.m_right->accept(this);

		switch (un.m_op.getType()) {
		case TokenType::MINUS:
			if (!isNum(m_val)) {
				throw RuntimeError(un.m_op, "Operand must be a number.");
			}
			m_val = -std::get<long double>(m_val);
			break;
		case TokenType::NOT:
			m_val = !isTrue(m_val);
		}

	}
	virtual void visit(const ParenGroup& group) override {
		group.m_enclosedExpr->accept(this);
	}
	virtual void visit(const Literal& lit) override {
		m_val = lit.m_val;
	}

	void interpret(Expr_ptr expr);
};