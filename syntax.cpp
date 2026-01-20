#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using namespace std;

// Token definitions (must match lexer) 

enum TokenType {
    INSTRUCTION,
    REGISTER,
    DIRECTIVE,
    IDENTIFIER,
    NUMBER,
    SYMBOL
};

struct Token {
    TokenType type;
    string value;
};

// Parser Data Structures 

struct Operand {
    string value;
};

struct Instruction {
    string opcode;
    vector<Operand> operands;
};

// Parser Class 

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
    Token previous();
    Token advance();
    bool check(TokenType type, const string& value = "");
    bool match(TokenType type, const string& value = "");

    void parseLine();
    void parseLabel();
    Instruction parseInstruction();
    vector<Operand> parseOperandList();
    Operand parseOperand();
};

// Helper Functions 

bool Parser::isAtEnd() {
    return pos >= tokens.size();
}

Token Parser::peek() {
    if (isAtEnd())
        return Token{SYMBOL, ""};
    return tokens[pos];
}

Token Parser::previous() {
    return tokens[pos - 1];
}

Token Parser::advance() {
    if (!isAtEnd())
        pos++;
    return previous();
}

bool Parser::check(TokenType type, const string& value) {
    if (isAtEnd())
        return false;
    if (tokens[pos].type != type)
        return false;
    if (!value.empty() && tokens[pos].value != value)
        return false;
    return true;
}

bool Parser::match(TokenType type, const string& value) {
    if (check(type, value)) {
        advance();
        return true;
    }
    return false;
}

// Parsing Logic 

void Parser::parseProgram() {
    while (!isAtEnd()) {
        parseLine();
    }
}

void Parser::parseLine() {
    // LABEL?
    if (check(IDENTIFIER) &&
        pos + 1 < tokens.size() &&
        tokens[pos + 1].value == ":") {
        parseLabel();
        return;
    }

    // DIRECTIVE? → skip safely
    if (check(DIRECTIVE)) {
        advance(); // consume directive

        // consume rest of directive line (operands if any)
        while (!isAtEnd() &&
               !check(INSTRUCTION) &&
               !check(DIRECTIVE) &&
               !check(IDENTIFIER)) {
            advance();
        }
        return;
    }

    // INSTRUCTION?
    if (check(INSTRUCTION)) {
        Instruction instr = parseInstruction();

        cout << "INSTRUCTION: " << instr.opcode << endl;
        for (size_t i = 0; i < instr.operands.size(); i++) {
            cout << "  OPERAND " << i + 1 << ": "
                 << instr.operands[i].value << endl;
        }
        return;
    }

    // Otherwise skip token
    advance();
}

void Parser::parseLabel() {
    string labelName = advance().value; // IDENTIFIER
    advance(); // consume ':'

    cout << "LABEL: " << labelName << endl;
}

Instruction Parser::parseInstruction() {
    Instruction instr;
    instr.opcode = advance().value; // INSTRUCTION

    // Check if next token CAN start an operand
    if (!isAtEnd() &&
        (peek().type == REGISTER ||
         peek().type == IDENTIFIER ||
         peek().type == NUMBER ||
         (peek().type == SYMBOL && peek().value == "["))) {

        instr.operands = parseOperandList();
    }

    return instr;
}

vector<Operand> Parser::parseOperandList() {
    vector<Operand> operands;
    operands.push_back(parseOperand());

    while (match(SYMBOL, ",")) {
        operands.push_back(parseOperand());
    }

    return operands;
}

Operand Parser::parseOperand() {
    Operand op;

    /* REGISTER | NUMBER | IDENTIFIER */
    if (match(REGISTER) || match(NUMBER) || match(IDENTIFIER)) {
        op.value = previous().value;
        return op;
    }

    /* MEMORY OPERAND: [ REGISTER | IDENTIFIER ] */
    if (match(SYMBOL, "[")) {
        if (!match(REGISTER) && !match(IDENTIFIER)) {
            throw runtime_error("Invalid memory operand");
        }
        string inner = previous().value;

        if (!match(SYMBOL, "]")) {
            throw runtime_error("Missing closing ']'");
        }

        op.value = "[" + inner + "]";
        return op;
    }

    throw runtime_error("Invalid operand");
}

// Example main() for testing 

int main() {
    extern vector<Token> lexer(const string&);
    vector<Token> tokens = lexer("test.asm");

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}

