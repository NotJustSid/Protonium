#pragma once
#include "Expressions.hpp"

class Callable {
public:
	virtual Value call(const Values& args) = 0;
	virtual int arity() = 0;
	virtual std::string info() = 0;
};