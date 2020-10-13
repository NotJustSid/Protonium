#pragma once
#include <unordered_map>
#include <string>

#include "Token.hpp"
#include "Expressions.hpp"

class Environment;
using Env_ptr = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment>{
private:
	std::unordered_map<std::string, Value> m_vars;
	Env_ptr m_parent; //enclosing scope
private:
	Env_ptr getSharedPtr();
public:
	Value& get(Token name);
	Value& getAt(Token name, std::size_t dist);
	Env_ptr parentAt(std::size_t distance);
	bool isDefined(const std::string& name) const;
	void assign(const std::string& name, const Value& val);
	void assignAt(const std::string& name, const Value& val, std::size_t dist);
	void strictAssign(Token name, const Value& val);
	void strictAssignAt(Token name, const Value& val, std::size_t dist);
	Environment();
	Environment(Env_ptr env);
};
