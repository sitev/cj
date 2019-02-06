int JsGen = 1;
int CppGen = 2;
int AsGen = 3;
class Project {
	array<string> sources;
	array<string> includes;
	string m_namespace;
	int generator;
}
auto project = newProject();
Project newProject() {
	auto project = Project();
	project.sources = {"first.cj", "second.cj"};
	project.includes = {};
	project.m_namespace = "";
	project.generator = CppGen;
	return project;
}
void main() {
}

