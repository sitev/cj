#pragma once

#include "lang.h"
using namespace lang;

namespace cj {

	class Lexer : public lang::Lexer {
	public:
		int submode = 0;
		int level;
		Lexer();
		virtual Str step(LexemeType &type);
	};
}
