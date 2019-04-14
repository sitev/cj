#include "lang.h"
using namespace lang;

#include "cj.h"

namespace cj {
	LuaGen::LuaGen(Parser *parser, Str fn) : Generator(parser, fn) {
		ext.push_back("lua");
	}

	LuaGen::~LuaGen() {

	}

	Str LuaGen::genNumber(Node *node) {
		Number *num = (Number*)node;
		Str s = to_string(num->value);
		return s;
	}

	Str LuaGen::genString(Node *node) {
		StringNode *sn = (StringNode*)node;
		Str s = sn->value;
		return s;
	}

	Str LuaGen::genVarDef(Node *node) {
		Str s = "";
		VarDef *vd = (VarDef*)node;
		Str tmp = vd->name;
		if (vd->clss) s = "obj.";

		int size = node->nodes.size();
		if (size > 0) {
			Node *nd = node->nodes[0];
			if (nd->nodeType == ntExpression) {
				Str tmp = genExpression(nd);
				s += tmp;
				return s;
			}
		}

		s += getVarName(vd->name) + " = 0;\n";
		return s;
	}

	Str LuaGen::genVar(Node *node) {
		//Поиск объекта для переменной (если она является полем)
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
	Str LuaGen::genFuncDef(Node *node) {
		FuncDef *fd = (FuncDef*)node;
		Str s = "\nfunction ";

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
				//s += fd->type + " ";
			}
		}
		s += fd->name + "(";

		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
			if (fdp->clss) s += fdp->clss->name + " *";
			else {
				if (fdp->type != "auto") s += fdp->type + " ";
			}
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
	Str LuaGen::genFunc(Node *node) {
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

	Str LuaGen::genOperator(Node *node) { return ""; }
	Str LuaGen::genOperatorIf(Operator *oper) { return ""; }
	Str LuaGen::genOperatorFor(Operator *oper) { return ""; }
	Str LuaGen::genOperatorWhile(Operator *oper) { return ""; }
	Str LuaGen::genOperatorReturn(Operator *oper) { return ""; }
	Str LuaGen::genExpOper(Node *node) {
		Operator *oper = (Operator*)node;
		if (oper->name == ".") return "->";
		Str s = (Str)" " + oper->name + " ";
		return s;
	}
	Str LuaGen::genExpression(Node *node, bool isExpNotCR) {
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
	Str LuaGen::genCodeBlock(Node *node) {
		Str s = "\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 != "") s += getTab(1, 3) + s2;
		}

		s += "end\n\n";
		return s;
	}

	Str LuaGen::genClass(Node *node) {
		Class *clss = (Class*)node;
		Str s = "";
		s += (Str)"\n" + clss->name + " = {}\n";

		if (clss->parent != nullptr) {
			s += (Str)"setmetatable(" + clss->name + ", { __index = " + clss->parent->name + " })\n";
		}

		s += (Str)"function " + clss->name + ":new()\n";
		s += "	local obj = {}\n";
		s += "\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 == "") continue;
			s = s + getTab(1, 4) + s2;
		}

		s += "\n";
		s += "	setmetatable(obj, self);\n";
		s += "	self.__index = self;\n";
		s += "	return obj\n";

		s += "end;\n\n";
		return s;
	}

	Str LuaGen::genConstruct(Node *node) {
		Construct *con = (Construct*)node;

		int count = con->clss->nodes.size();
		if (count == 0) {
			Str s = con->clss->name + ":new()";
			return s;
		}
		
		Str s = con->clss->name + ":new();\n";
		Str var_name = findVarNameIfField(node, true);

		count = con->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = con->nodes[i];
			s += generate(nd);
		}
		return s;
	}

	Str LuaGen::genCodeInsertion(Node *node) {
		CodeInsertion *ci = (CodeInsertion*)node;
		Str s;
		if (ci->cit == ciLua) {
			s = "\n-- { code insertion\n";
			s += ci->source;
			s += "\n-- } code insertion\n\n";
		}
		return s;
	}


	bool LuaGen::isReservedWord(Str s) {
		if (s == "namespace") return true;
		return false;
	}

	Str LuaGen::getVarName(Str s) {
		if (isReservedWord(s)) return (Str)"m_" + s;
		return s;
	}
}