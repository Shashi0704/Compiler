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
