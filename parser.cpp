#include "parser.h"
#include "common.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Global intermediate code vector
vector<IC> intermediateCode;

/* Constructor */
Parser::Parser(const vector<Token> &t) : tokens(t), pos(0), inCodeSection(false) {}

/* Utility methods */

bool Parser::isAtEnd()
{
    return pos >= tokens.size();
}

Token Parser::peek()
{
    if (isAtEnd())
        return Token{END_OF_FILE, ""};
    return tokens[pos];
}

Token Parser::advance()
{
    if (!isAtEnd())
        pos++;
    return tokens[pos - 1];
}

bool Parser::check(TokenType type, const string &value)
{
    if (isAtEnd())
        return false;
    if (tokens[pos].type != type)
        return false;
    if (!value.empty() && tokens[pos].value != value)
        return false;
    return true;
}

bool Parser::match(TokenType type, const string &value)
{
    if (check(type, value))
    {
        advance();
        return true;
    }
    return false;
}

/* Parsing logic */

void Parser::parseProgram()
{
    cout << "Parsing started...\n";

    while (!isAtEnd())
    {
        if (match(DIRECTIVE, ".CODE"))
        {
            inCodeSection = true;
            continue;
        }

        if (match(DIRECTIVE, ".DATA"))
        {
            inCodeSection = false;
            continue;
        }

        if (inCodeSection && check(INSTRUCTION))
        {
            Instruction instr = parseInstruction();
            generateIC(instr);
            continue;
        }

        // Skip other tokens
        advance();
    }
}

/* Parse a full instruction (opcode + operands) */
Instruction Parser::parseInstruction()
{
    Instruction instr;
    instr.opcode = advance().value; // consume opcode

    // Check if next token can start an operand
    if (!isAtEnd() && (peek().type == REGISTER || peek().type == NUMBER || peek().type == IDENTIFIER || (peek().type == SYMBOL && peek().value == "[")))
    {
        instr.operands = parseOperandList();
    }

    return instr;
}

/* Parse comma-separated operand list */
vector<Operand> Parser::parseOperandList()
{
    vector<Operand> operands;

    operands.push_back(parseOperand());

    while (match(SYMBOL, ","))
    {
        operands.push_back(parseOperand());
    }

    return operands;
}

/* Parse a single operand */
Operand Parser::parseOperand()
{
    Operand op;

    // Simple operand types
    if (match(REGISTER) || match(NUMBER) || match(IDENTIFIER))
    {
        op.value = tokens[pos - 1].value;
        return op;
    }

    // Memory operand: [REGISTER] or [IDENTIFIER]
    if (match(SYMBOL, "["))
    {
        if (!(match(REGISTER) || match(IDENTIFIER)))
        {
            throw runtime_error("Invalid memory operand: expected REGISTER or IDENTIFIER after '['");
        }
        string inner = tokens[pos - 1].value;

        if (!match(SYMBOL, "]"))
        {
            throw runtime_error("Missing closing ']' in memory operand");
        }

        op.value = "[" + inner + "]";
        return op;
    }

    throw runtime_error("Invalid operand");
}

/* Generate Intermediate Code from instruction */
void Parser::generateIC(const Instruction &instr)
{
    IC ic;
    ic.opcode = instr.opcode;
    ic.op1 = instr.operands.size() > 0 ? instr.operands[0].value : "";
    ic.op2 = instr.operands.size() > 1 ? instr.operands[1].value : "";
    intermediateCode.push_back(ic);
}
