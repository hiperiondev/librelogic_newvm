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

#ifndef LIBRELOGIC_ASSEM_DISASSEM_H_
#define LIBRELOGIC_ASSEM_DISASSEM_H_

void dump_instr(uint32_t instr);
void compile_il(char *file);



#endif /* LIBRELOGIC_ASSEM_DISASSEM_H_ */
