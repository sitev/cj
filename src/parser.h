#pragma once

using namespace lang;

namespace cj {

	class Parser : public lang::Parser {
	public:
		Parser(Lexer *lexer);

		virtual bool doMainCodeBlock();
		virtual bool doCodeBlock(Node *parent);
		virtual bool doMainStatement(Node *parent);
		virtual bool doStatement(Node *parent);

		virtual bool doFunc(Node *parent);
		virtual bool doFuncCall(Node *parent);
		virtual bool doFuncDef(Node *parent);
		virtual bool doFuncDefParams(FuncDef *fd);
		virtual bool doFuncDefBody(Node *parent);

//		virtual bool doVar(Node *parent);
		virtual bool doVarDef(Node *parent);
		virtual bool doVar(Node *parent);

		virtual bool doNumber(Node *parent);
		virtual bool doOperator(Node *parent);
		virtual bool doOperatorIf(Node *parent);
		virtual bool doOperatorFor(Node *parent);
		virtual bool doOperatorWhile(Node *parent);
		virtual bool doOperatorReturn(Node *parent);
		
		virtual bool doClass(Node *parent);



		virtual bool doExpression(Node *node, bool isCreateExpressionNode = true);
		virtual bool doUnatyOperator(Node *parent);
		virtual bool doBinatyOperator(Node *parent);

		virtual bool isIdentifier();
		virtual bool isStdType();
		virtual bool isOperator();
		virtual bool isOperator(Str s);
		//virtual bool isKeyword();
		virtual bool isNumber();
		virtual bool isLexeme(Str s);
		virtual bool isSpecial(Str s);
		virtual bool find(Str s);

		/*virtual bool isEof();*/
		
	protected:
		Str std_type, oper, identifier;
		int number;
		lang::Token token;
		virtual void addNode(Node *parent, Node *node);
		virtual VarDef* findVarDef(Node *parent, Str var);
		virtual FuncDef* findFuncDef(Node *parent, Str func);
	};

}
