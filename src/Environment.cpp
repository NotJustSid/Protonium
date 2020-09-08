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

bool Environment::isDefined(std::string name) const {
	return m_vars.contains(name);
}

void Environment::define(std::string name, Value val) {
	m_vars[name] = val;
}

//! Global scope

Environment::Environment() : m_parent(nullptr) {

}

//! Local scope

Environment::Environment(Env_ptr env) : m_parent(env) {

}