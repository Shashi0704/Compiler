#include <iostream>
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>

using namespace std;

/* ================================
   CPU STATE
================================ */
struct CPU {
    uint16_t AX = 0, BX = 0, CX = 0, DX = 0;
    uint16_t SP = 0, BP = 0, SI = 0, DI = 0;
    uint16_t IP = 0;
    bool halted = false;
};

/* ================================
   MEMORY
================================ */
static uint8_t memory[65536];

/* ================================
   Helpers
================================ */
static uint16_t& getReg(CPU& cpu, int reg) {
    switch (reg) {
        case 0: return cpu.AX;
        case 1: return cpu.CX;
        case 2: return cpu.DX;
        case 3: return cpu.BX;
        case 4: return cpu.SP;
        case 5: return cpu.BP;
        case 6: return cpu.SI;
        case 7: return cpu.DI;
        default: throw runtime_error("Invalid register");
    }
}

static uint8_t fetch8(CPU& cpu) {
    return memory[cpu.IP++];
}

static uint16_t fetch16(CPU& cpu) {
    uint16_t lo = fetch8(cpu);
    uint16_t hi = fetch8(cpu);
    return lo | (hi << 8);
}

/* ================================
   EMULATOR CORE
================================ */
void runEmulator(const vector<uint8_t>& machineCode) {
    CPU cpu;

    // Load code into memory
    for (size_t i = 0; i < machineCode.size(); i++)
        memory[i] = machineCode[i];

    cpu.IP = 0;

    while (!cpu.halted) {
        uint8_t opcode = fetch8(cpu);

        switch (opcode) {

        /* -------- MOV r16, imm16 -------- */
        case 0xB8: case 0xB9: case 0xBA: case 0xBB:
        case 0xBC: case 0xBD: case 0xBE: case 0xBF: {
            int reg = opcode - 0xB8;
            getReg(cpu, reg) = fetch16(cpu);
            break;
        }

        /* -------- MOV r16, r/m16 -------- */
        case 0x8B: {
            uint8_t modrm = fetch8(cpu);
            int mod = modrm >> 6;
            int reg = (modrm >> 3) & 7;
            int rm  = modrm & 7;

            if (mod == 0b11) {
                getReg(cpu, reg) = getReg(cpu, rm);
            } else if (mod == 0b00 && rm == 0b110) {
                uint16_t addr = fetch16(cpu);
                getReg(cpu, reg) = *(uint16_t*)&memory[addr];
            }
            break;
        }

        /* -------- MOV r/m16, r16 -------- */
        case 0x89: {
            uint8_t modrm = fetch8(cpu);
            int mod = modrm >> 6;
            int reg = (modrm >> 3) & 7;
            int rm  = modrm & 7;

            if (mod == 0b11) {
                getReg(cpu, rm) = getReg(cpu, reg);
            } else if (mod == 0b00 && rm == 0b110) {
                uint16_t addr = fetch16(cpu);
                *(uint16_t*)&memory[addr] = getReg(cpu, reg);
            }
            break;
        }

        /* -------- ADD r16, r/m16 -------- */
        case 0x03: {
            uint8_t modrm = fetch8(cpu);
            int mod = modrm >> 6;
            int reg = (modrm >> 3) & 7;
            int rm  = modrm & 7;

            if (mod == 0b11) {
                getReg(cpu, reg) += getReg(cpu, rm);
            } else if (mod == 0b00 && rm == 0b110) {
                uint16_t addr = fetch16(cpu);
                getReg(cpu, reg) += *(uint16_t*)&memory[addr];
            }
            break;
        }

        /* -------- ADD r/m16, r16 -------- */
        case 0x01: {
            uint8_t modrm = fetch8(cpu);
            int mod = modrm >> 6;
            int reg = (modrm >> 3) & 7;
            int rm  = modrm & 7;

            if (mod == 0b11) {
                getReg(cpu, rm) += getReg(cpu, reg);
            } else if (mod == 0b00 && rm == 0b110) {
                uint16_t addr = fetch16(cpu);
                *(uint16_t*)&memory[addr] += getReg(cpu, reg);
            }
            break;
        }

        /* -------- ADD AX, imm16 -------- */
        case 0x05:
            cpu.AX += fetch16(cpu);
            break;

        /* -------- ADD r/m16, imm16 -------- */
        case 0x81: {
            uint8_t modrm = fetch8(cpu);
            int subop = (modrm >> 3) & 7;
            int rm = modrm & 7;

            if (subop != 0)
                throw runtime_error("Unsupported 0x81 opcode");

            uint16_t imm = fetch16(cpu);

            if ((modrm >> 6) == 0b11) {
                getReg(cpu, rm) += imm;
            }
            break;
        }

        /* -------- HLT -------- */
        case 0xF4:
            cpu.halted = true;
            break;

        default:
            throw runtime_error("Unknown opcode: " + to_string(opcode));
        }
    }

    /* ================================
       FINAL REGISTER STATE
    ================================ */
    cout << "\nCPU STATE AFTER EXECUTION\n";
    cout << "AX = " << cpu.AX << endl;
    cout << "BX = " << cpu.BX << endl;
    cout << "CX = " << cpu.CX << endl;
    cout << "DX = " << cpu.DX << endl;
}
