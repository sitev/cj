#include "lang.h"
using namespace lang;

#include "cj.h"

namespace cj {

	Str std_types[] = {
		"void", "bool", "byte", "short", "int", "long", "ubyte", "ushort", "uint", "ulong", 
		"float", "double", "real", "char"
	};

	Parser::Parser(lang::Lexer *lexer) : lang::Parser(lexer) {

	}

	bool Parser::step() {
		return doCodeBlock();
	}

	bool Parser::doCodeBlock() {
		if (isSpecial(";")) return true;
		if (isSpecial("{") && doCodeBlock() && isSpecial("}")) return true;
		if (doStatement()) return true;

		return false;
	}

	bool Parser::doStatement() {
		bool iskw = false;
		if (isKeyword()) iskw = true;
		if (isIdentifier()) {
			if (isSpecial("(")) return doFunc();
			return doVar();
		}
		return false;
	}

	bool Parser::doVar() {
		return false;
	}

	bool Parser::doFunc() {
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
			int count = sizeof(std_types);
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

}