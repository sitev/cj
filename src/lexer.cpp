#include "cj.h"
#include "lang.h"
using namespace lang;

namespace cj {
	Lexer::Lexer() {
	}

	Str Lexer::step(LexemeType &type) {
		Str a;
		while (pos < len) {
			ubyte c = s[pos];

			if (c == 13) isCR = true; else isCR = false;
			if (c <= 32) {
				if (!(mode == lmCodeInsertion && submode == 3))
					c = 32;
			}

			if (mode == lmNormal) {
				if (a.length() > 0 && !isLetter(c) && !isNumeral(c) && !isSlesh && !isMul) return a;
				pos++;
				if (c != 32)
					a = a + (char)c;

				if (c == '\"') {
					mode = lmString;
					continue;
				}
				if (c == '*' && isSlesh) {
					mode = lmRemmark;
					continue;
				}
				if (c == '/' && isSlesh) {
					mode = lmRemmark2;
					continue;
				}
				if (c == '@') {
					mode = lmCodeInsertion;
					submode = 0;
					return a;
				}

				if (c == '/') isSlesh = true; else isSlesh = false;

				if (a.length() > 0 && !isLetter(c) && !isNumeral(c) && !isSlesh && !isMul) return a;
			}
			else if (mode == lmRemmark) {
				pos++;
				a = a + (char)c;
				if (c == '/' && isMul) {
					mode = lmNormal;
					return a;
				}
				if (c == '*') isMul = true; else isMul = false;
			}
			else if (mode == lmRemmark2) {
				pos++;
				a = a + (char)c;
				if (isCR) {
					mode = lmNormal;
					return a;
				}
				if (c == '*') isMul = true; else isMul = false;
			}
			else if (mode == lmString) {
				pos++;
				a = a + (char)c;
				if (c == '\"') {
					mode = lmNormal;
					return a;
				}
			}
			else if (mode == lmCodeInsertion) {
				if (submode == 0) {
					pos++;
					submode = 1;
					if (!isLetter(c)) return "";
					a = a + (char)c;
				}
				else if (submode == 1) {
					pos++;
					if (c != 32)
						a = a + (char)c;
					if (isLetter(c) || isNumeral(c)) continue;
					pos--;
					submode = 2;
					return a;
				}
				else if (submode == 2) {
					pos++;
					if (c == 32) continue;
					if (c == '{') {
						submode = 3;
						level = 1;
					}
				}
				else if (submode == 3) {
					if (c == '{') level++;
					else if (c == '}') level--;

					pos++;

					if (level <= 0) {
						type = ltCodeInsertion;
						mode = lmNormal;
						submode = 0;
						return a;
					}

					a = a + (char)c;
				}
			}
		}
		return a;
	}

}