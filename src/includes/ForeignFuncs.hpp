#pragma once
#include <iostream>

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

class Print : public Callable {
public:
	virtual int arity() override {
		return 1;
	}
	virtual std::string info() {
		return "<Proto::generic::foreignfn print : (<printable-type>) -> ()>";
	}
	virtual Value call(const Values& args) override {
		std::cout << std::setprecision(maxPrecision) << Interpreter::getInstance().stringify(args.at(0));
		return nullptr;
	}
};

class Println : public Callable {
public:
	virtual int arity() override {
		return 1;
	}
	virtual std::string info() {
		return "<Proto::generic::foreignfn println : (<printable-type>) -> ()>";
	}
	virtual Value call(const Values& args) override {
		std::cout << std::setprecision(maxPrecision) << Interpreter::getInstance().stringify(args.at(0)) << '\n';
		return nullptr;
	}
};