#include "lang.h"
using namespace lang;

#include "cj.h"

#include <iostream>
using namespace std;

namespace cj {

	vector<Str> std_types = {
		"void", "bool", "byte", "short", "int", "long", "ubyte", "ushort", "uint", "ulong", 
		"float", "double", "real", "char"
	};

	vector<Str> ari_opers = { "=", "+", "-", "*", "/", "%" };

	Parser::Parser(lang::Lexer *lexer) : lang::Parser(lexer) {

	}

	bool Parser::doMainCodeBlock() {
		while (true) {
			if (isSpecial(";")) continue;
			if (isSpecial("{")) {
				if (doCodeBlock() && isSpecial("}")) continue;
				return false;
			}
			if (doMainStatement()) continue;
			return false;
		}
	}

	bool Parser::doCodeBlock() {
		while (true) {
			if (isSpecial(";")) continue;
			if (isSpecial("{")) {
				if (doCodeBlock()) continue;
				return false;
			}
			if (isSpecial("}")) return true;
			if (doStatement()) continue;
			return false;
		}
	}

	bool Parser::doMainStatement() {
		if (isKeyword() && isIdentifier()) {
			if (isSpecial("(") && doFuncDef()) return true;
			if (doVarDef()) return true;
			return false;
		}
		if (isIdentifier()) {
			if (isSpecial("(") && doFunc()) return true;
			if (doVar()) return true;
			return false;
		}
		return false;
	}

	bool Parser::doStatement() {
		if (isKeyword() && isIdentifier() && doVarDef()) return true;
		if (isIdentifier()) {
			if (isSpecial("(") && doFuncCall()) return true;
			if (doVar()) return true;
			return false;
		}
		return false;
	}

	bool Parser::doFunc() {
		if (find(")")) {
			if (isSpecial(";") && doFuncCall()) return true;
			if (isSpecial("{") && doFuncDef()) return true;
			return false;
		}
	}

	bool Parser::doFuncCall() {
		if (isSpecial(")")) return true;
		while (true) {
			if (doExpression()) {
				if (isSpecial(")")) return true;
				if (isSpecial(",")) continue;
			}
			return false;
		}
	}

	bool Parser::doFuncDef() {
		if (!doFuncDefParams()) return false;
		if (!doFuncDefBody()) return false;

		return true;
	}

	bool Parser::doFuncDefParams() {
		if (isSpecial(")")) return true;
		while (true) {
			isKeyword();
			if (isIdentifier()) {
				if (isSpecial(")")) return true;
				if (isSpecial(",")) continue;
			}
		}
	}

	bool Parser::doFuncDefBody() {
		return doCodeBlock();
	}

	bool Parser::doVar() {
		if (doVarDef()) {
			doExpression();
			return true;
		}
		if (doVarUse()) {
			doExpression();
			return true;
		}
		return false;
	}

	bool Parser::doVarDef() {
		return true;
	}

	bool Parser::doVarUse() {
		return true;
	}

	bool Parser::doExpression() {
		doUnatyOperator();
		while (true) {
			if (isSpecial("(")) {
				if (!doExpression()) return false;
				if (isSpecial(")")) return false;
			}
			else if (isNumber()) {
			}
			else if (isIdentifier()) {
				if (isSpecial("(")) {
					if (!doFuncCall()) return false;
				}
				else {
					if (!doVar()) return false;
				}
				return false;
			}
			else return false;

			if (doBinatyOperator()) continue;
			break;
		}
		return true;
	}

	bool Parser::doUnatyOperator() {
		if (isSpecial(ari_opers[1])) return true;
		if (isSpecial(ari_opers[2])) return true;
		return false;
	}

	bool Parser::doBinatyOperator() {
		int count = ari_opers.size();
		for (int i = 0; i < count; i++) {
			if (isSpecial(ari_opers[i])) return true;
		}
		return false;
	}



	bool Parser::isIdentifier() {
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			return true;
		}

		rollback();
		return false;
	}

	bool Parser::isStdType() {
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			int count = std_types.size();
			for (int i = 0; i < count; i++) {
				if (std_types[i] == token.lexeme) return true;
			}
		}
		rollback();
		return false;
	}
	
	bool Parser::isKeyword() {
		if (isStdType()) return true;
		/*
		savePosition();
		token = getToken();
		if (token.type == ltIdentifier) {
			return true;
		}

		rollback();
		*/
		return false;
	}

	bool Parser::isNumber() {
		savePosition();
		token = getToken();
		if (token.type == ltNumber) {
			return true;
		}

		rollback();
		return false;
	}

	bool Parser::isSpecial(Str s) {
		savePosition();
		token = getToken();
		if (token.type == ltSpecial) {
			if (token.lexeme == s) return true;
		}

		rollback();
		return false;
	}

	bool Parser::find(Str s) {
		while (true) {
			Token token = getToken();
			if (token.lexeme == s) return true;
			if (token.type == ltEnd) return false;
		}
	}

}