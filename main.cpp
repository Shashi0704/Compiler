#include <iostream>
#include "common.h"
#include "parser.h"
#include "semantic.h"

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

    SemanticAnalyzer sem(tokens);
    sem.analyze();
    sem.printSymbolTable();
    sem.resolveIC(intermediateCode);

    cout << "\nRESOLVED INTERMEDIATE CODE\n";
    for (auto &ic : resolvedIC)
    {
        cout << "(" << ic.opcode << ", "
             << ic.op1 << ", "
             << ic.op2 << ")\n";
    }

    return 0;
}
