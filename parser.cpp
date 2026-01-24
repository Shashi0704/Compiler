#include "common.h"
#include <iostream>
#include <stdexcept>

using namespace std;

vector<IC> intermediateCode;

class Parser {
private:
    vector<Token> tokens;
    size_t pos = 0;

public:
    Parser(const vector<Token>& t) : tokens(t) {}

    void parseProgram();

private:
    bool isAtEnd();
    Token peek();
    Token advance();
    bool check(TokenType type, const string& value = "");
    bool match(TokenType type, const string& value = "");

    void parseLine();
    Instruction parseInstruction();
    vector<Operand> parseOperandList();
    Operand parseOperand();

    void generateIC(const Instruction& instr);
};

/* ---------------- Parser Utility Functions ---------------- */

bool Parser::isAtEnd() {
    return pos >= tokens.size();
}

Token Parser::peek() {
    if (isAtEnd()) return Token{END, ""};
    return tokens[pos];
}

Token Parser::advance() {
    if (!isAtEnd()) pos++;
    return tokens[pos - 1];
}

bool Parser::check(TokenType type, const string& value) {
    if (isAtEnd()) return false;
    if (tokens[pos].type != type) return false;
    if (!value.empty() && tokens[pos].value != value) return false;
    return true;
}

bool Parser::match(TokenType type, const string& value) {
    if (check(type, value)) {
        advance();
        return true;
    }
    return false;
}

/* ---------------- Parsing Logic ---------------- */

void Parser::parseProgram() {
    while (!isAtEnd()) {
        parseLine();
    }
}

void Parser::parseLine() {
    if (check(INSTRUCTION)) {
        Instruction instr = parseInstruction();
        generateIC(instr);
    } else {
        advance(); // skip unknown tokens
    }
}

Instruction Parser::parseInstruction() {
    Instruction instr;

    // Opcode
    instr.opcode = advance().value;

    // Operands
    instr.operands = parseOperandList();

    return instr;
}

vector<Operand> Parser::parseOperandList() {
    vector<Operand> ops;

    while (!isAtEnd() && !check(INSTRUCTION)) {
        if (match(COMMA)) continue;
        ops.push_back(parseOperand());
    }

    return ops;
}

Operand Parser::parseOperand() {
    Operand op;

    if (check(REGISTER) || check(NUMBER)) {
        op.value = advance().value;
        return op;
    }

    throw runtime_error("Invalid operand");
}

/* ---------------- Intermediate Code Generation ---------------- */

void Parser::generateIC(const Instruction& instr) {
    IC ic;
    ic.opcode = instr.opcode;
    ic.op1 = instr.operands.size() > 0 ? instr.operands[0].value : "";
    ic.op2 = instr.operands.size() > 1 ? instr.operands[1].value : "";
    intermediateCode.push_back(ic);
}
