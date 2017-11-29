#pragma once

using namespace lang;

namespace cj {

	enum ExpType {etNone, etInteger, etString};

	class FuncDef : public lang::FuncDef {
	public:
		bool isFrom = false;
		Str file;
		vector<FuncDefParam*> params;
		FuncDef();
	};

	class Class : public lang::Class {
	public:
		bool isFrom = false;
		Str file;
		Class();
	};


	class Parser : public lang::Parser {
	public:
		Parser(Lexer *lexer);

		virtual bool doMainCodeBlock();
		virtual bool doCodeBlock(Node *parent);
		virtual bool doMainStatement(Node *parent);
		virtual bool doStatement(Node *parent);

		virtual bool doFunc(Node *parent);
		virtual bool doFuncCall(Node *parent);
		virtual bool doFuncDef(Node *parent, bool isFrom = true);
		virtual bool doFuncDefParams(FuncDef *fd);
		virtual bool doFuncDefBody(Node *parent);

		virtual bool doVarDef(Node *parent);
		virtual bool doVar(Node *parent);
		virtual bool doVarInit(Node *parent, VarDef *vd);

		virtual bool doNumber(Node *parent);
		virtual bool doString(Node *parent);

		virtual bool doOperator(Node *parent);
		virtual bool doOperatorIf(Node *parent);
		virtual bool doOperatorFor(Node *parent);
		virtual bool doOperatorWhile(Node *parent);
		virtual bool doOperatorReturn(Node *parent);
		
		virtual bool doClass(Node *parent, uint flags = 0);
		virtual bool doConstruct(Node *parent);



		virtual bool doExpression(Node *node, bool isCreateExpressionNode = true);
		virtual bool doUnaryOperator(Node *parent);
		virtual bool doBinaryOperator(Node *parent);

		virtual bool isIdentifier();
		virtual bool isString();
		virtual bool isStdType();
		virtual bool isOperator();
		virtual bool isOperator(Str s);
		virtual bool isClass();

		virtual bool isNumber();
		virtual bool isLexeme(Str s);
		virtual bool isSpecial(Str s);
		virtual bool find(Str s);

		virtual bool isEof();
		
	protected:
		Str std_type, oper, identifier, cur_string;
		Class *clss = nullptr;
		int number;
		lang::Token token;
		virtual void addNode(Node *parent, Node *node);
		virtual VarDef* findVarDef(Node *parent, Str var);
		virtual VarDef* findVarDefInParams(Node *parent, Str var);
		virtual FuncDef* findFuncDef(Node *parent, Str func);
		virtual FuncDef* findFuncDefIntoClass(Node *parent, Str func);
		virtual Class* findClass(Node *parent, Str clss_nm);
	};

}
