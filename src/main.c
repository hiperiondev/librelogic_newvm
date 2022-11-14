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

#include "librelogic_newvm.h"
#include "librelogic_assem_disassem.h"

int main(void) {
    compile_il("test.il");
    printf("\n--------------------------------\n");
    printf("--------------------------------\n\n");
    compile_il("test2.il");

    return EXIT_SUCCESS;
}





