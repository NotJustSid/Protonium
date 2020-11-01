#include "includes/Expressions.hpp"
#include "includes/Callable.hpp"
#include "proto.hpp"

Binary::Binary(Expr_ptr l, Token op, Expr_ptr r) : m_left(l), m_op(op), m_right(r) {

}

void Binary::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Unary::Unary(Token op, Expr_ptr r) : m_op(op), m_right(r) {

}

void Unary::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

ParenGroup::ParenGroup(Expr_ptr enclosed) : m_enclosedExpr(enclosed) {

}

void ParenGroup::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Literal::Literal(Token literal) : m_literalType(literal.getlType()) {
	switch (literal.getlType()) {
	case LiteralType::NUM:
		try {
			m_val = std::stold(literal.str());
		}
		catch (const std::out_of_range&) {
			Proto::getInstance().error(literal.getLine(), "Number out of representation range: ", literal.str());
		}
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
	case LiteralType::NONE: Proto::getInstance().error(literal.getLine(), "Trying to instantiate a literal with no possible literal value: '", literal.str() + '\'');
	}
}

void Literal::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Variable::Variable(Token name) : m_name(name) {

}

void Variable::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Logical::Logical(Expr_ptr left, Token op, Expr_ptr right) : m_left(left), m_op(op), m_right(right) {

}

void Logical::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Assign::Assign(Token name, Token op, Expr_ptr val) : m_name(name), m_op(op), m_val(val) {

}

void Assign::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Call::Call(Expr_ptr callee, Token rparen, const std::vector<Expr_ptr>& args) : m_callee(callee), m_paren(rparen), m_args(args) {

}

void Call::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

ListExpr::ListExpr(const std::vector<Expr_ptr>& exprs, Token brkt) : m_exprs(exprs), m_brkt(brkt) {

}

void ListExpr::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Index::Index(Token indexOp, Expr_ptr list, Expr_ptr index) : m_indexOp(indexOp), m_list(list), m_index(index) {
}

void Index::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

RangeExpr::RangeExpr(Expr_ptr first, Expr_ptr end, Token op) : m_first(first), m_end(end), m_op(op) {
	m_step = nullptr;
}

RangeExpr::RangeExpr(Expr_ptr first, Expr_ptr sep, Expr_ptr end, Token op) : m_first(first), m_step(sep), m_end(end), m_op(op) {

}

void RangeExpr::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

InExpr::InExpr(Token name, Token in, Expr_ptr iterable) : m_name(name), m_inKeyword(in), m_iterable(iterable) {

}

void InExpr::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

IndexAssign::IndexAssign(Expr_ptr list, Expr_ptr index, Token indexOp, Token op, Expr_ptr val) : m_list(list), m_index(index), m_indexOp(indexOp), m_op(op), m_val(val) {

}

void IndexAssign::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}
