#include "core.h"
using namespace core;

#include "lang.h"

#include "cj.h"
using namespace cj;


#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )

int main(int argc, char* argv[])
{
	cout << "Cj compiler 0.1" << endl;
	if (argc < 2) {
		cout << "Error. Missing source file. Example:" << endl;
		cout << "cj source.cj" << endl;
		return 0;
	}

	Str fn = argv[1];

	File f(fn.to_string(), "rb");
	Str s;
	f.readAll(s);

	lang::Lexer *lexer = new lang::Lexer();
	//s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer);
	s = parser->run(s);

	Generator *generator = new JsGen(parser);
	s = generator->run();

	int pos = fn.rfind(".");
	fn = fn.substr(0, pos + 1) + "js";

	cout << "Target file: " << fn.to_string() << endl;

	File fout(fn.to_string(), "wb");
	fout.write((void*)s.to_string().c_str(), s.length());
	
	return 0;
}
