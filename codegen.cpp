#include "backend.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

using namespace std;

/* =========================================
   Global Machine Code Buffer
========================================= */
vector<uint8_t> machineCode;

/* =========================================
   Helper: emit byte / word
========================================= */
static void emit8(uint8_t b) {
    machineCode.push_back(b);
}

static void emit16(uint16_t w) {
    emit8(w & 0xFF);
    emit8((w >> 8) & 0xFF);
}

/* =========================================
   Encode ModR/M byte
========================================= */
static uint8_t modRM(uint8_t mod, uint8_t reg, uint8_t rm) {
    return (mod << 6) | (reg << 3) | rm;
}

/* =========================================
   MOV encoding
========================================= */
static void encodeMOV(const TypedInstruction& i) {
    auto dst = i.dst;
    auto src = i.src;

    // MOV reg, imm
    if (dst.type == REG && src.type == IMM) {
        emit8(0xB8 + dst.value);   // MOV r16, imm16
        emit16(src.value);
        return;
    }

    // MOV reg, reg
    if (dst.type == REG && src.type == REG) {
        emit8(0x8B);               // MOV r16, r/m16
        emit8(modRM(0b11, dst.value, src.value));
        return;
    }

    // MOV reg, mem
    if (dst.type == REG && src.type == MEM) {
        emit8(0x8B);               // MOV r16, r/m16
        emit8(modRM(0b00, dst.value, 0b110));
        emit16(src.value);
        return;
    }

    // MOV mem, reg
    if (dst.type == MEM && src.type == REG) {
        emit8(0x89);               // MOV r/m16, r16
        emit8(modRM(0b00, src.value, 0b110));
        emit16(dst.value);
        return;
    }

    throw runtime_error("Unsupported MOV encoding");
}

/* =========================================
   ADD encoding
========================================= */
static void encodeADD(const TypedInstruction& i) {
    auto dst = i.dst;
    auto src = i.src;

    // ADD reg, reg
    if (dst.type == REG && src.type == REG) {
        emit8(0x03);               // ADD r16, r/m16
        emit8(modRM(0b11, dst.value, src.value));
        return;
    }

    // ADD reg, mem
    if (dst.type == REG && src.type == MEM) {
        emit8(0x03);
        emit8(modRM(0b00, dst.value, 0b110));
        emit16(src.value);
        return;
    }

    // ADD mem, reg
    if (dst.type == MEM && src.type == REG) {
        emit8(0x01);               // ADD r/m16, r16
        emit8(modRM(0b00, src.value, 0b110));
        emit16(dst.value);
        return;
    }

    // ADD reg, imm
    if (dst.type == REG && src.type == IMM) {
        if (dst.value == 0) {
            emit8(0x05);           // ADD AX, imm16
            emit16(src.value);
        } else {
            emit8(0x81);           // ADD r/m16, imm16
            emit8(modRM(0b11, 0b000, dst.value));
            emit16(src.value);
        }
        return;
    }

    throw runtime_error("Unsupported ADD encoding");
}

/* =========================================
   Main code generator
========================================= */
void generateMachineCode(const vector<TypedInstruction>& instructions) {
    machineCode.clear();

    for (const auto& instr : instructions) {
        if (instr.opcode == "MOV") {
            encodeMOV(instr);
        }
        else if (instr.opcode == "ADD") {
            encodeADD(instr);
        }
        else if (instr.opcode == "HLT") {
            emit8(0xF4);           // HLT
        }
        else {
            throw runtime_error("Unknown instruction: " + instr.opcode);
        }
    }
}
