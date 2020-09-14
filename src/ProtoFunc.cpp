#include "includes/ProtoFunc.hpp"
#include "includes/Interpreter.hpp"
#include "includes/ReturnThrow.hpp"

ProtoFunction::ProtoFunction(Token name, const std::vector<Token>& params, Stmts body) : m_name(name.str()), m_params(params), m_body(body) {

}

ProtoFunction::ProtoFunction(const std::string& name, const std::vector<Token>& params, Stmts body) : m_name(name), m_params(params), m_body(body) {

}

int ProtoFunction::arity() {
	return m_params.size();
}

std::string ProtoFunction::info() {
	return (m_name == "") ? "<Proto::generic::userfn::lambda>" :"<Proto::generic::userfn " + m_name + ">";
}

Value ProtoFunction::call(const Values& args) {
	Env_ptr callEnv = std::make_shared<Environment>(Interpreter::getInstance().m_env);

	for (std::size_t i = 0; i < args.size(); i++) {
		callEnv->assign(m_params[i].str(), args[i]);
	}
	
	try {
		Interpreter::getInstance().executeBlock(m_body, callEnv);
	}
	catch (const ReturnThrow& rtrn) {
		return rtrn.m_val;
	}
	
	return nullptr;
}
