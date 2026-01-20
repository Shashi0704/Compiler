#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cctype>
#include <string>

using namespace std;

enum TokenType
{
  INSTRUCTION,
  REGISTER,
  DIRECTIVE,
  IDENTIFIER,
  NUMBER,
  SYMBOL
};

string tokenTypeToString(TokenType t)
{
  switch (t)
  {
  case INSTRUCTION:
    return "INSTRUCTION";

  case REGISTER:
    return "REGISTER";

  case DIRECTIVE:
    return "DIRECTIVE";

  case IDENTIFIER:
    return "IDENTIFIER";

  case NUMBER:
    return "NUMBER";

  case SYMBOL:
    return "SYMBOL";

  default:
    return "no such token";
  }
  return "UNKNOWN";
};

struct Token
{
  TokenType type;
  string value;
};

set<string> instruction = {
    "ADD", "ADC", "SUB", "SBB", "INC", "DEC", "MUL", "IMUL", "DIV", "IDIV", "NEG", "CMP", "DAA", "DAS", "AAA", "AAS", "AAM", "AAD", "MOV", "XCHG", "XLAT", "PUSH", "POP", "IN", "OUT", "LEA", "LDS", "LES", "LAHF", "SAHF", "HLT", "NOP", "WAIT", "ESC", "LOCK", "CLC", "STC", "CMC", "CLD", "STD", "CLI", "STI", "MOVSB", "MOVSW", "CMPSB", "CMPSW", "SCASB", "SCASW", "LODSB", "LODSW", "STOSB", "STOSW", "JMP", "CALL", "RET", "JE", "JZ", "JNE", "JNZ", "JA", "JAE", "JB", "JBE", "JG", "JGE", "JL", "JLE", "JC", "JNC", "JO", "JNO", "JS", "JNS", "LOOP", "LOOPE", "LOOPNE", "JCXZ", "SHL", "SAL", "SHR", "SAR", "ROL", "ROR", "RCL", "RCR", "AND", "OR", "XOR", "NOT", "TEST"};

set<string> registers = {
    "CS", "DS", "ES", "SS", "IP", "FLAGS", "SP", "BP", "SI", "DI", "AL", "AH", "BL", "BH", "CL", "CH", "DL", "DH", "AX", "BX", "CX", "DX"

};

set<string> directives = {
    ".DATA", ".CODE", ".STACK", "DB", "DW", "DD", "DQ", "DT", "ORG", "END", "ASSUME", "PROC", "ENDP", "PUBLIC", "EXTRN", "LABEL", "MACRO", "ENDM"

};

set<char> singleSymbols = {
    '+', '-', '*', '/', '%', '=', '<', '>',
    ',', ':', '(', ')', '[', ']',
    '.', '$', '@', '?', '#', '!',
    '&', '|', '^', '\'', '"', '~'};

set<string> multiSymbols = {
    "==", "!=", "<=", ">=", "<<", ">>"};

// helper function

string toUpper(string s)
{
  for (char &c : s)
    c = toupper(c);
  return s;
}

bool isNumber(const string &s)
{
  if (isdigit(s[0]))
    return true;
  return false;
}

// lexer function
vector<Token> lexer(const string &filename)
{
  ifstream fin(filename);
  vector<Token> tokens;

  if (!fin)
  {
    cout << "Error opening file\n";
    return tokens;
  }

  string line;

  while (getline(fin, line))
  {
    // remove comments
    size_t pos = line.find(';');
    if (pos != string::npos)
      line = line.substr(0, pos);

    int i = 0, n = line.length();

    while (i < n)
    {
      if (isspace(line[i]))
      {
        i++;
        continue;
      }

      // identifier / keyword
      if (isalpha(line[i]) || line[i] == '_' || line[i] == '.')
      {
        string word;
        while (i < n && (isalnum(line[i]) || line[i] == '_' || line[i] == '.'))
          word += line[i++];

        word = toUpper(word);

        if (instruction.count(word))
          tokens.push_back({INSTRUCTION, word});
        else if (registers.count(word))
          tokens.push_back({REGISTER, word});
        else if (directives.count(word))
          tokens.push_back({DIRECTIVE, word});
        else
          tokens.push_back({IDENTIFIER, word});

        continue;
      }

      // number
      if (isdigit(line[i]))
      {
        string num;
        while (i < n && isalnum(line[i]))
          num += line[i++];
        tokens.push_back({NUMBER, num});
        continue;
      }

      // multi-character symbol first
      if (i + 1 < n)
      {
        string two = line.substr(i, 2);
        if (multiSymbols.count(two))
        {
          tokens.push_back({SYMBOL, two});
          i += 2;
          continue;
        }
      }

      // singleSymbols
      if (singleSymbols.count(line[i]))
      {
        tokens.push_back({SYMBOL, string(1, line[i])});
        i++;
      }
      i++;
    }
  }

  fin.close();
  return tokens;
}
// main function for exection
int main()
{
  vector<Token> tokens = lexer("test.asm");

  for (auto &t : tokens)
    cout << tokenTypeToString(t.type) << " -> " << t.value << endl;

  return 0;
}
