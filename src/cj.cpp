#include "core.h"
using namespace core;

#include "lang.h"

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

	lang::Lexer *lexer = new lang::Lexer();
	//s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer);
	s = parser->run(s);

	cout << "---------- out ---------" << endl;
	Generator *generator = new JsGen(parser);
	s = generator->run();

	cout << s.to_string() << endl;

	//getchar();

	int pos = fn.rfind(".");
	fn = fn.substr(0, pos + 1) + "js";

	File fout(fn.to_string(), "wb");
	fout.write((void*)s.to_string().c_str(), s.length());
	//fout.save();
	
	return 0;
}
