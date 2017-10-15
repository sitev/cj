#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj {

	vector<Str> std_types = {
		"auto", "void", "bool", "byte", "short", "int", "long", "ubyte", "ushort", "uint", "ulong",
		"float", "double", "real", "char"
	};

	vector<Str> ari_opers = { "=", "+", "-", "*", "/", "%" };

	Parser::Parser(Lexer *lexer) : lang::Parser(lexer) {

	}

	bool Parser::doMainCodeBlock() {
		while (true) {
			if (isSpecial(";")) continue;
			if (isSpecial("{")) {
				if (doCodeBlock(nullptr)) 
					continue;
				return false;
			}
			if (doMainStatement(nullptr)) continue;
			return false;
		}
	}

	bool Parser::doCodeBlock(Node *parent) {
		Node *node = new CodeBlock();
		addNode(parent, node);
		while (true) {
			if (isSpecial(";")) continue;
			if (isSpecial("{")) {
				if (doCodeBlock(node)) continue;
				return false;
			}
			if (isSpecial("}")) return true;
			if (doStatement(node)) continue;
			return false;
		}
	}

	bool Parser::doMainStatement(Node *parent) {
		if (isKeyword() && isIdentifier()) {
			if (isSpecial("(") && doFuncDef(parent)) return true;
			if (doVarDef(parent)) return true;
			return false;
		}
		if (isIdentifier()) {
			if (isSpecial("(") && doFunc(parent)) return true;
			if (doVar(parent)) return true;
			return false;
		}
		return false;
	}

	bool Parser::doStatement(Node *parent) {
		if (isKeyword() && isIdentifier() && doVarDef(parent)) return true;
		if (isIdentifier()) {
			if (isSpecial("(") && doFuncCall(parent)) return true;
			if (doVar(parent)) return true;
			return false;
		}
		return false;
	}

	bool Parser::doFunc(Node *parent) {
		if (find(")")) {
			if (isSpecial(";") && doFuncCall(parent)) return true;
			if (isSpecial("{") && doFuncDef(parent)) return true;
			return false;
		}
	}

	bool Parser::doFuncCall(Node *parent) {
		Node *node = new Func();
		addNode(parent, node);
		if (isSpecial(")")) return true;
		while (true) {
			if (doExpression(node)) {
				if (isSpecial(")")) return true;
				if (isSpecial(",")) continue;
			}
			return false;
		}
	}

	bool Parser::doFuncDef(Node *parent) {
		FuncDef *node = new FuncDef();
		addNode(parent, node);
		if (!doFuncDefParams(node)) return false;
		if (!doFuncDefBody(node)) return false;

		return true;
	}

	bool Parser::doFuncDefParams(FuncDef *fd) {
		if (isSpecial(")")) return true;
		while (true) {
			isKeyword();
			if (isIdentifier()) {
				FuncDefParam *param = new FuncDefParam();
				param->type = keyword;
				param->name = identifier;
				
				fd->params.insert(fd->params.end(), param);

				if (isSpecial(")")) return true;
				if (isSpecial(",")) continue;
			}
		}
	}

	bool Parser::doFuncDefBody(Node *parent) {
		Node *node = new CodeBlock();
		addNode(parent, node);
		return doCodeBlock(node);
	}

	//bool Parser::doVar(Node *parent) {
	//	if (doVarDef(parent)) {
	//		return true;
	//	}
	//	if (doVarUse(parent)) {
	//		return true;
	//	}
	//	return false;
	//}

	bool Parser::doVarDef(Node *parent) {
		if (findVarDef(parent, identifier)) return false;

		VarDef *vd = new VarDef();
		vd->name = identifier;
		if (keyword == "") vd->type = "auto";
		else vd->type = keyword;
		addNode(parent, vd);

		if (isSpecial("=")) {
			Expression *exp = new Expression();
			addNode(parent, exp);

			Var *var = new Var();
			var->def = vd;
			addNode(exp, var);

			Operator *oper = new Operator();
			oper->name = "=";
			oper->count = 2;
			addNode(exp, oper);

			if (!doExpression(exp, false)) return false;
		}
		return true;
	}

	bool Parser::doVar(Node *parent) {
		if (parent == nullptr || parent->nodeType != ntExpression) {
			decPosition();
			doExpression(parent);
		}
		else {
			VarDef *vd = findVarDef(parent->parent, identifier);
			if (!vd) {
				VarDef *vd = new VarDef();
				vd->name = identifier;
				vd->type = "auto";
				addNode(parent->parent, vd);
			}

			Var *node = new Var();
			node->def = vd;
			addNode(parent, node);
		}
		return true;
	}

	bool Parser::doNumber(Node *parent) {
		Number *node = new Number();
		node->value = number;
		addNode(parent, node);
		return true;
	}

	bool Parser::doExpression(Node *node, bool isCreateExpressionNode) {
		Node *expression = node;
		if (isCreateExpressionNode) {
			expression = new Expression();
			addNode(node, expression);
		}

		doUnatyOperator(expression);
		while (true) {
			if (isSpecial("(")) {
				if (!doExpression(expression, false)) return false;
				if (isSpecial(")")) return false;
			}
			else if (isNumber()) {
				doNumber(expression);
			}
			else if (isIdentifier()) {
				if (isSpecial("(")) {
					if (!doFuncCall(expression)) return false;
				}
				else {
					if (!doVar(expression)) return false;
				}
			}
			else return false;

			if (doBinatyOperator(expression)) continue;
			break;
		}
		return true;
	}

	bool Parser::doUnatyOperator(Node *parent) {
		bool flag = false;
		if (isSpecial(ari_opers[1])) flag = true;
		else if (isSpecial(ari_opers[2])) flag = true;

		if (flag) {
			Operator *oper = new Operator();
			oper->name = token.lexeme;
			oper->count = 2;
			addNode(parent, oper);
			return true;
		}
		return false;
	}

	bool Parser::doBinatyOperator(Node *parent) {
		int count = ari_opers.size();
		for (int i = 0; i < count; i++) {
			if (isSpecial(ari_opers[i])) {
				Operator *oper = new Operator();
				oper->name = ari_opers[i];
				oper->count = 2;
				addNode(parent, oper);

				return true;
			}
		}
		return false;
	}



	bool Parser::isIdentifier() {
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			identifier = token.lexeme;
			return true;
		}

		rollback();
		return false;
	}

	bool Parser::isStdType() {
		std_type = "";
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			int count = std_types.size();
			for (int i = 0; i < count; i++) {
				if (std_types[i] == token.lexeme) {
					std_type = token.lexeme;
					return true;
				}
			}
		}
		rollback();
		return false;
	}
	
	bool Parser::isKeyword() {
		keyword = "";
		if (isStdType()) {
			keyword = std_type;
			return true;
		}
		return false;
	}

	bool Parser::isNumber() {
		savePosition();
		token = getToken();
		if (token.type == ltNumber) {
			number = stoi(token.lexeme.to_string());
			return true;
		}

		rollback();
		return false;
	}

	bool Parser::isSpecial(Str s) {
		savePosition();
		token = getToken();
		if (token.type == ltSpecial) {
			if (token.lexeme == s) return true;
		}

		rollback();
		return false;
	}

	bool Parser::find(Str s) {
		while (true) {
			Token token = getToken();
			if (token.lexeme == s) return true;
			if (token.type == ltEnd) return false;
		}
	}

	void Parser::addNode(Node *parent, Node *node) {
		node->parent = parent;

		if (parent == nullptr) nodes.insert(nodes.end(), node);
		else parent->nodes.insert(parent->nodes.end(), node);
	}

	VarDef* Parser::findVarDef(Node *parent, Str var) {
		if (parent == nullptr) {
			int count = nodes.size();
			for (int i = 0; i < count; i++) {
				Node *node = nodes[i];
				if (node->nodeType == ntVarDef) {
					VarDef *vd = (VarDef*)node;
					if (vd->name == var) return vd;
				}
			}
			return nullptr;
		}
		int count = parent->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *node = parent->nodes[i];
			if (node->nodeType == ntVarDef) {
				VarDef *vd = (VarDef*)node;
				if (vd->name == var) return nullptr;
			}
		}
	}

}
