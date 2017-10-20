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

	vector<Str> operators = { "if", "for", "while", "return"};

	vector<Str> ari_opers = { "=", "+", "-", "*", "/", "%" };

	Parser::Parser(Lexer *lexer) : lang::Parser(lexer) {

	}

	bool Parser::doMainCodeBlock() {
		while (true) {
			if (doMainStatement(nullptr)) continue;
			return false;
		}
	}

	bool Parser::doCodeBlock(Node *parent) {
		Node *node = new CodeBlock();
		addNode(parent, node);
		while (true) {
			if (isSpecial("}")) return true;
			if (doStatement(node)) continue;
			return false;
		}
	}

	bool Parser::doMainStatement(Node *parent) {
		if (isSpecial(";")) return true;
		if (isSpecial("{")) {
			return doCodeBlock(parent);
		}
		if (isOperator()) {
			return doOperator(parent);
		}

		if (isStdType()) {
			if (!isIdentifier()) return false;
			if (isSpecial("(")) return doFuncDef(parent);
			return doVarDef(parent);
		}
		
		if (isIdentifier()) {
			if (isSpecial("(")) return doFuncDef(parent);
			if (doVarDef(parent)) return true;
			decPosition();
			return doExpression(parent);
		}

		return doExpression(parent);
	}

	bool Parser::doStatement(Node *parent) {
		if (isSpecial(";")) return true;
		if (isSpecial("{")) {
			return doCodeBlock(parent);
		}
		if (isOperator()) {
			return doOperator(parent);
		}

		if (isStdType()) {
			if (!isIdentifier()) return false;
			return doVarDef(parent);
		}

		if (isIdentifier()) {
			if (doVarDef(parent)) return true;
			decPosition();
			return doExpression(parent);
		}

		return doExpression(parent);
	}

	bool Parser::doFunc(Node *parent) {
		int pos;
		savePosition(pos);
		if (find(")")) {
			if (isSpecial(";")) {
				rollback(pos);
				return doFuncCall(parent);
			}
			if (isSpecial("{")) {
				rollback(pos);
				return doFuncDef(parent);
			}
			return false;
		}
	}

	bool Parser::doFuncCall(Node *parent) {
		if (parent == nullptr || parent->nodeType != ntExpression) {
			decPosition();
			decPosition();
			doExpression(parent);
		}
		else {
			FuncDef *fd = findFuncDef(parent->parent, identifier);
			if (!fd) {
				FuncDef *fd = new FuncDef();
				fd->name = identifier;
				fd->type = "auto";
				addNode(parent->parent, fd);
			}

			Func *func = new Func();
			func->def = fd;
			addNode(parent, func);
			if (isSpecial(")")) return true;
			while (true) {
				if (doExpression(func)) {
					if (isSpecial(")")) return true;
					if (isSpecial(",")) continue;
				}
				return false;
			}
		}
		return true;
	}

	bool Parser::doFuncDef(Node *parent) {
		FuncDef *fd = new FuncDef();
		fd->name = identifier;
		if (std_type == "") fd->type = "auto"; else fd->type = std_type;
		addNode(parent, fd);
		if (!doFuncDefParams(fd)) return false;
		if (!doFuncDefBody(fd)) return false;

		return true;
	}

	bool Parser::doFuncDefParams(FuncDef *fd) {
		if (isSpecial(")")) return true;
		while (true) {
			isStdType();
			if (isIdentifier()) {
				FuncDefParam *param = new FuncDefParam();
				param->type = std_type;
				param->name = identifier;

				fd->params.insert(fd->params.end(), param);

				if (isSpecial(")")) return true;
				if (isSpecial(",")) continue;
			}
		}
	}

	bool Parser::doFuncDefBody(Node *parent) {
		if (!isSpecial("{")) return false;
		return doCodeBlock(parent);
	}

	bool Parser::doVarDef(Node *parent) {
		if (findVarDef(parent, identifier)) return false;

		VarDef *vd = new VarDef();
		vd->name = identifier;
		if (std_type == "") vd->type = "auto"; else vd->type = std_type;
		addNode(parent, vd);

		if (isSpecial("=")) {
			Expression *exp = new Expression();
			addNode(parent, exp);

			Var *var = new Var();
			var->def = vd;
			addNode(exp, var);

			ExpOper *oper = new ExpOper();
			oper->name = "=";
			oper->count = 2;
			addNode(exp, oper);

			return doExpression(exp, false);
		}
		return isSpecial(";");
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

	bool Parser::doOperator(Node *parent) {
		if (oper == "if") return doOperatorIf(parent);
		else if (oper == "for") return doOperatorFor(parent);
		else if (oper == "while") return doOperatorWhile(parent);
		else if (oper == "return") return doOperatorReturn(parent);
		return false;
	}

	bool Parser::doOperatorIf(Node *parent) {
		if (!isSpecial("(")) return false;
		Operator *oper = new Operator();
		oper->name = "if";
		addNode(parent, oper);
		if (!doExpression(oper)) return false;
		if (!isSpecial(")")) return false;
		if (!doStatement(oper)) return false;
		if (!isLexeme("else")) return true;
		if (!doStatement(oper)) return false;
		return true;
	}

	bool Parser::doOperatorFor(Node *parent) {
		if (!isSpecial("(")) return false;
		Operator *oper = new Operator();
		oper->name = "for";
		addNode(parent, oper);
		if (!doExpression(oper)) return false;
		if (!doExpression(oper)) return false;
		if (!doExpression(oper)) return false;
		if (!isSpecial(")")) return false;
		if (!doStatement(oper)) return false;
		return true;
	}

	bool Parser::doOperatorWhile(Node *parent) {
		if (!isSpecial("(")) return false;
		Operator *oper = new Operator();
		oper->name = "while";
		addNode(parent, oper);
		if (!doExpression(oper)) return false;
		if (!isSpecial(")")) return false;
		if (!doStatement(oper)) return false;
		return true;
	}

	bool Parser::doOperatorReturn(Node *parent) {
		Node *p = parent;
		if (parent->nodeType == ntCodeBlock) p = p->parent;
		FuncDef *fd = (FuncDef*)p;
		Operator *oper = new Operator();
		oper->name = "return";
		addNode(parent, oper);
		if (fd->type != "void") {
			if (doExpression(oper)) return true;
		}
		return isSpecial(";");
	}

	bool Parser::doExpression(Node *node, bool isCreateExpressionNode) {
		Expression *expression = (Expression*)node;
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
			if (isSpecial(";")) expression->isTZ = true;
			break;
		}
		return true;
	}

	bool Parser::doUnatyOperator(Node *parent) {
		bool flag = false;
		if (isSpecial(ari_opers[1])) flag = true;
		else if (isSpecial(ari_opers[2])) flag = true;

		if (flag) {
			ExpOper *oper = new ExpOper();
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
				ExpOper *oper = new ExpOper();
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

	bool Parser::isOperator() {
		std_type = "";
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			int count = operators.size();
			for (int i = 0; i < count; i++) {
				if (operators[i] == token.lexeme) {
					oper = token.lexeme;
					return true;
				}
			}
		}
		rollback();
		return false;
	}

	bool Parser::isOperator(Str s) {
		if (!isOperator()) return false;
		if (oper == s) return true;
		return true;
	}

	//bool Parser::isKeyword() {
	//	keyword = "";
	//	if (isStdType()) {
	//		keyword = std_type;
	//		return true;
	//	}
	//	if (isOperator()) {
	//		keyword = oper;
	//		return true;
	//	}
	//	return false;
	//}



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

	bool Parser::isLexeme(Str s) {
		savePosition();
		token = getToken();
		if (token.lexeme == s) return true;

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
			if (token.type == ltEof) return false;
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
				if (vd->name == var) return vd;
			}
		}
		return findVarDef(parent->parent, var);
		//return nullptr;
	}

	FuncDef* Parser::findFuncDef(Node *parent, Str func) {
		if (parent == nullptr) {
			int count = nodes.size();
			for (int i = 0; i < count; i++) {
				Node *node = nodes[i];
				if (node->nodeType == ntFuncDef) {
					FuncDef *fd = (FuncDef*)node;
					if (fd->name == func) return fd;
				}
			}
			return nullptr;
		}
		int count = parent->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *node = parent->nodes[i];
			if (node->nodeType == ntFuncDef) {
				FuncDef *fd = (FuncDef*)node;
				if (fd->name == func) return fd;
			}
		}
		return nullptr;
	}

}
