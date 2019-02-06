#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj {

	CppGen::CppGen(Parser *parser, Str fn) : Generator(parser, fn) {
		lstTarget.push_back(&sCpp);
		ext.push_back("h");
		ext.push_back("cpp");

		int pos = fn.rfind("\\");
		Str smallfn = fn.substr(pos + 1);
		sCpp = "";
	}

	CppGen::~CppGen() {
	}

	void CppGen::setIncludes(vector<Str> &includes) {
		this->includes = includes;
	}

	void CppGen::setNamespace(Str namespce) {
		this->namespce = namespce;
		sCpp = "#include \"";
		if (namespce != "") sCpp += namespce + ".h\"\n\n";
		else {
			Str s = fn;
			int pos = s.rfind("\\");
			if (pos < 0) pos = s.rfind("/");
			if (pos > 0) s = s.substr(pos + 1);

			sCpp += s + ".h\"\n\n";
		}
	}

	Str CppGen::getHeader() {
		Str s = "#pragma once\n\n";

		int count = includes.size();
		for (int i = 0; i < count; i++) {
			s += "#include ";
			s += includes[i] + "\n";
		}
		if (count != 0) s += "\n";

		count = parser->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *node = parser->nodes[i];

			if (node->nodeType == ntClass) {
				Class *clss = (Class*)node;
				s += "class " + clss->name.to_string() + ";\n";
			}
		}
		s += "\n";


		if (namespce != "") {
			s += (Str)"namespace " + namespce + " {\n";
			sCpp += (Str)"namespace " + namespce + " {\n";
		}
		return s;
	}

	Str CppGen::getFooter() {
		Str s;
		if (namespce != "") {
			sCpp += "\n}\n";
			s = "\n}\n";
		}
		return s;
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
			if (vd->clss) s += vd->clss->name + "*> " + vd->name;
			else s += genType(vd->type) + "> " + vd->name;
		}
		else {
			if (vd->clss) s += vd->clss->name + " *";
			else s = genType(vd->type) + " ";
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

		Str s = "";

		if (fd->isFrom) {
			s += "#include ";
			s += fd->file.substr(0, fd->file.length() - 1) + ".h\"\n\n";

			return s;
		}

		bool outInHeader = true;
		if (!fd->owner && fd->name == "main") outInHeader = false;

		if (!fd->isConstruct) {
			if (fd->clss) {
				s += fd->clss->name + "* ";
				sCpp += fd->clss->name + "* ";
			}
			else {
				if (outInHeader) s += genType(fd->type) + " ";
				sCpp += genType(fd->type) + " ";
			}
		}
		if (fd->owner) sCpp += fd->owner->name + "::";
		if (outInHeader) s += fd->name + "(";
		sCpp += fd->name + "(";

		int count = fd->params.size();
		for (int i = 0; i < count; i++) {
			FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
			if (fdp->clss) if (outInHeader) s += fdp->clss->name + " *";
			else if (outInHeader) s += genType(fdp->type) + " ";
			if (fdp->isRef) if (outInHeader) s += "&";
			if (outInHeader) s += fdp->name;

			if (fdp->clss) sCpp += fdp->clss->name + " *";
			else sCpp += genType(fdp->type) + " ";
			if (fdp->isRef) sCpp += "&";
			sCpp += fdp->name;
			if (i + 1 != count) {
				if (outInHeader) s += ", ";
				sCpp += ", ";
			}
		}

		if (outInHeader) s += ")";
		sCpp += ") ";
		if (outInHeader) s += ";\n";

		if (fd->isConstruct) {
			if (fd->owner->parent) {
				sCpp += (Str)": " + fd->owner->parent->name + "(";
				int count = fd->params.size();
				for (int i = 0; i < count; i++) {
					FuncDefParam *fdp = (FuncDefParam*)fd->params[i];
					sCpp += fdp->name;
					if (i + 1 != count) sCpp += ", ";
				}
				sCpp += ") ";
			}
		}


		count = fd->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = fd->nodes[i];
			generate(nd);
		}


		return s;
	}

	Str CppGen::genFunc(Node *node) {
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
		if (count > 2) s += getTab(1, 2) + (Str)"else " + generate(oper->nodes[2]);
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
		if (oper->name == ".") return "->";
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
		sCpp += "{\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 != "") sCpp += getTab(1, 3) + s2;
		}

		sCpp += "}\n\n";
		return "";
	}

	Str CppGen::genClass(Node *node) {
		Class *clss = (Class*)node;
		Str s;
		if (clss->isFrom) {
			bool flag = false;
			int count = froms.size();
			for (int i = 0; i < count; i++) {
				if (clss->file == froms[i]) flag = true;
			}
			if (!flag) {
				s = (Str)"#include " + clss->file.substr(0, clss->file.length() - 1) + ".h\"\n\n";
				froms.push_back(clss->file);
			}

			return s;
		}
		s = "class ";
		s += clss->name;
		
		if (clss->parent != nullptr) {
			s += (Str)" : public " + clss->parent->name;
		}

		s += " {\r\npublic:\r\n";

		int count = node->nodes.size();
		for (int i = 0; i < count; i++) {
			Node *nd = node->nodes[i];
			Str s2 = generate(nd);
			if (s2 == "") continue;
			s = s + getTab(1, 4) + s2;
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
			s = s + getTab(1, 5) + s2;
		}

//		s += "};\r\n";
		return s;
	}

	Str CppGen::genCodeInsertion(Node *node) {
		CodeInsertion *ci = (CodeInsertion*)node;
		Str s;
		if (ci->cit == ciCpph) {
			s = "\n// { code insertion\n";
			s += ci->source;
			s += "\n// } code insertion\n\n";
		}
		else if (ci->cit == ciCpp) {
			sCpp += "\n// { code insertion\n";
			sCpp += ci->source;
			sCpp += "\n// } code insertion\n\n";
		}
		return s;
	}

	Str CppGen::genType(Str type) {
		if (type == "string") return "String";
		return type;
	}
}
