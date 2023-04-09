#ifndef __BC23_BASM
#define __BC23_BASM

#include "def.h"

extern const char *OPCODES[];

enum TokenType {
    LABEL,
    ADDRESS,
    LITERAL,
    ARGSEP,
    REGISTER,
    OP_LDA, OP_LDX, OP_LDY, 
    OP_STA, OP_STX, OP_STY, 
    OP_INC, OP_DEC, 
    OP_ADD, OP_SUB, OP_CMP, 
    OP_JMP, OP_JC, OP_JNC, OP_JZ, OP_JNZ, OP_JPE, OP_JPO, OP_JM, OP_JP, 
    OP_CALL, OP_CC, OP_CNC, OP_CZ, OP_CNZ, OP_CPE, OP_CPO, OP_CM, OP_CP, 
    OP_RET, OP_RC, OP_RNC, OP_RZ, OP_RNZ, OP_RPE, OP_RPO, OP_RM, OP_RP,
    OP_PUSH, OP_POP,
    SPECIAL_END
};
#define NUM_OPCODES 40

enum TokenType match_opcode(char *token);

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

enum ArgumentType {
    ARG_REGISTER, ARG_ADDRESS, ARG_LITERAL, ARG_LABEL, ARG_NONE
};
enum Register {
    REGISTER_A, REGISTER_X, REGISTER_Y
};
struct Argument {
    enum ArgumentType type;
    int value;
};
struct Instruction {
    enum TokenType instruction;
    struct Argument arg1;
    struct Argument arg2;
};

#define BASM_STACK_SIZE         32
#define BASM_CALL_STACK_SIZE    32
#define BASM_CODE_SIZE          4096

#define BASM_FLAG_C             0b0001
#define BASM_FLAG_P             0b0010
#define BASM_FLAG_S             0b0100
#define BASM_FLAG_Z             0b1000

struct BASMProcessor {
    char stack[BASM_STACK_SIZE];
    size_t call_stack[BASM_CALL_STACK_SIZE];
    struct Instruction code[BASM_CODE_SIZE];
    char flags;
    char a;
    char x;
    char y;
};
struct BASMAssembleOutput {
    struct BASMProcessor *processor;
    char *error;
};

struct BASMAssembleOutput basm_assemble(struct Token *token_stream, size_t token_count);

void basm_execute(struct BASMProcessor *processor);

struct BASMParseOutput basm_parse(char *program);

#define BASM_HIGHLIGHT_LABEL    TEXT_CYAN
#define BASM_HIGHLIGHT_OPCODE   TEXT_PINK
#define BASM_HIGHLIGHT_NUMBER   TEXT_ORANGE

// can you believe it was so long i needed to split it
#define IS_OPCODE OP_LDA: case OP_LDX: case OP_LDY: case OP_STA: case OP_STX: case OP_STY: case OP_INC: case OP_DEC: case OP_ADD: case OP_SUB: case OP_CMP: case OP_JMP: case OP_JC: case OP_JNC: case OP_JZ: case OP_JNZ: case OP_JPE: case OP_JPO: case OP_JM: case OP_JP: case OP_CALL: case OP_CC: case OP_CNC: case OP_CZ: case OP_CNZ: case OP_CPE: case OP_CPO: case OP_CM: case OP_CP: case OP_RET: case OP_RC: case OP_RNC: case OP_RZ: case OP_RNZ: case OP_RPE: case OP_RPO: case OP_RM: case OP_RP 
#define IS_OPCODE2 OP_PUSH: case OP_POP

#endif