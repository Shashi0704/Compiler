#include <iostream>
#include "common.h"
#include "parser.h"
#include "semantic.h"
#include "backend.h"

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

    generateTypedInstructions(resolvedIC);

    cout << "\nTYPED INSTRUCTIONS\n";
    for (auto& ti : typedInstructions) {
        cout << ti.opcode
             << " | dst(type=" << ti.dst.type << ", val=" << ti.dst.value << ")"
             << " | src(type=" << ti.src.type << ", val=" << ti.src.value << ")\n";
    }

    validateInstructions(typedInstructions);
    cout << "\nInstruction validation passed.\n";

    generateMachineCode(typedInstructions);

    cout << "\nMACHINE CODE\n";
    for (auto b : machineCode) {
        printf("%02X ", b);
    }
    cout << endl;

    runEmulator(machineCode);


    return 0;
}
