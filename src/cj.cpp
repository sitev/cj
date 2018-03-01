#include "core.h"
using namespace core;

#include "lang.h"

#include "cj.h"
using namespace cj;

#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )

int main(int argc, char* argv[])
{
	cout << "Cj 0.3" << endl;
	if (argc < 2) {
		cout << "Error. Missing source file. Example:" << endl;
		cout << "cj source.cj" << endl;
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

	Str sb = R"(
		int JsGen = 1;
		int CppGen = 2;
		Project{
			string sources[];
			string includes[];
			string namespace;
			int generator;
		}
	)";

	s = sb + s;

	cj::Lexer *lexer = new cj::Lexer();       //s = lexer->run(s);
	cj::Parser *parser = new cj::Parser(lexer);
	s = parser->run(s);

	vector<Str> sources, includes;
	Str genName, namespce;

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

	s = "";
	count = sources.size();
	Str prefix = "c:/projects/cj/examples/10_webapp_shop/";
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
	parser->out("c:/projects/cj/examples/10_webapp_shop/syntax_tree.txt");

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
