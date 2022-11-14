/*******************************************************************************
 LibreLogic : a free PLC library
 Copyright (C) 2022, Antonis K. (kalamara AT ceid DOT upatras DOT gr)

 New VM: 2022 Emiliano Augusto Gonzalez ( egonzalez . hiperion @ gmail . com )

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "librelogic_newvm.h"

////////////////////////// VM /////////////////////////////
uint8_t vm_execute(uint32_t *vm_program, uint32_t prg_len) {
    uint32_t pc = 0;
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
#define DISPATCH() \
    goto *dispatch_vm[IL(vm_program[pc++])]

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
