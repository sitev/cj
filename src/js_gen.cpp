#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj { 

	JsGen::JsGen(Parser *parser) : Generator(parser) {
	}

	JsGen::~JsGen() {
	}

	Str JsGen::genNumber(Node *node) {
		Number *num = (Number*)node;
		Str s = to_string(num->value);
		return s;
	}

	Str JsGen::genVarDef(Node *node) {
		Str s = "var ";
		VarDef *vd = (VarDef*)node;
		s += vd->name + ";\r\n";
		return s;
	}

	Str JsGen::genVar(Node *node) {
		Var *var = (Var*)node;
		Str s = var->def->name;
		return s;
	}

	Str JsGen::genFuncDef(Node *node) {
		Str s = "function ";
		FuncDef *fd = (FuncDef*)node;
		s += fd->name + "(";

		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
			s += "var " + fdp->name;
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
		Str s = (Str)" " + oper->name + " ";
		return s;
	}

	Str JsGen::genExpression(Node *node) {
		Expression *exp = (Expression*)node;

		Str s = "";
		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			s += generate(nd);
		}

		if (count != 0) s += ";\r\n";
		return s;
	}

	Str JsGen::genCodeBlock(Node *node) {
		Str s = "{\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			s += getTab(1) + generate(nd);
		}

		s += "}\r\n";
		return s;
	}
}