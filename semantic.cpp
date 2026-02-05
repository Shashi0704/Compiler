#include "semantic.h"
#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

/* ================================
   Symbol Table Entry
================================ */
struct Symbol {
    string name;
    int address;
    int size;   // in bytes
};

/* ================================
   Global Tables
================================ */
map<string, Symbol> symbolTable;
vector<IC> resolvedIC;

/* ================================
   Constructor
================================ */
SemanticAnalyzer::SemanticAnalyzer(const vector<Token>& t)
    : tokens(t), pos(0), dataOffset(0) {}

/* ================================
   Utility Functions
================================ */
bool SemanticAnalyzer::isAtEnd() {
    return pos >= tokens.size();
}

Token SemanticAnalyzer::peek() {
    if (isAtEnd())
        return Token{END_OF_FILE, ""};
    return tokens[pos];
}

Token SemanticAnalyzer::advance() {
    if (!isAtEnd())
        pos++;
    return tokens[pos - 1];
}

/* ================================
   Main Semantic Analysis
================================ */
void SemanticAnalyzer::analyze() {
    while (!isAtEnd()) {
        if (peek().type == DIRECTIVE && peek().value == ".DATA") {
            advance();               // consume .DATA
            processDataSection();
        } else {
            advance();
        }
    }
}

/* ================================
   Process .DATA Section
================================ */
void SemanticAnalyzer::processDataSection() {
    while (!isAtEnd()) {

        // Stop when CODE section begins
        if (peek().type == DIRECTIVE && peek().value == ".CODE")
            return;

        // Expect: IDENTIFIER DB NUMBER
        if (peek().type == IDENTIFIER) {
            string name = advance().value;

            if (!(peek().type == DIRECTIVE && peek().value == "DB")) {
                throw runtime_error("Semantic error: Expected DB after " + name);
            }
            advance(); // consume DB

            if (peek().type != NUMBER) {
                throw runtime_error("Semantic error: Expected NUMBER after DB for " + name);
            }
            advance(); // consume value (not stored yet)

            addSymbol(name, 1);
        } else {
            advance();
        }
    }
}

/* ================================
   Add Symbol to Table
================================ */
void SemanticAnalyzer::addSymbol(const string& name, int size) {
    if (symbolTable.count(name)) {
        throw runtime_error("Semantic error: Duplicate symbol " + name);
    }

    Symbol sym;
    sym.name = name;
    sym.address = dataOffset;
    sym.size = size;

    symbolTable[name] = sym;
    dataOffset += size;
}

/* ================================
   Resolve Intermediate Code
================================ */
void SemanticAnalyzer::resolveIC(const vector<IC>& intermediateCode) {
    resolvedIC.clear();

    for (const auto& ic : intermediateCode) {
        IC resolved = ic;

        // Resolve operand 1
        if (symbolTable.count(ic.op1)) {
            resolved.op1 = "[" + to_string(symbolTable[ic.op1].address) + "]";
        }

        // Resolve operand 2
        if (symbolTable.count(ic.op2)) {
            resolved.op2 = "[" + to_string(symbolTable[ic.op2].address) + "]";
        }

        resolvedIC.push_back(resolved);
    }
}

/* ================================
   Debug Output
================================ */
void SemanticAnalyzer::printSymbolTable() {
    cout << "\nSYMBOL TABLE\n";
    cout << "-----------------------------\n";
    for (auto& entry : symbolTable) {
        cout << entry.second.name
             << " -> address: "
             << entry.second.address
             << ", size: "
             << entry.second.size << endl;
    }
}
