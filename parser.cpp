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


void Parser::generateIC(const Instruction& instr) {
    IC ic;
    ic.opcode = instr.opcode;
    ic.op1 = instr.operands.size() > 0 ? instr.operands[0].value : "";
    ic.op2 = instr.operands.size() > 1 ? instr.operands[1].value : "";
    intermediateCode.push_back(ic);
}
void Parser::parseLine() {
    if (check(INSTRUCTION)) {
        Instruction instr = parseInstruction();
        generateIC(instr);  
        return;
    }
    advance();
}

// private function parsing

