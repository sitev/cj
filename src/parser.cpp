#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj {

	vector<Str> std_types = {
		"auto", "void", "bool", "byte", "short", "int", "long", "ubyte", "ushort", "uint", "ulong",
		"float", "double", "real", "char", "str", "string", "ustring"
	};

	vector<Str> operators = { "if", "for", "while", "return", "from" };

	vector<Str> ari_opers = { "=", "+", "-", "*", "/", "%", "==", "!=", ">", "<", ">=", "<=", "." };

	FuncDef::FuncDef() {

	}

	Class::Class() {

	}

	CodeInsertion::CodeInsertion() {

	}

	Parser::Parser(lang::Lexer *lexer, int nPass) : lang::Parser(lexer, nPass) {

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
		if (isEof()) return false;
		if (isSpecial(";")) return true;
		if (isSpecial("{")) {
			return doCodeBlock(parent);
		}

		if (isSpecial("@")) {
			return doCodeInsertion(parent);
		}

		if (isOperator()) {
			return doOperator(parent);
		}

		if (isStdType()) {
			if (!isIdentifier()) return false;
			if (isSpecial("(")) return doFuncDef(parent);
			return doVarDef(parent);
		}
		
		if (isClass()) {
			if (!isIdentifier()) return false;
			if (isSpecial("(")) return doFuncDef(parent);
			return doVarDef(parent);
		}

		if (isIdentifier()) {
			if (isSpecial("(")) return doFunc(parent);
			if (isSpecial("{")) return doClass(parent);
			if (isOperator("from")) return doClass(parent, 1);
			if (isSpecial(":"))	return doClass(parent, 2);
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

		if (isSpecial("@")) {
			return doCodeInsertion(parent);
		}

		if (isOperator()) {
			return doOperator(parent);
		}

		if (isStdType()) {
			if (!isIdentifier()) return false;
			return doVarDef(parent);
		}

		if (isClass()) {
			if (!isIdentifier()) return false;
			return doVarDef(parent);
		}

		if (isIdentifier()) {
			if (isSpecial("(")) return doFunc(parent);
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
			else {
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
//???			FuncDef *fd = findFuncDef(parent->parent->parent->parent, identifier);
//??? Продумать для функций - методов класса

			FuncDef *fd = findFuncDef(nullptr, identifier);
			if (!fd) fd = findFuncDef(parent->parent->parent->parent, identifier);
			if (!fd) {
				Expression *exp = (Expression*)parent;
				int count = exp->nodes.size();
				if (count >= 2) {
					Node *node = exp->nodes[count - 1];
					if (node->nodeType == ntExpOper) {
						ExpOper *expOper = (ExpOper*)node;
						if (expOper->name == ".") {
							node = exp->nodes[count - 2];
							if (node->nodeType == ntVar) {
								Var *var = (Var*)node;
								fd = findFuncDefIntoVar(var, identifier);
							}
							else if (node->nodeType == ntFunc) {
								int a = 1; //??? Здесь возможна какая-то обработка )))))
							}
						}
					}
				}
			}
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

	bool Parser::doFuncDef(Node *parent, bool isFrom) {
		FuncDef *fd = new FuncDef();
		fd->name = identifier;
		if (parent) {
			if (parent->nodeType == ntClass) {
				Class *clss = (Class*)parent;
				fd->owner = clss;
				if (clss->name == identifier) {
					fd->isConstruct = true;
				}
			}
		}
		if (fd->isConstruct && std_type != "") return false;
		if (fd->isConstruct && clss != nullptr) return false;
		if (!fd->isConstruct) {
			if (clss) {
				fd->clss = clss;
			}
			else {
				if (std_type == "") fd->type = "auto"; else fd->type = std_type;
			}
		}
		addNode(parent, fd);
		if (!doFuncDefParams(fd)) return false;

		if (fd->owner) {
			if (((Class*)fd->owner)->isFrom) {
				return isSpecial(";");
			}
		}

		if (isOperator("from")) {
			fd->isFrom = true;
			if (!isString()) return false;
			fd->file = cur_string;
		}
		else {
			if (!doFuncDefBody(fd)) return false;
		}

		return true;
	}

	bool Parser::doFuncDefParams(FuncDef *fd) {
		if (isSpecial(")")) return true;
		while (true) {
			bool isRef = false;
			if (isStdType()) {
				if (isSpecial("&")) isRef = true;
				if (isIdentifier()) {
					FuncDefParam *param = new FuncDefParam();
					param->isRef = isRef;
					param->type = std_type;
					param->name = identifier;

					fd->params.push_back(param);

					if (isSpecial(")")) return true;
					if (isSpecial(",")) continue;
				}
				return false;
			}
			else {
				if (isSpecial("&")) isRef = true;
				if (isIdentifier()) {
					Class *clss = findClass(nullptr, identifier);
					if (!clss) {
						FuncDefParam *param = new FuncDefParam();
						param->isRef = isRef;
						param->type = "auto";
						param->name = identifier;

						fd->params.push_back(param);

						if (isSpecial(")")) return true;
						if (isSpecial(",")) continue;
					}
					else {
						if (isRef) return false;
						if (isSpecial("&")) isRef = true;
						if (isIdentifier()) {
							FuncDefParam *param = new FuncDefParam();
							param->isRef = isRef;
							param->clss = clss;
							param->name = identifier;

							fd->params.push_back(param);

							if (isSpecial(")")) return true;
							if (isSpecial(",")) continue;
						}
					}
				}
			}
			return false;
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
		if (clss) {
			vd->clss = clss;
		}
		else {
			if (std_type == "") vd->type = "auto"; else vd->type = std_type;
		}
		
		addNode(parent, vd);

		if (isSpecial("[")) {
			if (!isSpecial("]")) return false;
			vd->isArray = true;
		}
		else if (isSpecial("=")) {
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
		if (isOperator("from")) {
			vd->isFrom = true;
			if (!isString()) return false;
			vd->file = cur_string;
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
			if (!vd) vd = findVarDefInParams(parent->parent->parent, identifier);
			if (!vd) {
				Expression *exp = (Expression*)parent;
				int count = exp->nodes.size();
				if (count >= 2) {
					Node *node = exp->nodes[count - 1];
					if (node->nodeType == ntExpOper) {
						ExpOper *expOper = (ExpOper*)node;
						if (expOper->name == ".") {
							node = exp->nodes[count - 2];
							if (node->nodeType == ntVar) {
								Var *var = (Var*)node;
								vd = findVarDefIntoVar(var, identifier);
							}
							else if (node->nodeType == ntFunc) {
								int a = 1; //??? Здесь возможна какая-то обработка )))))
							}
						}
					}
				}
			}
			if (!vd) {
				vd = new VarDef();
				vd->name = identifier;
				vd->type = "auto";
				addNode(parent->parent, vd);
			}

			Var *var = new Var();
			var->def = vd;
			addNode(parent, var);

			//if (vd->clss) {
			//	if (isSpecial(".")) {
			//		if (isIdentifier()) {
			//			int count = vd->clss->nodes.size();
			//			for (int i = 0; i < count; i++) {
			//				Node *node = vd->clss->nodes[i];
			//				if (node->nodeType == ntVarDef) {
			//					VarDef *vd = (VarDef*)node;
			//				}
			//				else if (node->nodeType == ntFuncDef) {
			//					FuncDef *fd = (FuncDef*)node;
			//				}
			//			}
			//			int a = 1;
			//		}
			//	}
			//}

			if (vd->isArray) {
				if (isSpecial("[")) {
					if (!doExpression(parent)) return false;
					if (!isSpecial("]")) return false;
				}
			}
		}
		return true;
	}

	bool Parser::doConstruct(Node *parent) {
		if (!parent) return false; //??? непонятно что с этим делат в будущем)

		Class *clss = findClass(parent, identifier);
		if (!clss) return false;

		Construct *con = new Construct();
		con->clss = clss;
		addNode(parent, con);

		if (isSpecial("}")) return true;
		while (true) {
			if (!isIdentifier()) return false;
			if (!isSpecial(":")) return false;
			//1. Найти в классе clss свойство identifier
			cj::VarDef *vd = (cj::VarDef*)clss->findVar(identifier);

			//2. Определить его тип/класс
			// if (vd->clss) vd->clss
			// else vd->type;

			//3. Создать переменную
			Var *var = new Var();
			var->def = vd;
			addNode(con, var);

			//4. Вызвать функцию обработки инициализации свойства identifier
			bool flag = doVarInit(var, vd);
			int count = var->nodes.size();
			if (!flag) return false;
			if (isSpecial(",")) continue;
			if (isSpecial("}")) return true;
		}
		return false;
	}

	bool Parser::doVarInit(Node *parent, VarDef *vd) {
		//Array of string
		if (vd->type == "string") {
			if (vd->isArray) {
				if (!isSpecial("[")) return false;
				if (isSpecial("]")) return true;
				while (true) {
					if (!doExpression(parent)) return false;
					if (isSpecial(",")) continue;
					return isSpecial("]");
				}
			}
			else {
				return doExpression(parent);
			}
		}
		else if (vd->type == "int") {
			return doExpression(parent);
		}
		return false;
	}

	bool Parser::doNumber(Node *parent) {
		Number *node = new Number();
		node->value = number;
		addNode(parent, node);
		return true;
	}

	bool Parser::doString(Node *parent) {
		StringNode *node = new StringNode();
		node->value = cur_string;
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

	bool Parser::doClass(Node *parent, uint flags) {
		Class *clss = new Class();
		clss->name = identifier;
		addNode(parent, clss);
		if (flags == 1) {
			clss->isFrom = true;
			if (isString()) {
				if (isSpecial("{")) clss->file = cur_string;
				else return false;
			}
			else return false;
		}
		else if (flags == 2) {
			if (isIdentifier()) {
				Class *clss2 = findClass(nullptr, identifier);
				if (!clss2) return false;
				if (isSpecial("{")) clss->file = cur_string;
				else return false;
				clss->parent = clss2;
			}
			else return false;
		}
		while (true) {
			if (isSpecial("}")) return true;

			if (isSpecial("@")) {
				bool flag = doCodeInsertion(clss);
				if (!flag) return false;
			}
			else if (isStdType()) {
				if (!isIdentifier()) return false;
				if (isSpecial("(")) {
					bool flag = doFuncDef(clss);
					if (!flag) return false;
				}
				else {
					bool flag = doVarDef(clss);
					if (!flag) return false;
				}
			}
			else {
				if (!isIdentifier()) return false;
				if (isSpecial("(")) {
					bool flag = doFuncDef(clss);
					if (!flag) return false;
				}
				else {
					Class *clss2 = findClass(nullptr, identifier);
					if (clss2) {
						//если это класс, то тут возможна обработка описания функции или перепенной
					}
					else {
						bool flag = doVarDef(clss);
						if (!flag) return false;
					}
				}
			}
		}
		return false;
	}



	bool Parser::doExpression(Node *node, bool isCreateExpressionNode) {
		ExpType expType = etNone;

		Expression *expression = (Expression*)node;
		if (isCreateExpressionNode) {
			expression = new Expression();
			addNode(node, expression);
		}

		bool flag = doUnaryOperator(expression);
		if (expType == etNone && flag) expType = etInteger;

		while (true) {
			if (isSpecial("(")) {
				if (expType == etString) return false;
				if (expType == etNone) expType = etInteger;
				if (!doExpression(expression, false)) return false;
				if (isSpecial(")")) return false;
			}
			else if (isNumber()) {
				if (expType == etNone) expType = etInteger;
				doNumber(expression);
			}
			else if (isString()) {
				expType = etString;
				doString(expression);
			}
			else if (isIdentifier()) {
				if (isSpecial("(")) {
					if (!doFuncCall(expression)) return false;
				}
				else if (isSpecial("{")) {
					if (!doConstruct(expression)) return false;
				}
				else {
					if (!doVar(expression)) return false;
				}
			}
			else return false;

			if (doBinaryOperator(expression)) continue;
			if (isSpecial(";")) expression->isTZ = true;
			break;
		}
		return true;
	}

	bool Parser::doUnaryOperator(Node *parent) {
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

	bool Parser::doBinaryOperator(Node *parent) {
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

	bool Parser::doCodeInsertion(Node *parent) {
		if (!isIdentifier()) return false;

		CodeInsertion *ci = new CodeInsertion();
		if (identifier == "cpp") ci->cit = ciCpp;
		else if (identifier == "cpph") ci->cit = ciCpph;
		else if (identifier == "js") ci->cit = ciJs;
		else return false;

		addNode(parent, ci);

		if (!isCodeInsertion()) return false;
		ci->source = source;
		return true;
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

	bool Parser::isString() {
		savePosition();
		token = getToken();
		if (token.type == ltString) {
			cur_string = token.lexeme;
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

	bool Parser::isClass() {
		clss = nullptr;
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			clss = findClass(nullptr, token.lexeme);
			if (clss) return true;
		}
		rollback();
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

	bool Parser::isCodeInsertion() {
		savePosition();
		token = getToken();
		if (token.type == ltCodeInsertion) {
			source = token.lexeme;
			return true;
		}

		rollback();
		return false;
	}


	bool Parser::isEof() {
		savePosition();
		token = getToken();
		if (token.type == ltEof) return true;
		
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

	VarDef* Parser::findVarDefInParams(Node *parent, Str var) {
		if (parent->nodeType != ntFuncDef) return nullptr;
		FuncDef *fd = (FuncDef*)parent;
		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = fd->params[i];
			if (fdp->name == var) return fdp;
		}

		return nullptr;
	}

	VarDef* Parser::findVarDefIntoClass(Node *parent, Str var) {
		int count = parent->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *node = parent->nodes[i];
			if (node->nodeType == ntVarDef) {
				VarDef *vd = (VarDef*)node;
				if (vd->name == var) return vd;
			}
		}
		return nullptr;
	}

	VarDef* Parser::findVarDefIntoVar(Var *var, Str vn) {
		return findVarDefIntoClass(var->def->clss, vn);
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
		if (parent->nodeType == ntClass) {
			Class *clss = (Class*)parent;
			while (clss) {
				FuncDef *fd = findFuncDefIntoClass(clss, func);
				if (fd) return fd;
				clss = (Class*)(clss->parent);
			}
		}
		return nullptr;
	}

	FuncDef* Parser::findFuncDefIntoClass(Node *parent, Str func) {
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

	FuncDef* Parser::findFuncDefIntoVar(Var *var, Str func) {
		return findFuncDefIntoClass(var->def->clss, func);
	}

	Class* Parser::findClass(Node *parent, Str cls_nm) {
		if (parent == nullptr) {
			int count = nodes.size();
			for (int i = 0; i < count; i++) {
				Node *node = nodes[i];
				if (node->nodeType == ntClass) {
					Class *clss = (Class*)node;
					if (clss->name == cls_nm) return clss;
				}
			}
			return nullptr;
		}
		int count = parent->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *node = parent->nodes[i];
			if (node->nodeType == ntClass) {
				Class *clss = (Class*)node;
				if (clss->name == cls_nm) return clss;
			}
		}
		return findClass(parent->parent, cls_nm);
	}

}
