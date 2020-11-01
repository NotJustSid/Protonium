#pragma once
#include <unordered_map>

#include "Expressions.hpp"
#include "Statements.hpp"

class Resolver : public ExprVisitor, public StmtVisitor {
private:
	struct VarInfo {
		std::size_t line = 0;
		bool hasBeenRead = false;
	};

	std::vector<std::unordered_map<std::string, VarInfo>> m_scopes;
	
	bool inFunction = false;
	std::size_t rtrnWarnLine = 0;

	bool inControlFlow = false;
	bool inRangedFor = false;

private:
	void beginScope();
	void endScope();

	void define(Token name);
	
	void resolveLocal(const Expr& expr, Token name, bool hasBeenRead = false);
	void resolveFunc(const Func& f);
	void resolveFunc(const Lambda& f);

	bool isInCurrentScope(const std::string& name);

	Resolver() = default;

public:

	static Resolver& getInstance();

	void resolve(const Stmt_ptr stmt);
	void resolve(const Expr_ptr expr);

	// Inherited via ExprVisitor
	virtual void visit(const Binary&) override;
	virtual void visit(const Unary&) override;
	virtual void visit(const ParenGroup&) override;
	virtual void visit(const Literal&) override;
	virtual void visit(const Logical&) override;
	virtual void visit(const Call&) override;
	virtual void visit(const Lambda&) override;
	virtual void visit(const ListExpr&) override;
	virtual void visit(const Index&) override;
	virtual void visit(const RangeExpr&) override;
	virtual void visit(const Variable&) override;
	virtual void visit(const Assign&) override;
	virtual void visit(const IndexAssign&) override;
	virtual void visit(const InExpr&) override;

	// Inherited via StmtVisitor
	virtual void visit(const Expression&) override;
	virtual void visit(const If&) override;
	virtual void visit(const While&) override;
	virtual void visit(const For&) override;
	virtual void visit(const RangedFor&) override;
	virtual void visit(const Func&) override;
	virtual void visit(const Return&) override;
	virtual void visit(const Break&) override;
	virtual void visit(const Continue&) override;
	virtual void visit(const Block& block) override;
};