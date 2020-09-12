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
	bool isDefined(const std::string& name) const;
	void assign(const std::string& name, Value val);
	void strictAssign(Token name, Value val);
	Environment();
	Environment(Env_ptr env);
};
