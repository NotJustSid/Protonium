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
class Call;
class Lambda;
class ListExpr;
class Index;
class RangeExpr;
class IndexAssign;
class InExpr;

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
	virtual void visit(const Call&) = 0;
	virtual void visit(const Lambda&) = 0;
	virtual void visit(const ListExpr&) = 0;
	virtual void visit(const Index&) = 0;
	virtual void visit(const RangeExpr&) = 0;
	virtual void visit(const IndexAssign&) = 0;
	virtual void visit(const InExpr&) = 0;
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

class Callable;
using Callable_ptr = std::shared_ptr<Callable>;

class list_t;
using list_ptr = std::shared_ptr<list_t>;
using Value = std::variant<std::string, long double, std::nullptr_t, bool, Callable_ptr, list_ptr>;
using Values = std::vector<Value>;

class list_t {
public:
	Values m_list;
	//the types in here align with the order of the types in Value
	enum class Type : std::size_t {
		strList = 0,
		numList,
		nixList,
		boolList,
		fnList,
		multidimList,
		emptyList = 999
	};
	Type m_type;
public:
	list_t(const Values& list, Type type) : m_list(list), m_type(type) {

	}
};

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

class Call : public Expr {
public:
	Expr_ptr m_callee;
	Token m_paren;	//rparen to keep track of call line
	std::vector<Expr_ptr> m_args;
public:
	Call(Expr_ptr callee, Token rparen, const std::vector<Expr_ptr>& args);
	virtual void accept(ExprVisitor* visitor) const override;
};

class ListExpr : public Expr {
public:
	std::vector<Expr_ptr> m_exprs;
	Token m_brkt;	//for error reporting
public:
	ListExpr(const std::vector<Expr_ptr>& exprs, Token brkt);
	virtual void accept(ExprVisitor* visitor) const override;
};

class Index : public Expr {
public:
	Expr_ptr m_list;
	Expr_ptr m_index;
	Token m_indexOp;
public:
	Index(Token indexOp, Expr_ptr list, Expr_ptr index);
	virtual void accept(ExprVisitor* visitor) const override;
};

class RangeExpr : public Expr{
public:
	Expr_ptr m_first;
	Expr_ptr m_step;
	Expr_ptr m_end;
	Token m_op;
public:
	RangeExpr(Expr_ptr first, Expr_ptr end, Token op);
	RangeExpr(Expr_ptr first, Expr_ptr sep, Expr_ptr end, Token op);
	virtual void accept(ExprVisitor* visitor) const override;
};

class IndexAssign : public Expr {
public:
	Expr_ptr m_list;
	Expr_ptr m_index;
	Token m_indexOp;
	Token m_op;
	Expr_ptr m_val;
public:
	IndexAssign(Expr_ptr list, Expr_ptr index, Token indexOp, Token op, Expr_ptr val);
	virtual void accept(ExprVisitor* visitor) const override;
};

class InExpr : public Expr {
public:
	Token m_name;
	Token m_inKeyword;
	Expr_ptr m_iterable;
public:
	InExpr(Token name, Token in, Expr_ptr iterable);
	virtual void accept(ExprVisitor* visitor) const override;
};

const auto epsilon = std::numeric_limits<long double>::epsilon();
const auto maxPrecision = std::numeric_limits<long double>::digits10 + 1;