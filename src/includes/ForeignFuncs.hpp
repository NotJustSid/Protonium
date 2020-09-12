#pragma once
#include <iostream>
#include <chrono>

#include "Callable.hpp"

class Read : public Callable {
public:
	virtual int arity() override {
		return 0;
	}
	virtual std::string info() override {
		return "<Proto::generic::foreignfn read : () -> string_t>";
	}
	virtual Value call(const Values& args) override {
		std::string str;
		std::getline(std::cin, str);
		return str;
	}
};