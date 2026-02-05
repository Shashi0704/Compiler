#include "backend.h"
#include "common.h"
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

using namespace std;

/* =========================================
   Global Typed Instruction List
========================================= */
vector<TypedInstruction> typedInstructions;

/* =========================================
   Helper Functions
========================================= */

// Check if string is a number
static bool isNumber(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit(c)) return false;
    return true;
}

// Parse operand string into TypedOperand
TypedOperand parseOperand(const string& op) {
    TypedOperand result;
    result.type = NONE;
    result.value = 0;

    if (op.empty())
        return result;

    // Register operand
    if (registerCode.count(op)) {
        result.type = REG;
        result.value = registerCode[op];
        return result;
    }

    // Memory operand: [number]
    if (op.front() == '[' && op.back() == ']') {
        string addr = op.substr(1, op.size() - 2);
        if (!isNumber(addr)) {
            throw runtime_error("Invalid memory address: " + op);
        }
        result.type = MEM;
        result.value = stoi(addr);
        return result;
    }

    // Immediate operand
    if (isNumber(op)) {
        result.type = IMM;
        result.value = stoi(op);
        return result;
    }

    throw runtime_error("Unknown operand type: " + op);
}

/* =========================================
   Main Typing Function
========================================= */

void generateTypedInstructions(const vector<IC>& resolvedIC) {
    typedInstructions.clear();

    for (const auto& ic : resolvedIC) {
        TypedInstruction ti;
        ti.opcode = ic.opcode;

        ti.dst = parseOperand(ic.op1);
        ti.src = parseOperand(ic.op2);

        typedInstructions.push_back(ti);
    }
}
