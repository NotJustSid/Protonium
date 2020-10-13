#include "includes/Environment.hpp"
#include "includes/Interpreter.hpp"

Env_ptr Environment::getSharedPtr() {
	return shared_from_this();
}

Value& Environment::get(Token name) {
	if (m_vars.find(name.str())!=m_vars.end()) {
		return m_vars[name.str()];
	}

	if (m_parent != nullptr) {
		return m_parent->get(name);
	}

	throw RuntimeError(name, "Undefined variable '" + name.str() + "'.");
}

Value& Environment::getAt(Token name, std::size_t dist) {
	return parentAt(dist)->get(name);
}

Env_ptr Environment::parentAt(std::size_t distance) {
	Env_ptr env = getSharedPtr();
	for (std::size_t i = 0; i < distance; i++) {
		env = env->m_parent;
	}
	return env;
}

bool Environment::isDefined(const std::string& name) const {
	return (m_vars.find(name)!=m_vars.end());
}

void Environment::assign(const std::string& name, const Value& val) {
	m_vars[name] = val;
}

void Environment::strictAssign(Token name, const Value& val) {
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

void Environment::assignAt(const std::string& name, const Value& val, std::size_t dist) {
	parentAt(dist)->assign(name, val);
}

void Environment::strictAssignAt(Token name, const Value& val, std::size_t dist) {
	parentAt(dist)->strictAssign(name, val);
}

//! Global scope

Environment::Environment() : m_parent(nullptr) {

}

//! Local scope

Environment::Environment(Env_ptr env) : m_parent(env) {

}