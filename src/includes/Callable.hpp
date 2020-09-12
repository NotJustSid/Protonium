#pragma once
#include "Expressions.hpp"

using Values = std::vector<Value>;

class Callable {
public:
	virtual Value call(const Values& args) = 0;
	virtual int arity() = 0;
	virtual std::string info() = 0;
};