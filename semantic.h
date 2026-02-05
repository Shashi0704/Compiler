#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "common.h"
#include <vector>

class SemanticAnalyzer {
private:
    std::vector<Token> tokens;
    int pos;
    int dataOffset;

    bool isAtEnd();
    Token peek();
    Token advance();

    void processDataSection();
    void addSymbol(const std::string& name, int size);

public:
    SemanticAnalyzer(const std::vector<Token>& tokens);

    void analyze();
    void resolveIC(const std::vector<IC>& intermediateCode);
    void printSymbolTable();
};

extern std::vector<IC> resolvedIC;

#endif
