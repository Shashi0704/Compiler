#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include <vector>
#include <string>

using namespace std;

class Parser
{
private:
  vector<Token> tokens;
  int pos;
  bool inCodeSection;

public:
  Parser(const vector<Token> &t);
  void parseProgram();

private:
  bool isAtEnd();
  Token peek();
  Token advance();
  bool check(TokenType type, const string &value = "");
  bool match(TokenType type, const string &value = "");

  void parseLine();
  Instruction parseInstruction();
  vector<Operand> parseOperandList();
  Operand parseOperand();

  void generateIC(const Instruction &instr);
};

#endif
