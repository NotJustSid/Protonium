#include "includes/Environment.hpp"
#include "includes/Interpreter.hpp"

Value& Environment::get(Token name) {
	if (m_vars.find(name.str())!=m_vars.end()) {
		return m_vars[name.str()];
	}

	if (m_parent != nullptr) {
		return m_parent->get(name);
	}

	throw RuntimeError(name, "Undefined variable '" + name.str() + "'.");
}

bool Environment::isDefined(const std::string& name) const {
	return (m_vars.find(name)!=m_vars.end());
}

void Environment::assign(const std::string& name, Value val) {
	m_vars[name] = val;
}

void Environment::strictAssign(Token name, Value val) {
	if (m_vars.find(name.str())!=m_vars.end()) {
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