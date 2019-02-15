﻿#pragma once
#include<stack>
#include<string>
#include<cstdlib>
#include<cmath>
#include "nodetype.h"
#include "scope.h"
using namespace std;

class record
{
public:
    static Scope globalScope;
};

namespace ast
{
    extern bool isInit;
    extern map<string,int> BinOpPriority;

    static void Init();
    static bool canpush(stack<string> &, string);
    static bool isNum(const char &);
    static bool isBinOp(const char &);
    static bool isLetter(const char &);
    static BasicNode* __ToAST(string &);

    BasicNode* ToAST(string);
}
