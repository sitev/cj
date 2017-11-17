#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj {

	CppGen::CppGen(Parser *parser) : Generator(parser) {
	}

	CppGen::~CppGen() {
	}

	Str CppGen::genNumber(Node *node) {
		Number *num = (Number*)node;
		Str s = to_string(num->value);
		return s;
	}

	Str CppGen::genString(Node *node) {
		StringNode *sn = (StringNode*)node;
		Str s = sn->value;
		return s;
	}

	Str CppGen::genVarDef(Node *node) {
		VarDef *vd = (VarDef*)node;
		Str s;
		if (vd->isArray) {
			s = "vector<";
			s += vd->type + "> " + vd->name;
		}
		else {
			s = vd->type + " ";
			s += vd->name;
		}
		s += ";\r\n";
		return s;
	}

	Str CppGen::genVar(Node *node) {
		//Поиск объекта для переменной (если она вляется полем)
		Str obj = "";
		if (node->parent) {
			if (node->parent->nodeType == ntConstruct) {
				if (node->parent->parent->nodeType == ntExpression) {
					int count = node->parent->parent->nodes.size();
					if (count > 0) {
						Node *nd = node->parent->parent->nodes[0];
						if (nd->nodeType == ntVar) {
							Var *v = (Var*)nd;
							obj = v->def->name + "->";
						}
					}
				}
			}
		}
		
		Var *var = (Var*)node;
		Str s = obj + var->def->name;

		int count = var->nodes.size();
		if (var->def->isArray) {
			s += " = {";
			for (int i = 0; i < count; i++) {
				Node *nd = var->nodes[i];
				s += generate(nd);
				if (i + 1 != count) s += ", ";
			}
			s += "};\r\n";
		}
		else {
			for (int i = 0; i < count; i++) {
				Node *nd = var->nodes[i];
				s += " = ";
				s += generate(nd) + ";\r\n";
			}
		}
		return s;
	}

	Str CppGen::genFuncDef(Node *node) {
		FuncDef *fd = (FuncDef*)node;
		Str s = fd->type + " ";
		s += fd->name + "(";

		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
			s += fdp->name;
			if (i + 1 != count) s += ", ";
		}

		s += ") ";

		count = fd->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = fd->nodes[i];
			s += generate(nd);
		}

		return s;
	}

	Str CppGen::genFunc(Node *node) {
		Func *func = (Func*)node;
		Str s = func->def->name + "()";

		return s;
	}

	Str CppGen::genOperator(Node *node) {
		Operator *oper = (Operator*)node;
		if (oper->name == "if") return genOperatorIf(oper);
		else if (oper->name == "for") return genOperatorFor(oper);
		else if (oper->name == "while") return genOperatorWhile(oper);
		else if (oper->name == "return") return genOperatorReturn(oper);
		return "";
	}

	Str CppGen::genOperatorIf(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]);
		s += ") ";
		if (count > 1) s += generate(oper->nodes[1]); else s += ";\r\n";
		if (count > 2) s += (Str)"else " + generate(oper->nodes[2]);
		return s;
	}

	Str CppGen::genOperatorFor(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0], true);
		if (count > 1) s += generate(oper->nodes[1], true);
		if (count > 2) s += generate(oper->nodes[2]);
		s += ") ";
		if (count > 3) s += generate(oper->nodes[3]); else s += ";\r\n";
		return s;
	}

	Str CppGen::genOperatorWhile(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]);
		s += ") ";
		if (count > 1) s += generate(oper->nodes[1]); else s += ";\r\n";
		return s;
	}

	Str CppGen::genOperatorReturn(Operator *oper) {
		Str s = oper->name + " ";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]); else s += ";\r\n";
		return s;
	}

	Str CppGen::genExpOper(Node *node) {
		Operator *oper = (Operator*)node;
		Str s = (Str)" " + oper->name + " ";
		return s;
	}

	Str CppGen::genExpression(Node *node, bool isExpNotCR) {
		Expression *exp = (Expression*)node;

		Str s = "";
		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			s += generate(nd);
		}

		if (exp->isTZ) {
			s += ";";
			if (!isExpNotCR) s += "\r\n";
		}

		return s;
	}

	Str CppGen::genCodeBlock(Node *node) {
		Str s = "{\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 != "") s += getTab(1) + s2;
		}

		s += "}\r\n";
		return s;
	}

	Str CppGen::genClass(Node *node) {
		Class *clss = (Class*)node;
		Str s = "class ";
		s += clss->name + " {\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			s = s + getTab(1) + s2;
		}

		s += "};\r\n";
		return s;
	}

	Str CppGen::genConstruct(Node *node) {
		Construct *con = (Construct*)node;
		Str s = "new ";
		s += con->clss->name + "();\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			s = s + getTab(1) + s2;
		}

//		s += "};\r\n";
		return s;
	}

}