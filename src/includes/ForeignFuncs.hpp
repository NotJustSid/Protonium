#pragma once
#include <iostream>

#include "Callable.hpp"

class Read : public Callable {
public:
	virtual int arity() override {
		return 0;
	}
	virtual std::string info() override {
		return "<Proto::generic::foreignfn read>";
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
		return "<Proto::generic::foreignfn print>";
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
		return "<Proto::generic::foreignfn println>";
	}
	virtual Value call(const Values& args) override {
		std::cout << std::setprecision(maxPrecision) << Interpreter::getInstance().stringify(args.at(0)) << '\n';
		return nullptr;
	}
};

class Copy : public Callable {
	virtual int arity() override {
		return 1;
	}
	virtual std::string info() {
		return "<Proto::generic::foreignfn copy>";
	}
	virtual Value call(const Values& args) override {
		const Value& val = args.at(0);
		
		if (!std::holds_alternative<list_ptr>(val)) return val;

		auto& list = std::get<list_ptr>(val);
		return std::make_shared<list_t>(list->m_list, list->m_type);
	}
};