
#include "lang.h"
using namespace lang;

#include "cj.h"

namespace cj {
	AsGen::AsGen(Parser *parser, Str fn) : Generator(parser, fn) {
		ext.push_back("as");
	}

	AsGen::~AsGen() {

	}

	Str AsGen::genNumber(Node *node) { 
		Number *num = (Number*)node;
		Str s = to_string(num->value);
		return s;
	}

	Str AsGen::genString(Node *node) {
		StringNode *sn = (StringNode*)node;
		Str s = sn->value;
		return s;
	}

	Str AsGen::genVarDef(Node *node) { 
		VarDef *vd = (VarDef*)node;
		Str s;
		if (vd->isArray) {
			s = "array<";
			if (vd->clss) s += vd->clss->name + "*> " + getVarName(vd->name);
			else s += vd->type + "> ";
		}
		else {
			if (vd->clss) s += vd->clss->name + " *";
			else s = vd->type + " ";
		}

		int size = node->nodes.size();
		if (size > 0) {
			Node *nd = node->nodes[0];
			if (nd->nodeType == ntExpression) {
				s += genExpression(nd);
				return s;
			}
		}

		s += getVarName(vd->name) + ";\r\n";
		return s;
	}

	Str AsGen::genVar(Node *node) {
		//Поиск объекта для переменной (если она вляется полем)
		Str obj = findVarNameIfField(node);
		if (obj != "") obj += ".";

		Var *var = (Var*)node;
		Str s = obj + getVarName(var->def->name);

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
	Str AsGen::genFuncDef(Node *node) {
		FuncDef *fd = (FuncDef*)node;

		Str s = "";

		if (fd->isFrom) {
			s += "#include ";
			s += fd->file.substr(0, fd->file.length() - 1) + ".h\"\n\n";

			return s;
		}

		if (!fd->isConstruct) {
			if (fd->clss) {
				s += fd->clss->name + "* ";
			}
			else {
				s += fd->type + " ";
			}
		}
		s += fd->name + "(";

		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
			if (fdp->clss) s += fdp->clss->name + " *";
			else s += fdp->type + " ";
			if (fdp->isRef) s += "&";
			s += fdp->name;

			if (i + 1 != count) {
				s += ", ";
			}
		}

		s += ") ";
		//s += ";\n";

		count = fd->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = fd->nodes[i];
			s += generate(nd);
		}

		return s;
	}
	Str AsGen::genFunc(Node *node) {
		Func *func = (Func*)node;
		Str s = func->def->name + "(";

		int count = func->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = func->nodes[i];
			s += generate(nd);
		}

		s += ")";

		return s;
	}

	Str AsGen::genOperator(Node *node) { return ""; }
	Str AsGen::genOperatorIf(Operator *oper) { return ""; }
	Str AsGen::genOperatorFor(Operator *oper) { return ""; }
	Str AsGen::genOperatorWhile(Operator *oper) { return ""; }
	Str AsGen::genOperatorReturn(Operator *oper) { return ""; }
	Str AsGen::genExpOper(Node *node) {
		Operator *oper = (Operator*)node;
		if (oper->name == ".") return "->";
		Str s = (Str)" " + oper->name + " ";
		return s;
	}
	Str AsGen::genExpression(Node *node, bool isExpNotCR) { 
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
	Str AsGen::genCodeBlock(Node *node) { 
		Str s = "{\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 != "") s += getTab(1, 3) + s2;
		}

		s += "}\n\n";
		return s;
	}

	Str AsGen::genClass(Node *node) {
		Class *clss = (Class*)node;
		Str s;
		s = "class ";
		s += clss->name;

		if (clss->parent != nullptr) {
			s += (Str)" : " + clss->parent->name;
		}

		s += " {\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 == "") continue;
			s = s + getTab(1, 4) + s2;
		}

		s += "}\r\n";
		return s;
	}

	Str AsGen::genConstruct(Node *node) {
		Construct *con = (Construct*)node;
		int count = node->nodes.size();
		if (count == 0) {
			Str s = con->clss->name + "();\r\n";
			return s;
		}
		Str s = "new";
		s += con->clss->name + "();\r\n";
		Str var_name = findVarNameIfField(node, true);
		s += con->clss->name + " new" + con->clss->name + "() {\n\t" + "auto " + var_name + " = " + con->clss->name + "();\n";

		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			s = s + getTab(1, 5) + s2;
		}

		s += (Str)"\treturn " + var_name + ";\n}\n";

		return s;
	}

	bool AsGen::isReservedWord(Str s) {
		if (s == "namespace") return true;
		return false;
	}

	Str AsGen::getVarName(Str s) {
		if (isReservedWord(s)) return (Str)"m_" + s;
		return s;
	}
}