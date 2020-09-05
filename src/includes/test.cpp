#include "Expressions.hpp"
#include <iostream>
#include "Token.hpp"

//This was just to test if the visitor pattern and the tree printer was
//working or not.
//Code is outdated

int main() {

	auto minus = Token(TokenType::MINUS, "-", 0, LiteralType::NONE);
	auto literal = Literal(Token(TokenType::NUMBER, "123", 0, LiteralType::NUM));
	auto unary = Unary(minus, literal);
	auto prod = Token(TokenType::PRODUCT, "*", 0, LiteralType::NONE);
	auto literal2 = Literal(Token(TokenType::NUMBER, "45.67", 0, LiteralType::NUM));
	auto group = ParenGroup(literal2);
	auto expr = Binary(unary, prod, group);

	TreePrinter printer;
	std::cout << printer.tree(expr);
	std::cin.get();
}