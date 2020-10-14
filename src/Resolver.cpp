#include "includes/Resolver.hpp"

#include "proto.hpp"
#include "includes/Interpreter.hpp"
#include "includes/Lambda.hpp"

void Resolver::beginScope() {
	m_scopes.push_back({});
}

void Resolver::endScope() {
	auto& scope = m_scopes.back();
	for (auto& [name, var] : scope) {
		if (!var.hasBeenRead)
			Proto::getInstance().warn(var.line, "Unused local variable '" + name + "'.");
	}
	m_scopes.pop_back();
}

void Resolver::resolve(const Stmt_ptr stmt) {
	stmt->accept(this);
}

void Resolver::resolve(const Expr_ptr expr) {
	expr->accept(this);
}

void Resolver::define(Token name) {
	if (m_scopes.empty()) return;
	m_scopes.back()[name.str()] = { name.getLine(), false };
}

void Resolver::resolveLocal(const Expr& expr, Token name, bool hasBeenRead) {
	if (!m_scopes.empty())
		for (int i = m_scopes.size() - 1; i >= 0; i--) {
			if (m_scopes[i].find(name.str()) != m_scopes[i].end()) {
				Interpreter::getInstance().resolve(expr, m_scopes.size() - i - 1);
				if (hasBeenRead) {
					m_scopes[i].at(name.str()).hasBeenRead = true;
				}
				return;
			}
		}
}

void Resolver::resolveFunc(const Func& f) {
	bool temp = inFunction;
	inFunction = true;
	beginScope();
	for (auto& param : f.m_params) {
		define(param);
	}
	for (auto& stmt : f.m_body) {
		if (rtrnLine) {
			Proto::getInstance().warn(rtrnLine, "Redundant code after 'return' statement.");
			rtrnLine = 0;
		}
		resolve(stmt);
	}
	endScope();
	inFunction = temp;
	rtrnLine = 0;
}

void Resolver::resolveFunc(const Lambda& f) {
	bool temp = inFunction;
	inFunction = true;
	beginScope();
	for (auto& param : f.m_params) {
		define(param);
	}
	for (auto& stmt : f.m_body) {
		if (rtrnLine) {
			Proto::getInstance().warn(rtrnLine, "Redundant code after 'return' statement.");
		}
		resolve(stmt);
	}
	endScope();
	inFunction = temp;
	rtrnLine = 0;
}

bool Resolver::isInCurrentScope(const std::string& name) {
	if (m_scopes.empty()) return false;
	return m_scopes.back().find(name) != m_scopes.back().end();
}

// Expressions

Resolver& Resolver::getInstance() {
	static Resolver res;
	return res;
}

void Resolver::visit(const Binary& bin) {
	resolve(bin.m_left);
	resolve(bin.m_right);
}

void Resolver::visit(const Unary& un) {
	resolve(un.m_right);
}

void Resolver::visit(const ParenGroup& group) {
	resolve(group.m_enclosedExpr);
}

void Resolver::visit(const Literal& lit) {
	//nothing to resolve
}

void Resolver::visit(const Logical& logic) {
	resolve(logic.m_left);
	resolve(logic.m_right);
}

void Resolver::visit(const Call& call) {
	resolve(call.m_callee);
	for (auto& arg : call.m_args) {
		resolve(arg);
	}
}

void Resolver::visit(const Lambda& expr) {
	resolveFunc(expr);
}

void Resolver::visit(const ListExpr& list) {
	for (auto& expr : list.m_exprs) {
		resolve(expr);
	}
}

void Resolver::visit(const Index& index) {
	resolve(index.m_index);
	resolve(index.m_list);
}

void Resolver::visit(const RangeExpr& range) {
	resolve(range.m_first);
	if (range.m_step != nullptr) {
		resolve(range.m_step);
	}
	resolve(range.m_end);
}

void Resolver::visit(const Variable& var) {
	resolveLocal(var, var.m_name, true);
}

void Resolver::visit(const Assign& expr) {
	bool isStrictAssign = expr.m_op.getType() == TokenType::BT_EQUAL;
	bool isLazyAssign = expr.m_op.getType() == TokenType::EQUAL;

	if (isLazyAssign) {
		if (isInCurrentScope(expr.m_name.str())) {
			//! assignment
			resolve(expr.m_val);
			resolveLocal(expr, expr.m_name);
			return;
		}
		else {
			//! no variable with the name in current scope, so define one.
			resolve(expr.m_val);
			define(expr.m_name);
			if(!m_scopes.empty())	//ensure we aren't in the global env
				resolveLocal(expr, expr.m_name);
			return;
		}
	}

	if (isStrictAssign) {
		resolve(expr.m_val);
		resolveLocal(expr, expr.m_name);
		return;
	}

}


// Statements

void Resolver::visit(const Expression& stmt) {
	resolve(stmt.m_expr);
}

void Resolver::visit(const If& stmt) {
	resolve(stmt.m_condition);
	resolve(stmt.m_thenBranch);
	if (stmt.m_elseBranch != nullptr) {
		resolve(stmt.m_elseBranch);
	}
}

void Resolver::visit(const While& stmt) {
	resolve(stmt.m_condition);
	resolve(stmt.m_body);
}

void Resolver::visit(const For& stmt) {
	beginScope();
	if(stmt.m_init) resolve(stmt.m_init);
	resolve(stmt.m_condition);
	if(stmt.m_increment) resolve(stmt.m_increment);
	if (auto block = std::dynamic_pointer_cast<Block>(stmt.m_body)) {
		for (auto& stmt : block->m_stmts) {
			resolve(stmt);
		}
	}
	else resolve(stmt.m_body);
	endScope();
}

void Resolver::visit(const Func& f) {
	define(f.m_name);

	resolveFunc(f);
}

void Resolver::visit(const Return& rtrn) {
	if (!inFunction) {
		Proto::getInstance().error(rtrn.m_keyword.getLine(), "'return' statements can only be used in a function's body.");
		return;
	}
	if (rtrn.m_val != nullptr) {
		resolve(rtrn.m_val);
	}
	rtrnLine = rtrn.m_keyword.getLine();
}

void Resolver::visit(const Break&) {
	//nothing to resolve
}

void Resolver::visit(const Continue&) {
	//nothing to resolve
}

void Resolver::visit(const Block& block) {
	beginScope();
	for (auto& stmt : block.m_stmts) {
		resolve(stmt);
	}
	endScope();
}