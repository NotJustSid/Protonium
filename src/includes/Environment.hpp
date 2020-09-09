#pragma once
#include <unordered_map>
#include <string>

#include "Token.hpp"
#include "Expressions.hpp"

class Environment;
using Env_ptr = std::shared_ptr<Environment>;

class Environment {
private:
	std::unordered_map<std::string, Value> m_vars;
	Env_ptr m_parent; //enclosing scope
public:
	Value& get(Token name);
	bool isDefined(std::string name) const;
	void assign(std::string name, Value val);
	void strictAssign(std::string name, Value val);
	//! Global scope
	Environment();
	//! Local scope
	Environment(Env_ptr env);
};
