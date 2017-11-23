int JsGen = 1;
int CppGen = 2;

Project {
	string sources[];
	int generator;
	int a;
}

project = Project {
	sources: [ "file1.cj", "file2.cj", "main.cj" ],
	generator: CppGen
}

