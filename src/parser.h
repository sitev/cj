#pragma once

using namespace lang;

namespace cj {

	enum ExpType {etNone, etInteger, etString};

	class VarDef : public lang::VarDef {
	public:
		bool isFrom = false;
		Str file;
	};

	class FuncDefParam : public VarDef {
	};

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

	enum CodeInsertionType {ciNone, ciCpp, ciCpph, ciJs, ciLua};

	class CodeInsertion : public lang::CodeInsertion {
	public:
		CodeInsertionType cit = ciNone;
		Str source;
		CodeInsertion();
	};


	class Parser : public lang::Parser {
	protected:
		virtual void clear();
	public:
		Parser(lang::Lexer *lexer, int nPass = 1);

		virtual bool doMainCodeBlock();
		//virtual bool doCodeBlockPassMain(Node *parent);
		virtual bool doCodeBlock(Node *parent);
		virtual bool doMainStatement(Node *parent);
		virtual bool doStatement(Node *parent);

		virtual bool doFunc(Node *parent);
		virtual bool doFuncCall(Node *parent);
		virtual bool doFuncDefPassMain(Node *parent, bool isFrom = true);
		virtual bool doFuncDef(Node *parent, bool isFrom = true);
		virtual bool doFuncDefParams(FuncDef *fd);
		virtual bool doFuncDefBody(Node *parent);

		virtual bool doVarDefPassSingle(Node *parent);
		virtual bool doVarDefPassLight(Node *parent);
		virtual bool doVarDefPassMain(Node *parent);
		virtual bool doVarDef(Node *parent);

		virtual bool doVar(Node *parent);
		virtual bool doVarInit(Node *parent, VarDef *vd);

		virtual bool doUnknown(Node *parent);

		virtual bool doNumber(Node *parent);
		virtual bool doString(Node *parent);

		virtual bool doOperator(Node *parent);
		virtual bool doOperatorIf(Node *parent);
		virtual bool doOperatorFor(Node *parent);
		virtual bool doOperatorWhile(Node *parent);
		virtual bool doOperatorReturn(Node *parent);
		
		virtual bool doClassPassSingle(Node *parent, uint flags);
		virtual bool doClassPassLight(Node *parent, uint flags);
		virtual bool doClassPassMain(Node *parent, uint flags);
		virtual bool doClass(Node *parent, uint flags = 0);

		virtual bool doConstruct(Node *parent);



		virtual bool doExpression(Node *node, bool isCreateExpressionNode = true);
		virtual bool doUnaryOperator(Node *parent);
		virtual bool doBinaryOperator(Node *parent);

		virtual bool doCodeInsertion(Node *parent);

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

		virtual bool isCodeInsertion();
		virtual bool isEof();
		
	protected:
		Str std_type, oper, identifier, cur_string, source;
		Class *clss = nullptr;
		int number;
		lang::Token token;
		virtual Node* addOrTestNode(Node *parent, Node *node);
		virtual void testNode(Node *parent, Node *node);
		virtual VarDef* findVarDef(Node *parent, Str var);
		virtual VarDef* findVarDefInParams(Node *parent, Str var);
		virtual VarDef* findVarDefIntoClass(Node *parent, Str var);
		virtual VarDef* findVarDefIntoVar(Var *var, Str vn);

		virtual FuncDef* findFuncDef(Node *parent, Str func);
		virtual FuncDef* findFuncDefIntoClass(Node *parent, Str func);
		virtual FuncDef* findFuncDefIntoVar(Var *var, Str func);
		virtual Class* findClass(Node *parent, Str clss_nm);
	};

}
