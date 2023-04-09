#include "basm/basm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Token get_token(struct Token *token_stream, size_t token_count, size_t *index) {
    if((*index)++ == token_count) {
        return (struct Token){
            SPECIAL_END, 0, NULL
        };
    } else {
        return token_stream[*index-1];
    }
}

void print_instruction(struct Instruction instruction) {
    char *arg1type;
    char *arg2type;
    switch(instruction.arg1.type) {
        case ARG_REGISTER:
            arg1type = "Register";
            break;
        case ARG_ADDRESS:
            arg1type = "Address";
            break;
        case ARG_LITERAL:
            arg1type = "Literal";
            break;
        case ARG_LABEL:
            arg1type = "Label";
            break;
        case ARG_NONE:
            arg1type = "None";
    }
    switch(instruction.arg2.type) {
        case ARG_REGISTER:
            arg2type = "Register";
            break;
        case ARG_ADDRESS:
            arg2type = "Address";
            break;
        case ARG_LITERAL:
            arg2type = "Literal";
            break;
        case ARG_LABEL:
            arg2type = "Label";
            break;
        case ARG_NONE:
            arg2type = "None";
    }
    

    printf("%s: %s (%d), %s (%d)\n",
        OPCODES[instruction.instruction - OP_LDA],
        arg1type, instruction.arg1.value,
        arg2type, instruction.arg2.value  
    );
}

struct BASMAssembleOutput basm_assemble(struct Token *token_stream, size_t token_count) {
    struct BASMAssembleOutput output;
    struct BASMProcessor *processor = malloc(sizeof(struct BASMProcessor));
    output.processor = processor;
    output.error = NULL;

    size_t index = 0;
    struct Token token;

    struct Instruction currentInstruction;
    currentInstruction.instruction = 0;
    currentInstruction.arg1.type = ARG_NONE;
    currentInstruction.arg2.type = ARG_NONE;

    size_t instruction_count = 0;

    char labels[32][128];
    char label_locations[32];
    size_t label_count = 0;

    while((token = get_token(token_stream, token_count, &index)).type != SPECIAL_END) {
        switch(token.type) {
            case IS_OPCODE: case IS_OPCODE2:
                if(currentInstruction.instruction == 0) { // ok to start a new instruction
                    currentInstruction.instruction = token.type;
                } else {
                    switch(currentInstruction.instruction) {
                        // ZERO ARGUMENT
                        case OP_INC: 
                        case OP_DEC: 
                        case OP_RET: 
                        case OP_RC: 
                        case OP_RNC: 
                        case OP_RZ: 
                        case OP_RNZ:
                        case OP_RPE:
                        case OP_RPO:
                        case OP_RM:
                        case OP_RP:
                            processor->code[instruction_count++] = currentInstruction;
                            currentInstruction.instruction = token.type;
                            currentInstruction.arg1.type = ARG_NONE;
                            currentInstruction.arg2.type = ARG_NONE;
                            break;
                        // ONE LABEL ARGUMENT
                        case OP_JMP:
                        case OP_JC:
                        case OP_JNC:
                        case OP_JZ:
                        case OP_JNZ:
                        case OP_JPE:
                        case OP_JPO:
                        case OP_JM:
                        case OP_JP:
                        case OP_CALL:
                        case OP_CC:
                        case OP_CNC:
                        case OP_CZ:
                        case OP_CNZ:
                        case OP_CPE:
                        case OP_CPO:
                        case OP_CM:
                        case OP_CP:
                            if(currentInstruction.arg2.type != ARG_NONE) {
                                return (struct BASMAssembleOutput){ NULL, "Opcode takes 1 input, 2 given"};
                            }
                            if(currentInstruction.arg1.type != ARG_LABEL) {
                                return (struct BASMAssembleOutput){ NULL, "Opcode takes 1 label input"};
                            }
                            processor->code[instruction_count++] = currentInstruction;
                            currentInstruction.instruction = token.type;
                            currentInstruction.arg1.type = ARG_NONE;
                            currentInstruction.arg2.type = ARG_NONE;
                            break;
                        // ONE OR TWO
                        case OP_LDA:
                        case OP_LDX:
                        case OP_LDY:
                        case OP_STA:
                        case OP_STX:
                        case OP_STY:
                        case OP_ADD:
                        case OP_SUB:
                        case OP_CMP:
                            if(currentInstruction.arg2.type == ARG_LABEL) {
                                return (struct BASMAssembleOutput){ NULL, "Cannot use label as offset"};
                            }
                            processor->code[instruction_count++] = currentInstruction;
                            currentInstruction.instruction = token.type;
                            currentInstruction.arg1.type = ARG_NONE;
                            currentInstruction.arg2.type = ARG_NONE;
                            break;
                    }
                }
                break;
            case LABEL: //TODO: debug why the labels are weird, fix registers
                int match = -1;
                for(size_t label_index = 0; label_index < label_count; label_index++) {
                    if(strcmp(token.metadata, labels[label_index]) == 0) {
                        match = label_index;
                        break;
                    }
                }
                if(match == -1) {
                    strcpy(labels[label_count++], token.metadata);
                    match = label_count;
                }
                if(currentInstruction.instruction >= OP_JMP && currentInstruction.instruction <= OP_CP && currentInstruction.arg1.type == ARG_NONE) {
                    currentInstruction.arg1.type = ARG_LABEL;
                    currentInstruction.arg1.value = match;
                } else {
                    label_locations[match] = instruction_count;
                }
                break;
            case ADDRESS:
                if(currentInstruction.instruction == 0) {
                    return (struct BASMAssembleOutput){ NULL, "Stray address"};
                }
                if(currentInstruction.arg1.type == ARG_NONE) {
                    currentInstruction.arg1.type = ARG_ADDRESS;
                    currentInstruction.arg1.value = token.literal;
                } else {
                    currentInstruction.arg2.type = ARG_ADDRESS;
                    currentInstruction.arg2.value = token.literal;
                }
                break;
            case LITERAL:
                if(currentInstruction.instruction == 0) {
                    return (struct BASMAssembleOutput){ NULL, "Stray literal"};
                }
                if(currentInstruction.arg1.type == ARG_NONE) {
                    currentInstruction.arg1.type = ARG_LITERAL;
                    currentInstruction.arg1.value = token.literal;
                } else {
                    currentInstruction.arg2.type = ARG_LITERAL;
                    currentInstruction.arg2.value = token.literal;
                }
                break;
            case REGISTER:
                if(currentInstruction.instruction == 0) {
                    return (struct BASMAssembleOutput){ NULL, "Stray register"};
                }
                if(currentInstruction.arg1.type == ARG_NONE) {
                    currentInstruction.arg1.type = ARG_REGISTER;
                    currentInstruction.arg1.value = token.literal;
                } else {
                    currentInstruction.arg2.type = ARG_REGISTER;
                    currentInstruction.arg2.value = token.literal;
                }
                break;
            case ARGSEP:
                continue;
        }
    }
    processor->code[instruction_count++] = currentInstruction;
    for(size_t i = 0; i < instruction_count; i++) {
        print_instruction(processor->code[i]);
    }

    return output;
}

void basm_execute(struct BASMProcessor *processor) {}