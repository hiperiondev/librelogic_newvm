/*******************************************************************************
 LibreLogic : a free PLC library
 Copyright (C) 2022, Antonis K. (kalamara AT ceid DOT upatras DOT gr)

 New VM: 2022 Emiliano Augusto Gonzalez
 ( egonzalez . hiperion @ gmail . com )

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBRELOGIC_NEWVM_H_
#define LIBRELOGIC_NEWVM_H_

#include <stdint.h>

// VM INSTR
//        [        INSWORD2 (25)          ]
//              [      INSWORD1 (21)      ]
//                     [   INSWORD0 (16)  ]
// [INSBYTE0][INSBYTE1][INSBYTE2][INSBYTE3]
// [IIIIICNP][RWGOOOOO][BBBBBBBB][TTTTTTTT]
// I: il instruction
// C: conditional
// N: negate instruction
// P: push
// R: return
// W: 1: word/ 0:byte:bit
// G: negate argument
// O: operand
// B: byte
// T: bit

#define INSBYTE0(x)        ((x & 0xFF000000) >> 24)
#define INSBYTE1(x)        ((x & 0x00FF0000) >> 16)
#define INSBYTE2(x)        ((x & 0x0000FF00) >> 8)
#define INSBYTE3(x)        (x & 0xFF)
#define INSWORD0(x)        (x & 0xFFFF)
#define INSWORD1(x)        (x & 0x1FFFFF)
#define INSWORD2(x)        (x & 0x1FFFFFF)

#define IL(x)              (x >> 27)
#define OPERAND(x)         ((x & 0x1F0000) >> 16)
#define BIT_COND(x)        (x & 0x4000000)
#define BIT_NEGATE_INS(x)  (x & 0x2000000)
#define BIT_PUSH(x)        (x & 0x1000000)
#define BIT_RETURN(x)      (x & 0x800000)
#define BIT_WORD(x)        (x & 0x400000)
#define BIT_NEGATE_ARG(x)  (x & 0x200000)

#define SET_IL(i, v)       (i = (i | (v << 27)))
#define SET_OPERAND(i, v)  (i = (i | (v << 16)))
#define SET_COND(i)        (i = (i | 0x4000000))
#define SET_NEGATE_INS(i)  (i = (i | 0x2000000))
#define SET_PUSH(i)        (i = (i | 0x1000000))
#define SET_RETURN(i)      (i = (i | 0x800000))
#define SET_WORD(i)        (i = (i | 0x400000))
#define SET_NEGATE_ARG(i)  (i = (i | 0x200000))
#define SET_INSWORD0_VAL(i, v) (i = (i | (v & 0xFFFF)))
#define SET_BIT_VAL(i, v) (i = (i | (v & 0xFF)))
#define SET_BYTE_VAL(i, v)  (i = (i | ((v & 0xff) << 8)))
#define SET_INSWORD2_VAL(i, v) (i = (i | INSWORD2(v)))

typedef enum IL_COMMANDS {
//   instr  //       | modifiers |  description
    IL_NOP, //  0x00 |           |  Not operation
    IL_LD,  //  0x01 |     N     |  Loads the (negated) the value of the operand into the accumulator.
    IL_ST,  //  0x02 |     N     |  Stores the (negated) content of the accumulator in the operand.
    IL_S,   //  0x03 |           |  Sets the operand (type BOOL) to TRUE if the content of the accumulator is TRUE.
    IL_R,   //  0x04 |           |  Sets the operand (type BOOL) to FALSE if the content of the accumulator is TRUE.
    IL_AND, //  0x05 |    N,(    |  Bitwise AND of the accumulator value and (negated) operand.
    IL_OR,  //  0x06 |    N,(    |  Bitwise OR of the accumulator value and (negated) operand.
    IL_XOR, //  0x07 |    N,(    |  Bitwise exclusive OR of the accumulator value and (negated) operand.
    IL_NOT, //  0x08 |           |  Bitwise negation of the accumulator value.
    IL_ADD, //  0x09 |     (     |  Addition of the accumulator value and the operand; result is written into the accumulator.
    IL_SUB, //  0x0a |     (     |  Subtraction of the operand from the accumulator value; result is written into the accumulator.
    IL_MUL, //  0x0b |     (     |  Multiplication of accumulator value and operand; result is written into the accumulator.
    IL_DIV, //  0x0c |     (     |  Division of the accumulator value by the operand; result is written into the accumulator.
    IL_GT,  //  0x0d |     (     |  Checks whether the accumulator value is greater than the operand value; write result (BOOL) into the accumulator.
    IL_GE,  //  0x0e |     (     |  Checks whether the accumulator value is greater than or equal to the operand value; write result (BOOL) into the accumulator.
    IL_EQ,  //  0x0f |     (     |  Checks whether the accumulator value is equal to the operand value; write result (BOOL) into the accumulator.
    IL_NE,  //  0x10 |     (     |  Checks whether the accumulator value is not equal to the operand value; write result (BOOL) into the accumulator.
    IL_LE,  //  0x11 |     (     |  Checks whether the accumulator value is smaller than or equal to the operand value; write result (BOOL) into the accumulator.
    IL_LT,  //  0x12 |     (     |  Checks whether the accumulator value is smaller than the operand value; result (BOOL) is written into the accumulator.
    IL_JMP, //  0x13 |    CN     |  Unconditional (conditional) jump to the specified jump label.
    IL_CAL, //  0x14 |    CN     |  (Conditional) call of a program or a function block (if the accumulator value is TRUE).
    IL_POP, //  0x15 |           |  not defined.
    IL_16,  //  0x16 |           |  not defined.
    IL_17,  //  0x17 |           |  not defined.
    IL_18,  //  0x18 |           |  not defined.
    IL_19,  //  0x19 |           |  not defined.
    IL_1A,  //  0x1a |           |  not defined.
    IL_1B,  //  0x1b |           |  not defined.
    IL_1C,  //  0x1c |           |  not defined.
    IL_1D,  //  0x1d |           |  not defined.
    IL_1E,  //  0x1e |           |  not defined.
    IL_1F   //  0x1f |           |  not defined.
} il_commands_t ;

typedef enum IL_OPERANDS {
    // operands
    N_OPERANDS,      // 0x00
    OP_INPUT,        // 0x01 | i
    OP_REAL_INPUT,   // 0x02 | if
    OP_FALLING,      // 0x03 | f
    OP_RISING,       // 0x04 | r
    OP_MEMORY,       // 0x05 | m
    OP_REAL_MEMORY,  // 0x06 | mf
    OP_COMMAND,      // 0x07 | c
    OP_BLINKOUT,     // 0x08 | b
    OP_TIMEOUT,      // 0x09 | t
    OP_OUTPUT,       // 0x0a | q
    OP_REAL_OUTPUT,  // 0x0b | qf
    // coils
    OP_CONTACT,      // 0x0c | Q
    OP_REAL_CONTACT, // 0x0d | QF
    OP_START,        // 0x0e | T
    OP_PULSEIN,      // 0x0f | M
    OP_REAL_MEMIN,   // 0x10 | MF
    OP_WRITE,        // 0x11 | W
    OP_END,          // 0x12 |
} il_operands_t;



#endif /* LIBRELOGIC_NEWVM_H_ */
