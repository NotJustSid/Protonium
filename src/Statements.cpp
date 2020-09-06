#include "includes/Statements.hpp"

Expression::Expression(Expr_ptr expr) : m_expr(expr) {

}

void Expression::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

Print::Print(Expr_ptr expr) : m_expr(expr) {

}

void Print::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}
