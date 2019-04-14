#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj { 

	JsGen::JsGen(Parser *parser, Str fn) : Generator(parser, fn) {
	}

	JsGen::~JsGen() {
	}

	Str JsGen::genRemmark(Node *node) {
		Remmark *rem = (Remmark*)node;
		if (rem->isMultiLine) return (Str)"/*" + rem->value + "*/\n";
		else return (Str)"//" + rem->value + "\n";
	}

	Str JsGen::genNumber(Node *node) {
		Number *num = (Number*)node;
		Str s = to_string(num->value);
		return s;
	}

	Str JsGen::genVarDef(Node *node) {
		Str s = "var ";
		VarDef *vd = (VarDef*)node;
		s += vd->name;
		if (vd->isArray) s += " = []";
		s += ";\r\n";
		return s;
	}

	Str JsGen::genVar(Node *node) {
		Var *var = (Var*)node;
		Str s = var->def->name;

		if (var->def->isArray) {
			s += "[";
			int count = var->nodes.size();
			for (int i = 0; i < count; i++) {
				Node *nd = var->nodes[i];
				s += generate(nd);
			}
			s += "]";
		}
		return s;
	}

	Str JsGen::genFuncDef(Node *node) {
		Str s = "function ";
		FuncDef *fd = (FuncDef*)node;
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

	Str JsGen::genFunc(Node *node) {
		Func *func = (Func*)node;
		Str s = func->def->name + "()";

		return s;
	}

	Str JsGen::genOperator(Node *node) {
		Operator *oper = (Operator*)node;
		if (oper->name == "if") return genOperatorIf(oper);
		else if (oper->name == "for") return genOperatorFor(oper);
		else if (oper->name == "while") return genOperatorWhile(oper);
		else if (oper->name == "return") return genOperatorReturn(oper);
		return "";
	}

	Str JsGen::genOperatorIf(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]);
		s += ") ";
		if (count > 1) s += generate(oper->nodes[1]); else s += ";\r\n";
		if (count > 2) s += (Str)"else " + generate(oper->nodes[2]);
		return s;
	}

	Str JsGen::genOperatorFor(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0], true);
		if (count > 1) s += generate(oper->nodes[1], true);
		if (count > 2) s += generate(oper->nodes[2]);
		s += ") ";
		if (count > 3) s += generate(oper->nodes[3]); else s += ";\r\n";
		return s;
	}

	Str JsGen::genOperatorWhile(Operator *oper) {
		Str s = oper->name + " (";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]);
		s += ") ";
		if (count > 1) s += generate(oper->nodes[1]); else s += ";\r\n";
		return s;
	}

	Str JsGen::genOperatorReturn(Operator *oper) {
		Str s = oper->name + " ";
		int count = oper->nodes.size();
		if (count > 0) s += generate(oper->nodes[0]); else s += ";\r\n";
		return s;
	}

	Str JsGen::genExpOper(Node *node) {
		Operator *oper = (Operator*)node;
		Str s = (Str)" " + oper->name + " ";
		return s;
	}

	Str JsGen::genExpression(Node *node, bool isExpNotCR) {
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

	Str JsGen::genCodeBlock(Node *node) {
		Str s = "{\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 != "") s += getTab(1, 1) + s2;
		}

		s += "}\r\n";
		return s;
	}

	Str JsGen::genClass(Node *node) {
		Class *clss = (Class*)node;
		Str s = "var ";
		s += clss->name + " = /** @class */ (function () {\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			s = s + s2;
		}

		s += "}());\r\n";
		return s;
	}
}