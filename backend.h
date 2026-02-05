#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>  
#include "common.h"

using namespace std;

/* ================= Operand Typing ================= */

enum OperandType {
    REG,
    MEM,
    IMM,
    NONE
};

struct TypedOperand {
    OperandType type;
    int value;      // register id | memory address | immediate value
};

struct TypedInstruction {
    string opcode;
    TypedOperand dst;
    TypedOperand src;
};

/* ================= 8086 Register Encoding ================= */
/* static prevents multiple-definition linker errors */
static map<string, int> registerCode = {
    {"AX", 0}, {"CX", 1}, {"DX", 2}, {"BX", 3},
    {"SP", 4}, {"BP", 5}, {"SI", 6}, {"DI", 7}
};

/* ================= Backend API ================= */

extern vector<TypedInstruction> typedInstructions;
void generateTypedInstructions(const vector<IC>& resolvedIC);
void validateInstructions(const vector<TypedInstruction>& instructions);
extern vector<uint8_t> machineCode;
void generateMachineCode(const vector<TypedInstruction>& instructions);
void runEmulator(const vector<uint8_t>& machineCode);


#endif
