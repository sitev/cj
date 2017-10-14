#pragma once

namespace cj {

	class Parser : public lang::Parser {
	protected:
		Token token;
	public:
		Parser(lang::Lexer *lexer);

		virtual bool doMainCodeBlock();
		virtual bool doCodeBlock();
		virtual bool doMainStatement();
		virtual bool doStatement();

		virtual bool doFunc();
		virtual bool doFuncCall();
		virtual bool doFuncDef();
		virtual bool doFuncDefParams();
		virtual bool doFuncDefBody();

		virtual bool doVar();
		virtual bool doVarDef();
		virtual bool doVarUse();

		virtual bool doExpression();
		virtual bool doUnatyOperator();
		virtual bool doBinatyOperator();

		virtual bool isIdentifier();
		virtual bool isStdType();
		virtual bool isKeyword();
		virtual bool isNumber();
		virtual bool isSpecial(Str s);
		virtual bool find(Str s);
		
	};

}
