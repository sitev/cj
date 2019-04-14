#include "core.h"
using namespace core;

#include "lang.h"
#include "cj.h"
using namespace cj;

#include "lua_wrap.h"

#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )
#pragma comment (lib, "lua53.lib")
#pragma comment (lib, "lua_wrap.lib")

int main(int argc, char* argv[])
{
	cout << "Cj " << CJ_VERSION << endl << "----------" << endl;
	if (argc < 2) {
		cout << "Error. Missing source file. Example:" << endl;
		cout << "cj source.pcj" << endl;
		return 0;
	}

	Str fn = argv[1];

	File f(fn.to_string(), "rb");
	Str s;
	f.readAll(s);

	Str sb = R"(int JsGen = 1;
int CppGen = 2;
int CSharpGen = 3;
int LuaGen = 4;
int AsGen = 5;

Project {
	string sources[];
	string includes[];
	string namespace;
	int generator;
}

)";

	Str se = R"(
)";

	s = sb + s + se;
	cout << s.to_string() << endl;

	cj::Lexer *lexer = new cj::Lexer();       //s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer, 2);
	s = parser->run(s);
	//parser->out("C:\\projects\\cjso\\cj\\examples\\11_light\\syntax_tree.txt");

	Generator *pgen = new LuaGen(parser, fn);
	pgen->run();

	Lua lua;
	int result = lua.open((fn + ".lua").to_string());

	vector<Str> sources, includes;
	auto project = lua.get("project");
	auto srcs = project["sources"];
	for (int i = 0; i < srcs.length(); i++) {
		string s = srcs[i + 1];
		sources.push_back(s);
	}
	auto incs = project["includes"];
	for (int i = 0; i < incs.length(); i++) {
		string s = incs[i + 1];
		includes.push_back(s);
	}
	
	Str namespce = project["m_namespace"].cast<string>();
	GeneratorType genType = (GeneratorType)project["generator"].cast<int>();

	int pos = fn.rfind("\\");
	Str prefix = fn.substr(0, pos + 1);
	s = "";
	int count = sources.size();
	for (int i = 0; i < count; i++) {
		Str fileName = sources[i];
		if (fileName < 2) return -1;
		fileName = prefix + fileName;
		File *ff = new File(fileName.to_string(), "rb");
		Str sf;
		ff->readAll(sf);
		s += sf + "\r\n";
		delete ff;
	}
	parser->run(s);
	//parser->out(prefix + "syntax_tree.txt");

	int pfn = fn.rfind(".");
	fn = fn.substr(0, pfn);
	Generator *generator;
	switch (genType) {
		case gtJsGen: generator = new JsGen(parser, fn); break;
		case gtCppGen:  
			generator = new CppGen(parser, fn);
			((CppGen*)generator)->setIncludes(includes);
			((CppGen*)generator)->setNamespace(namespce);
			break;
		default:
			return -1;
	}

	generator->run();

	return 0;
}
