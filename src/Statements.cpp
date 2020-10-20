#include "includes/Statements.hpp"

Expression::Expression(Expr_ptr expr) : m_expr(expr) {

}

void Expression::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

Block::Block(const Stmts& stmts) : m_stmts(stmts) {

}

void Block::accept(StmtVisitor* visitor) const {
	return visitor->visit(*this);
}

If::If(Expr_ptr condition, Stmt_ptr thenBranch, Stmt_ptr elseBranch) : m_condition(condition), m_thenBranch(thenBranch), m_elseBranch(elseBranch) {

}

void If::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

While::While(Expr_ptr condition, Stmt_ptr body) : m_condition(condition), m_body(body) {

}

void While::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

For::For(Expr_ptr init, Expr_ptr cond, Expr_ptr increment, Stmt_ptr body) : m_init(init), m_condition(cond), m_increment(increment), m_body(body) {

}

void For::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

Func::Func(Token name, const std::vector<Token>& params, const Stmts& body) : m_name(name), m_params(params), m_body(body) {

}

void Func::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

Return::Return(Token keyword, Expr_ptr val) : m_keyword(keyword), m_val(val) {

}

void Return::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

void Break::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}

void Continue::accept(StmtVisitor* visitor) const {
	visitor->visit(*this);
}
