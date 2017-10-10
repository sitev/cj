#pragma once

namespace cj {

	class Parser : public lang::Parser {
	protected:
		Token token;
	public:
		Parser(lang::Lexer *lexer);
		virtual bool step();
		virtual bool doCodeBlock();
		virtual bool doStatement();
		virtual bool doVar();
		virtual bool doFunc();

		virtual bool isIdentifier();
		virtual bool isStdType();
		virtual bool isKeyword();
		virtual bool isNumber();
		virtual bool isSpecial(Str s);
		
	};

}
