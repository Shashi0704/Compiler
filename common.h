#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
using namespace std;

enum TokenType
{
    INSTRUCTION,
    REGISTER,
    DIRECTIVE,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    COMMA,
    COLON,
    END_OF_FILE

};

struct Token
{
    TokenType type;
    string value;
};

struct Operand
{
    string value;
};

struct Instruction
{
    string opcode;
    vector<Operand> operands;
};

struct IC
{
    string opcode;
    string op1;
    string op2;
};
vector<Token> lexer(const string &filename);

#endif
