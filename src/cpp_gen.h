#pragma once

namespace cj {

	class CppGen : public Generator {
	public:
		CppGen(Parser *parser, Str fn);
		~CppGen();
		virtual void setIncludes(vector<Str> &includes);
	protected:
		Str sCpp;
		vector<Str> includes;

		virtual Str getHeader();
		virtual Str genNumber(Node *node);
		virtual Str genString(Node *node);
		virtual Str genVarDef(Node *node);
		virtual Str genVar(Node *node);
		virtual Str genFuncDef(Node *node);
		virtual Str genFunc(Node *node);
		virtual Str genOperator(Node *node);
		virtual Str genOperatorIf(Operator *oper);
		virtual Str genOperatorFor(Operator *oper);
		virtual Str genOperatorWhile(Operator *oper);
		virtual Str genOperatorReturn(Operator *oper);
		virtual Str genExpOper(Node *node);
		virtual Str genExpression(Node *node, bool isExpNotCR = false);
		virtual Str genCodeBlock(Node *node);
		virtual Str genClass(Node *node);
		virtual Str genConstruct(Node *node);
		virtual Str genCodeInsertion(Node *node);
	};

}