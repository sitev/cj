#include "core.h"
using namespace core;

#include "lang.h"
using namespace lang;

#include "cj.h"
using namespace cj;


#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )

int main(int argc, char* argv[])
{
	if (argc < 2) return 0;

	Str fn = argv[1];
	//cout << fn.to_string() << endl;

	File f(fn.to_string(), "rb");
	Str s;
	f.readAll(s);

	cout << s.to_string() << endl;

	Lexer *lexer = new Lexer();
	//s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer);
	s = parser->run(s);
	cout << s.to_string() << endl;

	getchar();

	return 0;
}
