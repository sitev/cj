#pragma once

#pragma comment( lib, "core.lib" )
#pragma comment( lib, "lang.lib" )
#pragma comment (lib, "lua53.lib")
#pragma comment (lib, "lua_wrap.lib")

#include "core.h"
using namespace core;

#include "lang.h"
#include "lua_wrap.h"

#define CJ_VERSION "0.3.3"

#include "lexer.h"
#include "parser.h"
#include "js_gen.h"
#include "cpp_gen.h"
#include "csharp_gen.h"
#include "as_gen.h"
#include "lua_gen.h"
