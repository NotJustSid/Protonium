#include "includes/Expressions.hpp"
#include "proto.hpp"

Interpreter& Interpreter::getInstance() {
	static Interpreter i;
	return i;
}

void Interpreter::interpret(Expr_ptr expr) {
	try {
		expr->accept(this);
		std::cout << std::setprecision(maxPrecision) << stringify(m_val) << '\n';
	}
	catch (const RuntimeError& err) {
		Proto::getInstance().runtimeError(err);
	}
}
