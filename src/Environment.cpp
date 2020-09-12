#include "includes/Environment.hpp"
#include "includes/Interpreter.hpp"

Value& Environment::get(Token name) {
	if (m_vars.contains(name.str())) {
		return m_vars[name.str()];
	}

	if (m_parent != nullptr) {
		return m_parent->get(name);
	}

	throw RuntimeError(name, "Undefined variable '" + name.str() + "'.");
}

bool Environment::isDefined(const std::string& name) const {
	return m_vars.contains(name);
}

void Environment::assign(const std::string& name, Value val) {
	m_vars[name] = val;
}

void Environment::strictAssign(Token name, Value val) {
	if (m_vars.contains(name.str())) {
		m_vars[name.str()] = val;
	}
	else if (m_parent != nullptr) {
		m_parent->strictAssign(name, val);
	}
	else {
		throw RuntimeError(name, "Undefined variable '" + name.str() + "'.");
	}
}

//! Global scope

Environment::Environment() : m_parent(nullptr) {

}

//! Local scope

Environment::Environment(Env_ptr env) : m_parent(env) {

}