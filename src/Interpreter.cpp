#include "includes/Interpreter.hpp"
#include "proto.hpp"

RuntimeError::RuntimeError(Token t, std::string err) : m_error(err), m_tok(t) {

}

const char* RuntimeError::what() const noexcept {
	return m_error.c_str();
}

Token RuntimeError::getToken() const {
	return m_tok;
}

Interpreter::Interpreter() {
	m_env = std::make_shared<Environment>();
	m_val = nullptr;
}

bool Interpreter::isNum(const Value& val) {
	return std::holds_alternative<long double>(val);
}

bool Interpreter::isNix(const Value& val) {
	return std::holds_alternative<void*>(val);
}

bool Interpreter::isStr(const Value& val) {
	return std::holds_alternative<std::string>(val);
}

bool Interpreter::isBool(const Value& val) {
	return std::holds_alternative<bool>(val);
}

bool Interpreter::isTrue(const Value& val) {
	//? return an error if the thingy is not a bool?

	if (isNix(val)) {
		return false;
	}
	if (isBool(val)) {
		return std::get<bool>(val);
	}
	return true;
}

bool Interpreter::isEqual(const Value& left, const Value& right) {
	if (isNum(left) && isNum(right)) {
		return isEqual(std::get<long double>(left), std::get<long double>(right));
	}
	return left == right;
}

bool Interpreter::isEqual(long double left, long double right) {
	return std::fabsl(left - right) < epsilon;
}

std::string Interpreter::stringify(const Value& value, const char* strContainer) {
	if (isNix(value)) return "nix";
	if (isNum(value)) {
		std::stringstream ss;
		ss << std::setprecision(maxPrecision) << std::get<long double>(value);
		return ss.str();
	}
	if (isStr(value)) {
		return strContainer + std::get<std::string>(value) + strContainer;
	}
	if (isBool(value)) {
		return std::get<bool>(value) ? "true" : "false";
	}
}

Interpreter& Interpreter::getInstance() {
	static Interpreter i;
	return i;
}

void Interpreter::visit(const Binary& bin) {

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

void Interpreter::visit(const Unary& un) {
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

void Interpreter::visit(const ParenGroup& group) {
	group.m_enclosedExpr->accept(this);
}

void Interpreter::visit(const Literal& lit) {
	m_val = lit.m_val;
}

void Interpreter::visit(const Variable& var) {
	m_val = m_env->get(var.m_name);
}

void Interpreter::visit(const Logical& log) {
	log.m_left->accept(this);

	if (log.m_op.getType() == TokenType::OR) {
		if (isTrue(m_val)) {
			//the left side is true
			m_val = true;
		}
		else {
			log.m_right->accept(this);
			if (isTrue(m_val)) {
				//the left is false and right side is true
				m_val = true;
			}
			else {
				//both the left and right side are false
				m_val = false;
			}
		}
	}
	else {
		//and
		if (!isTrue(m_val)) {
			//the left side is false
			m_val = false;
		}
		else {
			log.m_right->accept(this);
			if (isTrue(m_val)) {
				//both the left side and the right side are true
				m_val = true;
			}
			else {
				//the left side is true but the right side is false
				m_val = false;
			}
		}
	}
}

void Interpreter::visit(const Assign& expr) {
	expr.m_val->accept(this);

	m_env->assign(expr.m_name.str(), m_val);
}


void Interpreter::visit(const Print& print) {
	print.m_expr->accept(this);
	std::cout << std::setprecision(maxPrecision) << stringify(m_val);
}

void Interpreter::visit(const Expression& expr) {
	expr.m_expr->accept(this);
}

void Interpreter::visit(const Block& block) {
	executeBlock(block.m_stmts, std::make_shared<Environment>(m_env));
}

void Interpreter::visit(const If& ifStmt) {

	ifStmt.m_condition->accept(this);
	if (isTrue(m_val)) {
		execute(ifStmt.m_thenBranch);
	}
	else if (ifStmt.m_elseBranch != nullptr) {
		execute(ifStmt.m_elseBranch);
	}

}

void Interpreter::visit(const While& whilestmt) {
	whilestmt.m_condition->accept(this);
	while (isTrue(m_val)) {
		execute(whilestmt.m_body);
	}
}

void Interpreter::execute(Stmt_ptr stmt) {
	stmt->accept(this);
}

void Interpreter::executeBlock(Stmts stmts, Env_ptr env)
{
	Env_ptr parent = m_env;
	
	try {
		m_env = env;

		for (auto stmt : stmts) {
			execute(stmt);
		}

		m_env = parent;
	}
	catch (const RuntimeError& err) {
		m_env = parent;
		Proto::getInstance().runtimeError(err);
	}
}

void Interpreter::interpret(const std::vector<Stmt_ptr>& stmts) {
	try {
		for (auto& stmt : stmts) {
			execute(stmt);
		}
	}
	catch (const RuntimeError& err) {
		Proto::getInstance().runtimeError(err);
	}
}

std::string Interpreter::interpret(Expr_ptr expr) {
	try {
		expr->accept(this);
		return stringify(m_val, "\"");
	}
	catch (const RuntimeError& err) {
		Proto::getInstance().runtimeError(err);
		return "";
	}
}