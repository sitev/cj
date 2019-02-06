#include "core.h"
using namespace core;

#include "lang.h"
#include "cj.h"
using namespace cj;

#include "lua_wrap.h"

#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )
#pragma comment (lib, "lua51.lib")
#pragma comment (lib, "lua_wrap.lib")

int main(int argc, char* argv[])
{
	cout << "Cj 0.3" << endl;
	if (argc < 2) {
		cout << "Error. Missing source file. Example:" << endl;
		cout << "cj source.pcj" << endl;
		return 0;
	}


	/*
	Так видится будущее:
	1. парсим файл проектов
	2. запускаем некий интерпритатор
	...

	На данный момент
	1. парсим файл проектов
	2. поиск нужных полей
	...
	*/
	Str fn = argv[1];

	File f(fn.to_string(), "rb");
	Str s;
	f.readAll(s);

	Str sb = R"(int JsGen = 1;
int CppGen = 2;
int AsGen = 3;
Project {
	string sources[];
	string includes[];
	string namespace;
	int generator;
}

)";

	Str se = R"(

void main() {

}
)";

	s = sb + s + se;


	cj::Lexer *lexer = new cj::Lexer();       //s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer, 2);
	s = parser->run(s);
	parser->out("C:\\projects\\cjso\\cj\\examples\\11_light\\syntax_tree.txt");

	Generator *pgen = new AsGen(parser, fn);
	pgen->run();

	/*
	AngelScript as;
//	as.regCppFunc("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
	AngelScriptModule *mod = as.newModule("project_module", "C:\\projects\\cjso\\cj\\examples\\11_light\\test.pcj.as");
	int cnt = mod->GetGlobalVarCount();
	//AngelScript 
	int idx = mod->GetGlobalVarIndexByName("project");
	asIScriptObject *b = (asIScriptObject*)mod->GetAddressOfGlobalVar(idx);
	int n = b->GetPropertyCount();
	asIScriptObject *c = (asIScriptObject*)b->GetAddressOfProperty(2);
	int nc = c->GetPropertyCount();
	for (int i = 0; i < 6; i++) {
		//int tp = b->GetPropertyTypeId(i);
		char *v1 = (char*)b->GetAddressOfProperty(i);
		char *v2 = (char*)b->GetUserData(i);
		int aa = 0;
	}




	AngelScriptFunc *func = mod->getFunc("string get_str()");
	as.exec(func);
	*/
	vector<Str> sources, includes;
	Str genName, namespce;

	/*
	bool isProjectFound = false;
	int count = parser->nodes.size();
	for (int i = 0; i < count; i++) {
		Node *node = parser->nodes[i];
		cout << node->nodeType << endl;

		if (isProjectFound) {
			if (node->nodeType == ntExpression) {
				int count1 = node->nodes.size();
				if (count1 == 3) {
					Node *node2 = node->nodes[2];
					if (node2->nodeType == ntConstruct) {
						int count2 = node2->nodes.size();

						for (int i = 0; i < count2; i++) {
							Node *node3 = node2->nodes[i];
							if (node3->nodeType == ntVar) {
								Var *var = (Var*)node3;
								if (var->def->name == "sources") {
									int count3 = node3->nodes.size();
									for (int i = 0; i < count3; i++) {
										Node *node4 = node3->nodes[i];
										if (node4->nodeType == ntExpression) {
											int count4 = node4->nodes.size();
											if (count4 > 0) {
												Node *node5 = node4->nodes[0];
												if (node5->nodeType == ntString) {
													StringNode *sn = (StringNode*)node5;
													sources.push_back(sn->value);
												}
											}
										}
									}
								}
								else if (var->def->name == "includes") {
									int count3 = node3->nodes.size();
									for (int i = 0; i < count3; i++) {
										Node *node4 = node3->nodes[i];
										if (node4->nodeType == ntExpression) {
											int count4 = node4->nodes.size();
											if (count4 > 0) {
												Node *node5 = node4->nodes[0];
												if (node5->nodeType == ntString) {
													StringNode *sn = (StringNode*)node5;
													includes.push_back(sn->value);
												}
											}
										}
									}
								}
								else if (var->def->name == "namespace") {
									int count3 = node3->nodes.size();
									for (int i = 0; i < count3; i++) {
										Node *node4 = node3->nodes[i];
										if (node4->nodeType == ntExpression) {
											int count4 = node4->nodes.size();
											if (count4 > 0) {
												Node *node5 = node4->nodes[0];
												if (node5->nodeType == ntString) {
													StringNode *sn = (StringNode*)node5;
													if (sn->value.length() >= 2) {
														namespce = sn->value.substr(1, sn->value.length() - 2);
													}
												}
											}
										}
									}
								}
								else if (var->def->name == "generator") {
									int count3 = node3->nodes.size();
									for (int i = 0; i < count3; i++) {
										Node *node4 = node3->nodes[i];
										if (node4->nodeType == ntExpression) {
											int count4 = node4->nodes.size();
											if (count4 > 0) {
												Node *node5 = node4->nodes[0];
												if (node5->nodeType == ntVar) {
													Var *var = (Var*)node5;
													genName = var->def->name;
												}
											}
										}
									}
								}
							}
						}

					}
				}
			}
		}

		isProjectFound = false;

		if (node->nodeType == ntVarDef) {
			cj::VarDef *vd = (cj::VarDef*)node;
			if (vd->name == "project") {
				isProjectFound = true;
			}
			cout << vd->name.to_string() << endl;
		}
	}
	*/

	s = "";
	int count = sources.size();
	Str prefix = "C:\\projects\\cjso\\cj\\examples\\11_light\\";
	for (int i = 0; i < count; i++) {
		Str fileName = sources[i];
		if (fileName < 2) return -1;
		fileName = prefix + fileName.substr(1, fileName.length() - 2);
		File *ff = new File(fileName.to_string(), "rb");
		Str sf;
		ff->readAll(sf);
		s += sf + "\r\n";
		delete ff;
	}
	parser->run(s);
	parser->out("C:\\projects\\cjso\\cj\\examples\\11_light\\syntax_tree.txt");

	int pos = fn.rfind(".");
	fn = fn.substr(0, pos);
	Generator *generator;
	if (genName == "JsGen") generator = new JsGen(parser, fn);
	else if (genName == "CppGen") {
		generator = new CppGen(parser, fn);
		((CppGen*)generator)->setIncludes(includes);
		((CppGen*)generator)->setNamespace(namespce);
	}
	else return -1;

	generator->run();

	return 0;
}
