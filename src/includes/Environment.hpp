#pragma once
#include <unordered_map>
#include <string>

#include "Token.hpp"
#include "Expressions.hpp"

class Environment {
private:
	std::unordered_map<std::string, Value> m_vars;
public:
	Value& get(Token name);
	bool isDefined(std::string name) const;
	void define(std::string name, Value val);
	Environment() = default;
};