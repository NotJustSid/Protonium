#include "includes/Expressions.hpp"

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

void Literal::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}

Variable::Variable(Token name) : m_name(name) {

}

void Variable::accept(ExprVisitor* visitor) const {
	visitor->visit(*this);
}
