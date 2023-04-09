#include "basm/basm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *OPCODES[] = {
    "lda",
    "ldx",
    "ldy",
    "sta",
    "stx",
    "sty"
};
#define NUM_OPCODES 6
const char PUNCTUATION[] = {
    ',', '\n'
};
#define NUM_PUNCTUATION 3

enum TokenType match_opcode(char *token) {
    for(int i = 0; i < NUM_OPCODES; i++) {
        if(strcmp(OPCODES[i], token) == 0) {
            return OP_LDA + i;
        }
    }
    return -1;
}

int is_punctuation(char c) {
    int condition = 0;
    for(int i = 0; i < NUM_PUNCTUATION; i++) {
        condition = (condition || c == PUNCTUATION[i]);
    }
    return condition;
}

size_t _basm_get_token(char *line, char *out, size_t i) {
    int advance = 0;
    while(line[i] == ' ') {
        advance++;
        i++;
    }
    size_t j = i;
    while(line[j] != '\0' && line[j] != ' ' && !is_punctuation(line[j])) {
        advance++;
        j++;
    }

    strncpy(out, line+i, j-i);
    return (j == i ? 0 : advance);
}

size_t _basm_get_line(char *program, char *out, size_t i) {
    int advance = 0;
    while(program[i] == ' ' || program[i] == '\n') {
        advance++;
        i++;
    }
    size_t j = i;
    while(program[j] != '\0' && program[j] != '\n') {
        advance++;
        j++;
    }

    strncpy(out, program+i, j-i);
    return (j == i ? 0 : advance);
}

void append_token(struct Token token, struct Token *token_list, size_t *token_count) {
    token_list[(*token_count)++] = token;
}

char *parse_token(char *token, struct Token *token_list, size_t *token_count) {
    size_t token_len = strlen(token);
    if(token[token_len-1] == ':') { // potentially a label
        if(token[0] == '#') {
            char *error = malloc(2048*sizeof(char));
            sprintf(error, "Cannot have reserved character # in label %s", token);
            return error;
        }
        char *candidate = malloc((token_len-1)*sizeof(char));
        strncpy(candidate, token, token_len-1);
        if(match_opcode(candidate) != -1) { // reserved keyword
            char *error = malloc(2048*sizeof(char));
            sprintf(error, "Invalid label %s", candidate);
            free(candidate);
            return error;
        }
        append_token((struct Token) {
            LABEL, 0, candidate
        }, token_list, token_count);
        return NULL;
    } 
    int opcode = match_opcode(token);
    if(opcode > -1) {
        append_token((struct Token) {
            opcode, 0, ""
        }, token_list, token_count);
        return NULL;
    }
    int literal = atoi(token);
    if(literal != 0 || (literal == 0 && token[0] == '0')) {
        append_token((struct Token) {
            LITERAL, literal, ""
        }, token_list, token_count);
        return NULL;
    }
    if(token[0] == '#') { // potentially an address
        if(token_len == 1) {
            return "Empty address not allowed";
        }
        char *candidate = malloc((token_len-1)*sizeof(char));
        strncpy(candidate, token+1, token_len-1);
        int address = atoi(candidate);
        if(address != 0 || (address == 0 && candidate[0] == '0')) {
            append_token((struct Token) {
                ADDRESS, address, ""
            }, token_list, token_count);
            return NULL;
        }
    }
    if(token[0] == '"' && token[token_len-1] == '"') { // lord no one do anything stupid
        char *outstr = malloc(token_len*sizeof(char));
        strcpy(outstr, token);
        append_token((struct Token) {
            LITERAL, 0, outstr
        }, token_list, token_count);
        return NULL;
    }

    return NULL;
}

struct BASMParseOutput basm_parse(char *program) {
    struct BASMParseOutput output = {
        malloc(8192*sizeof(char)), malloc(512*sizeof(struct Token)), 0, NULL
    };
    memset(output.syntax_highlighted, 0, 8192);

    size_t i = 0;
    size_t file_t;

    char line[4096];
    char catbuffer[4096];
    while((file_t = _basm_get_line(program, line, i)) > 0) {
        char token[4096];
        size_t line_t;
        size_t j = 0;
        while((line_t = _basm_get_token(line, token, j)) > 0) {
            char *error = parse_token(token, output.token_stream, &output.token_count);
            if(error != NULL) {
                output.error = error;
                return output;
            }
            // strncat(output.syntax_highlighted, program+i+j, line_t+1);
            // printf("copied %zd bytes for token %s\n", line_t+1, token);
            
            // printf("%s %d\n", token, output.token_stream[output.token_count-1].type, output.token_count);

            j += line_t;
            memset(token, 0, line_t);
        }

        i += file_t;
        memset(line, 0, file_t);
    }
    strcat(output.syntax_highlighted, program+i);
    
    // char line[4096];
    // char token[4096];
    // size_t length = _basm_get_token(program, token, i);
    // printf("%s (%zd characters)\n", token, length);
    return output;
}