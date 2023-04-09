#ifndef __BC23_BASM
#define __BC23_BASM

#include "def.h"

extern const char *OPCODES[];

enum TokenType {
    LABEL,
    ADDRESS,
    LITERAL,
    OP_LDA, OP_LDX, OP_LDY, OP_STA, OP_STX, OP_STY
};
struct Token {
    enum TokenType type;
    int literal; // addresses, numbers
    char *metadata; // label names, string literals
};
struct BASMParseOutput {
    char *syntax_highlighted;
    struct Token *token_stream;
    size_t token_count;
    char *error;
};

struct BASMParseOutput basm_parse(char *program);

#define BASM_HIGHLIGHT_LABEL    TEXT_CYAN
#define BASM_HIGHLIGHT_OPCODE   TEXT_PINK

#endif