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
    "sty",
    "inc",
    "dec",
    "add",
    "sub",
    "cmp",
    "jmp",
    "jc",
    "jnc",
    "jz",
    "jnz",
    "jpe",
    "jpo",
    "jm",
    "jp",
    "call",
    "cc",
    "cnc",
    "cz",
    "cnz",
    "cpe",
    "cpo",
    "cm",
    "cp",
    "ret",
    "rc",
    "rnc",
    "rz",
    "rnz",
    "rpe",
    "rpo",
    "rm",
    "rp",
    "push",
    "pop"
};
const char PUNCTUATION[] = {
    ',',';','.'
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
    if(is_punctuation(line[i])) {
        strncpy(out, line+i, 1);
        return 1;
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
    if(token_len == 1) {
        if(token[0] == 'a') {
            append_token((struct Token) {
                REGISTER, 0, ""
            }, token_list, token_count);
            return NULL;
        } else if(token[0] == 'x') {
            append_token((struct Token) {
                REGISTER, 1, ""
            }, token_list, token_count);
            return NULL;
        } else if(token[0] == 'y') {
            append_token((struct Token) {
                REGISTER, 2, ""
            }, token_list, token_count);
            return NULL;
        }
    }
    if(token_len == 1 && token[0] == ',') {
        append_token((struct Token) {
            ARGSEP, 0, ""
        }, token_list, token_count);
        return NULL;
    }
    if(token[token_len-1] == ':') { // potentially a label
        if(token[0] == '#') {
            char *error = malloc(2048*sizeof(char));
            sprintf(error, "Cannot have reserved character # in \nlabel %s", token);
            return error;
        }
        char *candidate = malloc((token_len)*sizeof(char));
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
    if(token[0] == 'x') { // hex
        int hex = 0;
        for(size_t i = 1; i < token_len; i++) {
            hex *= 16;
            char c = token[i];
            if('0' <= c && c <= '9') {
                hex += c-'0';
            } else if('a' <= c && c <= 'f') {
                hex += 10+c-'a';
            } else {
                char *error = malloc(2048*sizeof(char));
                sprintf("Invalid hex literal %s", token);
            }
        }
        append_token((struct Token) {
            LITERAL, hex, ""
        }, token_list, token_count);
        return NULL;
    }
    if(token[0] == 'b') { // binary
        int bin = 0;
        for(size_t i = 1; i < token_len; i++) {
            bin *= 2;
            char c = token[i];
            if(c == '0') {
                bin += 0;
            } else if (c == '1') {
                bin += 1;
            } else {
                char *error = malloc(2048*sizeof(char));
                sprintf("Invalid binary literal %s", token);
            }
        }
        append_token((struct Token) {
            LITERAL, bin, ""
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
        int hex = 0;
        for(size_t i = 1; i < token_len; i++) {
            hex *= 16;
            char c = token[i];
            if('0' <= c && c <= '9') {
                hex += c-'0';
            } else if('a' <= c && c <= 'f') {
                hex += 10+c-'a';
            } else {
                char *error = malloc(2048*sizeof(char));
                sprintf("Invalid hex address %s", token);
            }
        }
        append_token((struct Token) {
            ADDRESS, hex, ""
        }, token_list, token_count);
        return NULL;
    }
    if(token[0] == '"' && token[token_len-1] == '"') { // lord no one do anything stupid
        char *outstr = malloc(token_len*sizeof(char));
        strcpy(outstr, token);
        append_token((struct Token) {
            LITERAL, 0, outstr
        }, token_list, token_count);
        return NULL;
    }

    char *label = malloc((token_len+1)*sizeof(char));
    strcpy(label, token);
    append_token((struct Token) {
        LABEL, 0, label
    }, token_list, token_count);
    return NULL;
}

struct BASMParseOutput basm_parse(char *program) {
    struct BASMParseOutput output = {
        malloc(8192*sizeof(char)), malloc(512*sizeof(struct Token)), 0, NULL
    };
    memset(output.syntax_highlighted, 0, 8192);

    size_t i = 0;
    size_t file_t;
    int line_number = 0;
    char *line_number_text = malloc(16*sizeof(char));

    char line[4096];
    char catbuffer[4096];
    while((file_t = _basm_get_line(program, line, i)) > 0) {
        sprintf(line_number_text, TEXT_GREY "%02d" TEXT_WHITE "    ", line_number++);
        strcat(output.syntax_highlighted, line_number_text);
        char token[4096];
        size_t line_t;
        size_t j = 0;
        while((line_t = _basm_get_token(line, token, j)) > 0) {
            char *error = parse_token(token, output.token_stream, &output.token_count);
            if(error != NULL) {
                output.error = error;
                return output;
            }
            switch(output.token_stream[output.token_count-1].type) {
                case LABEL:
                    strcat(output.syntax_highlighted, BASM_HIGHLIGHT_LABEL);
                    break;
                case ADDRESS:
                case LITERAL:
                    strcat(output.syntax_highlighted, BASM_HIGHLIGHT_NUMBER);
                    break;
                case IS_OPCODE:
                    strcat(output.syntax_highlighted, "  " BASM_HIGHLIGHT_OPCODE);
                    break;
            }
            strcat(output.syntax_highlighted, token);
            switch(output.token_stream[output.token_count-1].type) {
                case IS_OPCODE: case IS_OPCODE2:
                    strcat(output.syntax_highlighted, TEXT_WHITE " ");
                    break;
                default:
                    strcat(output.syntax_highlighted, TEXT_WHITE);
            }
            
            j += line_t;
            memset(token, 0, 4096);
        }
        strcat(output.syntax_highlighted, "\n");

        i += file_t;
        memset(line, 0, 4096);
    }
    return output;
}