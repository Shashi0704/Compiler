#include <iostream>
#include "common.h"
#include "parser.h"

vector<Token> lexer(const string &); // from lexer.cpp
extern vector<IC> intermediateCode;

int main()
{
    vector<Token> tokens = lexer("test.asm");

    Parser parser(tokens);
    parser.parseProgram();

    cout << "\nINTERMEDIATE CODE\n";
    for (auto &ic : intermediateCode)
    {
        cout << "(" << ic.opcode << ", "
             << ic.op1 << ", "
             << ic.op2 << ")\n";
    }

    return 0;
}
