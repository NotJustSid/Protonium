#include <cmath>

#include "includes/Interpreter.hpp"
#include "includes/ForeignFuncs.hpp"
#include "includes/ReturnThrow.hpp"
#include "includes/Lambda.hpp"
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
	m_global = std::make_shared<Environment>();
	m_env = m_global;
	m_val = nullptr;

	Value readfunc = std::make_shared<Read>();
	Value printfunc = std::make_shared<Print>();
	Value printlnfunc = std::make_shared<Println>();
	m_global->assign("read", readfunc);
	m_global->assign("print", printfunc);
	m_global->assign("println", printlnfunc);
}

bool Interpreter::isNum(const Value& val) {
	return std::holds_alternative<long double>(val);
}

bool Interpreter::isNix(const Value& val) {
	return std::holds_alternative<nullptr_t>(val);
}

bool Interpreter::isStr(const Value& val) {
	return std::holds_alternative<std::string>(val);
}

bool Interpreter::isBool(const Value& val) {
	return std::holds_alternative<bool>(val);
}

bool Interpreter::isTrue(const Value& val) {
	if (isNix(val)) {
		return false;
	}
	if (isBool(val)) {
		return std::get<bool>(val);
	}
	if (isNum(val) && isEqual(std::get<long double>(val), 0)) {
		return false;
	}
	return true;
}

bool Interpreter::isCallable(const Value& val) {
	return std::holds_alternative<Callable_ptr>(val);
}

bool Interpreter::isList(const Value& val) {
	return std::holds_alternative<list_ptr>(val);
}

bool Interpreter::isEqual(const Value& left, const Value& right) {
	if (isNum(left) && isNum(right)) {
		return isEqual(std::get<long double>(left), std::get<long double>(right));
	}
	if (isList(left) && isList(right)) {
		auto leftList = std::get<list_ptr>(left)->m_list;
		auto leftListType = std::get<list_ptr>(left)->m_type;

		auto rightList = std::get<list_ptr>(right)->m_list;
		auto rightListType = std::get<list_ptr>(right)->m_type;

		if (leftListType != rightListType) return false;
		if (leftList.size() != rightList.size()) return false;

		for (std::size_t i = 0; i < leftList.size(); i++) {
			if (!isEqual(leftList[i], rightList[i])) return false;
		}
		return true;
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
	if (isCallable(value)) {
		return std::get<Callable_ptr>(value)->info();
	}
	if (isList(value)) {
		auto list = std::get<list_ptr>(value);
		std::string str = "[";
		for (auto& val : list->m_list) {
			str += stringify(val, strContainer);
			str += ", ";
		}
		if (!list->m_list.empty()) {
			str.pop_back();
			str.pop_back();
		}
		str += "]";
		return str;
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
	case TokenType::EXPONENTATION:
		if (!numOperands) {
			throw RuntimeError(bin.m_op, "Operands must be numbers.");
		}
		m_val = std::powl(std::get<long double>(left), std::get<long double>(right));
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

	if (expr.m_op.getType() == TokenType::EQUAL) {
		m_env->assign(expr.m_name.str(), m_val);
	}
	else if (expr.m_op.getType() == TokenType::BT_EQUAL) {
		m_env->strictAssign(expr.m_name, m_val);
	}
}

void Interpreter::visit(const Call& expr) {
	expr.m_callee->accept(this);
	auto callee = m_val;

	std::vector<Value> args;
	for (auto arg : expr.m_args) {
		arg->accept(this);
		args.push_back(m_val);
	}

	if (!isCallable(callee)) {
		throw RuntimeError(expr.m_paren, "Provided object is not callable.");
	}

	auto fn = std::get<Callable_ptr>(callee);
	if (fn->arity() != args.size()) {
		std::string err = "Expected " + std::to_string(fn->arity()) + " arguments but got " + std::to_string(args.size()) + " arguments.";

		throw RuntimeError(expr.m_paren, err);
	}

	m_val = fn->call(args);
}

void Interpreter::visit(const Lambda& expr) {
	m_val = std::make_shared<ProtoFunction>("", expr.m_params, expr.m_body);
}

void Interpreter::visit(const ListExpr& expr) {
	Values values;
	std::size_t type = 999; //999 == empty list
	bool first = true;
	for (auto& exp : expr.m_exprs) {
		exp->accept(this);
		values.push_back(m_val);
		if (first) {
			type = m_val.index();
			first = false;
		}
		if (type != m_val.index()) {
			throw RuntimeError(expr.m_brkt, "Lists are homogenous and can't contain different types.");
		}
	}
	m_val = std::make_shared<list_t>(values, static_cast<list_t::Type>(type));
}

void Interpreter::visit(const Index& expr) {
	expr.m_list->accept(this);
	
	if (!isList(m_val)) {
		throw RuntimeError(expr.m_indexOp, "The index operator can only be used on lists.");
	}

	auto list = std::get<list_ptr>(m_val);

	expr.m_index->accept(this);
	auto index = m_val;

	if (isList(index)) {
		auto listIndex = std::get<list_ptr>(index);

		if (listIndex->m_type == list_t::Type::emptyList) {
			throw RuntimeError(expr.m_indexOp, "Cannot use an empty list as an index.");
		}
		if (listIndex->m_type != list_t::Type::numList) {
			throw RuntimeError(expr.m_indexOp, "The indexing list must contain numbers.");
		}
		Values val;
		for (auto i : listIndex->m_list) {
			//TODO temp
			auto num = std::lround(std::get<long double>(i));
			if (num <= 0) throw RuntimeError(expr.m_indexOp, "Indices can't be negative or zero.");

			if(num > list->m_list.size()) throw RuntimeError(expr.m_indexOp, "One or more of the indices is greater than the length of the list.");
			
			//num-1 because indexing is 1-based.
			val.push_back(list->m_list[num-1]);
		}
		m_val = std::make_shared<list_t>(val, list->m_type);
	}
	else {
		if (!isNum(index)) {
			throw RuntimeError(expr.m_indexOp, "The index must be a list or a number.");
		}
		//TODO temp
		auto i = std::lround(std::get<long double>(index));
		
		if (i <= 0) throw RuntimeError(expr.m_indexOp, "Indices can't be negative or zero.");

		if (i > list->m_list.size()) throw RuntimeError(expr.m_indexOp, "The index is greater than the length of the list.");

		m_val = list->m_list[i-1];
	}
}

void Interpreter::visit(const RangeExpr& expr) {
	expr.m_first->accept(this);
	if (!isNum(m_val)) {
		throw RuntimeError(expr.m_op, "Ranges can only contain numeric descriptors.");
	}
	long double first = std::get<long double>(m_val);

	long double step = 1;
	if (expr.m_step != nullptr) {
		expr.m_step->accept(this);
		if (!isNum(m_val)) {
			throw RuntimeError(expr.m_op, "Ranges can only contain numeric descriptors.");
		}
		step = std::get<long double>(m_val);
		if (isEqual(step, 0)) {
			throw RuntimeError(expr.m_op, "Range step cannot be 0.");
		}
	}

	expr.m_end->accept(this);
	if (!isNum(m_val)) {
		throw RuntimeError(expr.m_op, "Ranges can only contain numeric descriptors.");
	}
	long double end = std::get<long double>(m_val);

	Values rangeList;

	for (auto i = first; i <= end; i += step) {
		rangeList.push_back(i);
	}

	m_val = std::make_shared<list_t>(rangeList, list_t::Type::numList);
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
	try{
		while (isTrue(m_val)) {
			execute(whilestmt.m_body);
			whilestmt.m_condition->accept(this);
		}
	}
	catch (const BreakThrow&) {
		//we broke out of the loop
	}
}

void Interpreter::visit(const Break& breakstmt) {
	throw BreakThrow();
}

void Interpreter::visit(const Func& func) {
	auto fn = std::make_shared<ProtoFunction>(func.m_name, func.m_params, func.m_body);
	m_env->assign(func.m_name.str(), fn);
}

void Interpreter::visit(const Return& stmt) {
	if (stmt.m_val != nullptr) {
		stmt.m_val->accept(this);
	}
	else m_val = nullptr;
	throw ReturnThrow(m_val);
}

void Interpreter::execute(Stmt_ptr stmt) {
	stmt->accept(this);
}

void Interpreter::executeBlock(Stmts stmts, Env_ptr env) {
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
	catch (const ReturnThrow& rtrn) {
		//! Make sure the environment is reset before undwinding all the 
		//! way back to the call...
		m_env = parent;
		throw rtrn;
	}
}

void Interpreter::interpret(const Stmts& stmts) {
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
		if (!(std::dynamic_pointer_cast<Call>(expr) && isNix(m_val))) {
			return stringify(m_val, "\"");
		}
		else return "";
	}
	catch (const RuntimeError& err) {
		Proto::getInstance().runtimeError(err);
		return "";
	}
}