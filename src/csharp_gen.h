#pragma once

namespace cj {

	class CSharpGen : public Generator {
	public:
		CSharpGen(Parser *parser, Str fn);
		~CSharpGen();
		virtual void setIncludes(vector<Str> &includes);
		virtual void setNamespace(Str namespce);
	protected:
		//Str sCpp;
		vector<Str> includes;
		Str namespce;
		vector<Str> froms;
		bool wasReturn = false;

		virtual Str getHeader();
		virtual Str getFooter();
		virtual Str genRemmark(Node *node);
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
		virtual Str genType(Str type);
	};

}