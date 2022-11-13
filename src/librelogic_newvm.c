/*
 ============================================================================
 Name        : librelogic_newvm.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "librelogic_newvm.h"

#define EP(x) [x] = #x

#define MAX_PROGRAM_LEN  65535
uint32_t vm_program[MAX_PROGRAM_LEN];
uint32_t pc = 0;

const char *il_commands_str[32] = {
        "NOP", // 0x00
        "LD",  // 0x01
        "ST",  // 0x02
        "S",   // 0x03
        "R",   // 0x04
        "AND", // 0x05
        "OR",  // 0x06
        "XOR", // 0x07
        "NOT", // 0x08
        "ADD", // 0x09
        "SUB", // 0x0a
        "MUL", // 0x0b
        "DIV", // 0x0c
        "GT",  // 0x0d
        "GE",  // 0x0e
        "EQ",  // 0x0f
        "NE",  // 0x10
        "LE",  // 0x11
        "LT",  // 0x12
        "JMP", // 0x13
        "CAL", // 0x14
        "POP", // 0x15
        "N16", // 0x16
        "N17", // 0x17
        "N18", // 0x18
        "N19", // 0x19
        "N1A", // 0x1a
        "N1B", // 0x1b
        "N1C", // 0x1c
        "N1D", // 0x1d
        "N1E", // 0x1e
        "N1F", // 0x1f
};

const char *IlOperands[17] = {
        "i",  // 0x00
        "if", // 0x01
        "f",  // 0x02
        "r",  // 0x03
        "m",  // 0x04
        "mf", // 0x05
        "c",  // 0x06
        "b",  // 0x07
        "t",  // 0x08
        "q",  // 0x09
        "qf", // 0x0a
        "Q",  // 0x0b
        "QF", // 0x0c
        "T",  // 0x0d
        "M",  // 0x0e
        "MF", // 0x0f
        "W",  // 0x10
};

typedef struct instr {
    uint32_t instr;
    bool cond;
    bool neg_ins;
    bool push;
    bool neg_arg;
    bool word;
    uint8_t il;
    uint8_t operand;
    uint8_t arg_byte;
    uint8_t arg_bit;
    uint16_t arg_word;
    uint32_t jmp_line;
} instr_t;

instr_t decode_instruction(uint32_t instr) {
    instr_t result;

    result.instr = instr;
    result.il = IL(instr);
    result.operand = OPERAND(instr);
    result.cond = COND(instr);
    result.neg_ins = NEGATE_INS(instr);
    result.push = PUSH(instr);
    result.neg_arg = NEGATE_ARG(instr);
    result.word = WORD(instr);
    result.arg_byte = INSBYTE2(instr);
    result.arg_bit = INSBYTE3(instr);
    result.arg_word = INSWORD0(instr);
    result.jmp_line = INSWORD2(instr);

    return result;
}

void dump_instr(uint32_t instr) {
    instr_t result = decode_instruction(instr);

    printf("%s", il_commands_str[result.il]);
    if (result.neg_ins)
        printf("!");
    if (result.cond)
        printf("?");
    if (result.push)
        printf("(");

    printf(" ");
    if (IL(instr) == IL_JMP) {
        printf("%lu", (long unsigned int) result.jmp_line);
    } else {
        if (result.neg_arg)
            printf("!");
        printf("%%%s", IlOperands[result.operand]);

        if (result.word)
            printf("%d", result.arg_word);
        else
            printf("%d/%d", result.arg_byte, result.arg_bit);
    }
}

/////////////// compiler //////////////////
typedef struct label {
    char label[512];
    uint32_t line;
} label_t;

bool isBlank(char *line) {
    char *ch;
    bool is_blank = true;

    for (ch = line; *ch != '\0'; ++ch) {
        if (!isspace(*ch)) {
            is_blank = false;
            break;
        }
    }

    return is_blank;
}

char* strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if (*sub && (q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            memmove(q, p, r - p);
            q += r - p;
        }
        memmove(q, p, strlen(p) + 1);
    }
    return str;
}

char* ltrim(char *s) {
    while (isspace(*s))
        s++;
    return s;
}

char* rtrim(char *s) {
    char *back = s + strlen(s);
    while (isspace(*--back))
        ;
    *(back + 1) = '\0';
    return s;
}

char* trim(char *s) {
    return rtrim(ltrim(s));
}

void strupp(char *beg) {
    while ((*beg++ = toupper(*beg)))
        ;
}

void to_binary(uint32_t num, char *binary) {
    if (num == 0) {
        printf("0");
        return;
    }

    // Stores binary representation of number.
    int binaryNum[32]; // Assuming 32 bit integer.
    int i = 0;

    for (; num > 0;) {
        binaryNum[i++] = num % 2;
        num /= 2;
    }

    strcat(binary, "[");
    // Printing array in reverse order.
    for (int j = 31; j >= 0; j--) {
        strcat(binary, binaryNum[j] ? "1" : "0");
        if (!(j % 8))
            strcat(binary, "][");
    }
    binary[strlen(binary) - 1] = '\0';
}

void compile_il(char *file) {
    uint32_t code;
    FILE *f;
    char line[512];
    char ln_ins[2][50] = {"", ""};
    char *ln = NULL;
    char *ptr;
    int index, pc = 0, pos = 0, labels_qty;
    label_t *labels;
    bool mod_cond = false;
    bool mod_neg_ins = false;
    bool mod_push = false;
    bool mod_neg_arg = false;
    bool word = false;
    uint32_t label_line = 0;
    uint8_t operand = 0;
    uint8_t arg_byte = 0, arg_bit = 0;
    uint16_t arg_word = 0;
    uint8_t instr = 0;

    labels = malloc(sizeof(label_t));
    labels_qty = 0;
    f = fopen(file, "r");
    if (f == NULL) {
        printf("Error: can't open file\n");
        exit(1);
    }

// first pass: localize labels
    printf("labels:\n");
    while (fgets(line, 512, f)) {
        if(isBlank(line))
            continue;

        ptr = strchr(line, ':');
        pc++;
        if (ptr == NULL)
            continue;

        index = ptr - line;
        memcpy(labels[pos].label, line, index);
        labels_qty++;
        labels[pos].line = pc;
        printf("  [%04d] (%s)\n", labels[pos].line, labels[pos].label);
        ++pos;

        labels = realloc(labels, (pos + 1) * sizeof(label_t));
    }
    printf("\n");
    rewind(f);
    pc = 0;

// second pass: compile
    printf("program:\n");
    while (fgets(line, 512, f)) {
        if (isBlank(line))
            continue;
        pc++;
        code = 0;

        // erase label
        ptr = strchr(line, ':');
        if (ptr != NULL) {
            index = ptr - line;
            memcpy(line, ptr + 1, strlen(line) - index);
        }

        // erase comment
        ptr = strchr(line, ';');
        if (ptr != NULL) {
            index = ptr - line;
            line[index] = '\0';
        }

        ln = trim(line);
        printf("  [%04d] %s\n", pc, ln);

        index = 0;
        ptr = strtok(ln, " ");
        while (ptr != NULL) {
            strcpy(ln_ins[index++], ptr);
            ptr = strtok(NULL, " ");
        }

        mod_cond = false;
        mod_neg_ins = false;
        mod_push = false;
        mod_neg_arg = false;

        instr = 255;

        strupp(ln_ins[0]);
        ptr = strchr(ln_ins[0], '!');
        if (ptr != NULL) {
            mod_neg_ins = true;
            index = ptr - ln_ins[0];
            ln_ins[0][index] = ' ';
        }

        ptr = strchr(ln_ins[0], '(');
        if (ptr != NULL) {
            mod_push = true;
            index = ptr - ln_ins[0];
            ln_ins[0][index] = ' ';
        }

        ptr = strchr(ln_ins[0], '?');
        if (ptr != NULL) {
            mod_cond = true;
            index = ptr - ln_ins[0];
            ln_ins[0][index] = ' ';
        }

        ln = trim(ln_ins[0]);
        for (index = 0; index < 32; index++) {
            if (!strcmp(ln, il_commands_str[index])) {
                instr = index;
                break;
            }
        }
        if (instr == 255) {
            printf("ERROR: unknown instruction\n");
            return;
        }
        printf("    instr: %d(%s) / neg: %d / cond: %d / push:%d\n", instr, il_commands_str[instr], mod_neg_ins, mod_cond, mod_push);

        label_line = 0;
        bool lb = false;
        if (instr == IL_JMP) {
            for (index = 0; index < labels_qty + 1; index++) {
                ptr = NULL;
                ptr = strstr(ln_ins[1], labels[index].label);
                if (ptr != NULL) {
                    label_line = labels[index].line;
                    lb = true;
                    break;
                }
            }
            if (!lb) {
                printf("ERROR: label (%s) not found\n", ln_ins[1]);
                return;
            }
            printf("    JMP: (%s) [%04lu]\n", labels[index].label, (long unsigned int) label_line);
        } else {
            ptr = strchr(ln_ins[1], '!');
            if (ptr != NULL) {
                mod_neg_arg = true;
                index = ptr - ln_ins[1];
                ln_ins[1][index] = ' ';
            }

            ptr = strchr(ln_ins[1], '%');
            if (ptr == NULL) {
                printf("ERROR: bad argument\n");
                return;
            }
            index = ptr - ln_ins[1];
            ln_ins[1][index] = ' ';

            ln = trim(ln_ins[1]);
            operand = 255;
            for (index = 0; index < 16; index++) {
                ptr = strstr(ln, IlOperands[index]);
                if (ptr) {
                    operand = index;
                    break;
                }
            }
            if(operand == 255) {
                printf("ERROR: unknown operand\n");
                return;
            }
            ln = ptr + strlen(IlOperands[operand]);
            printf("    operand type: %d (%s) neg: %d\n", operand, IlOperands[operand], mod_neg_arg);

            word = false;
            arg_byte = 0;
            arg_bit = 0;
            arg_word = 0;

            ptr = strchr(ln, '/');
            if (ptr != NULL) {
                index = 0;
                ptr = strtok(ln, "/");
                while (ptr != NULL) {
                    strcpy(ln_ins[index++], ptr);
                    ptr = strtok(NULL, "/");
                }

                arg_byte = atoi(ln_ins[0]);
                arg_bit = atoi(ln_ins[1]);
                printf("    arg: (byte: %d / bit: %d)\n", arg_byte, arg_bit);
            } else {
                word = true;
                arg_word = atoi(ln);
                printf("    arg: (word: %d)\n", arg_word);
            }
        }

        // create op
        SET_IL(code, instr);
        if (mod_cond)
            SET_COND(code);
        if (mod_neg_ins)
            SET_NEGATE_INS(code);

        if (instr == IL_JMP) {
            SET_INSWORD2_VAL(code, label_line);
        } else {
            SET_OPERAND(code, operand);
            if (mod_push)
                SET_PUSH(code);
            if (mod_neg_arg)
                SET_NEGATE_ARG(code);
            if (word) {
                SET_WORD(code);
                SET_INSWORD0_VAL(code, arg_word);
            } else {
                SET_BYTE_VAL(code, arg_byte);
                SET_BIT_VAL(code, arg_bit);
            }
        }

        printf("    OP: 0x%08x\n", code);
        char bin[50] = "";
        to_binary(code, bin);
        printf("           [        INSWORD2 (25)          ]\n");
        printf("                 [      INSWORD1 (21)      ]\n");
        printf("                        [   INSWORD0 (16)  ]\n");
        printf("    [INSBYTE0][INSBYTE1][INSBYTE2][INSBYTE3]\n");
        printf("    [IIIIICNP][RWGOOOOO][BBBBBBBB][TTTTTTTT]\n");
        printf("    %s\n", bin);
        printf(" DECODE INSTR: ");
        dump_instr(code);
        printf("\n-----------\n");
    }

}

////////////////////////// VM /////////////////////////////
uint8_t vm_execute() {
    uint8_t status = 0;

    static void *dispatch_vm[] = {
            &&_IL_NOP, &&_IL_LD,
            &&_IL_ST,  &&_IL_S,
            &&_IL_R,   &&_IL_AND,
            &&_IL_OR,  &&_IL_XOR,
            &&_IL_NOT, &&_IL_ADD,
            &&_IL_SUB, &&_IL_MUL,
            &&_IL_DIV, &&_IL_GT,
            &&_IL_GE,  &&_IL_EQ,
            &&_IL_NE,  &&_IL_LE,
            &&_IL_LT,  &&_IL_JMP,
            &&_IL_CAL, &&_IL_HALT
    };
#define DISPATCH() goto *dispatch_vm[IL(vm_program[pc++])]

    DISPATCH();
    ////////////////////
    _IL_NOP:
    //

    DISPATCH();

    _IL_LD:
    //

    DISPATCH();

    _IL_ST:
    //

    DISPATCH();

    _IL_S:
    //

    DISPATCH();

    _IL_R:
    //

    DISPATCH();

    _IL_AND:
    //

    DISPATCH();

    _IL_OR:
    //

    DISPATCH();

    _IL_XOR:
    //

    DISPATCH();

    _IL_NOT:
    //

    DISPATCH();

    _IL_ADD:
    //

    DISPATCH();

    _IL_SUB:
    //

    DISPATCH();

    _IL_MUL:
    //

    DISPATCH();

    _IL_DIV:
    //

    DISPATCH();

    _IL_GT:
    //

    DISPATCH();

    _IL_GE:
    //

    DISPATCH();

    _IL_EQ:
    //

    DISPATCH();

    _IL_NE:
    //

    DISPATCH();

    _IL_LE:
    //

    DISPATCH();

    _IL_LT:
    //

    DISPATCH();

    _IL_JMP:
    //

    DISPATCH();

    _IL_CAL:
    //

    DISPATCH();

    _IL_HALT:
    //
    return status;
}

int main(void) {
    compile_il("test2.il");

    return EXIT_SUCCESS;
}
