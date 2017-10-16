#pragma once

namespace cj {

	class JsGen : public Generator {
	public:
		JsGen(Parser *parser);
		~JsGen();

	protected:
		virtual Str genNumber(Node *node);
		virtual Str genVarDef(Node *node);
		virtual Str genVar(Node *node);
		virtual Str genFuncDef(Node *node);
		virtual Str genFunc(Node *node);
		virtual Str genOperator(Node *node);
		virtual Str genExpression(Node *node);
		virtual Str genCodeBlock(Node *node);
	};

}