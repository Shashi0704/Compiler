#include "backend.h"
#include <stdexcept>
#include <string>

using namespace std;

/* ======================================================
   Helper: validate MOV instruction
====================================================== */
static void validateMOV(const TypedInstruction& instr) {
    OperandType dst = instr.dst.type;
    OperandType src = instr.src.type;

    // MOV rules (8086)
    // REG <- REG | MEM | IMM
    // MEM <- REG
    // MEM <- MEM ❌
    // MEM <- IMM ❌

    if (dst == MEM && src == MEM) {
        throw runtime_error("Illegal MOV: memory to memory not allowed");
    }

    if (dst == MEM && src == IMM) {
        throw runtime_error("Illegal MOV: immediate to memory not allowed");
    }

    if (dst == NONE || src == NONE) {
        throw runtime_error("Illegal MOV: missing operand");
    }
}

/* ======================================================
   Helper: validate ADD instruction
====================================================== */
static void validateADD(const TypedInstruction& instr) {
    OperandType dst = instr.dst.type;
    OperandType src = instr.src.type;

    // ADD rules (8086)
    // REG <- REG | MEM | IMM
    // MEM <- REG
    // MEM <- MEM ❌

    if (dst == MEM && src == MEM) {
        throw runtime_error("Illegal ADD: memory to memory not allowed");
    }

    if (dst == NONE || src == NONE) {
        throw runtime_error("Illegal ADD: missing operand");
    }
}

/* ======================================================
   Helper: validate HLT instruction
====================================================== */
static void validateHLT(const TypedInstruction& instr) {
    if (instr.dst.type != NONE || instr.src.type != NONE) {
        throw runtime_error("Illegal HLT: operands not allowed");
    }
}

/* ======================================================
   Main validation function
====================================================== */
void validateInstructions(const vector<TypedInstruction>& instructions) {
    for (const auto& instr : instructions) {

        if (instr.opcode == "MOV") {
            validateMOV(instr);
        }
        else if (instr.opcode == "ADD") {
            validateADD(instr);
        }
        else if (instr.opcode == "HLT") {
            validateHLT(instr);
        }
        else {
            throw runtime_error("Unsupported instruction: " + instr.opcode);
        }
    }
}
