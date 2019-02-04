/*  Copyright 2008-2017 Carsten Elton Sorensen

    This file is part of ASMotor.

    ASMotor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ASMotor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ASMotor.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XASM_COMMON_TOKENS_H_INCLUDED_
#define XASM_COMMON_TOKENS_H_INCLUDED_

typedef enum {
    T_NONE = 0,
    T_LINEFEED = '\n',
    T_LEFT_PARENS = '(',
    T_COMMA = ',',
    T_AT = '@',

    T_FIRST_TOKEN = 300,
    T_STRING = T_FIRST_TOKEN,
    T_LABEL,
    T_ID,
    T_NUMBER,

    T_OP_EQUAL,
    T_OP_NOT_EQUAL,
    T_OP_GREATER_THAN,
    T_OP_GREATER_OR_EQUAL,
    T_OP_LESS_THAN,
    T_OP_LESS_OR_EQUAL,
    T_OP_BOOLEAN_OR,
    T_OP_BOOLEAN_AND,
    T_OP_BOOLEAN_NOT,
    T_OP_BITWISE_OR,
    T_OP_BITWISE_XOR,
    T_OP_BITWISE_AND,
    T_OP_BITWISE_ASL,
    T_OP_BITWISE_ASR,
    T_OP_BITWISE_NOT,
    T_OP_ADD,
    T_OP_SUBTRACT,
    T_OP_MULTIPLY,
    T_OP_DIVIDE,
    T_OP_MODULO,
    T_OP_BIT,

    T_FUNC_DEF,

    T_FUNC_BANK,

    T_OP_FDIV,
    T_OP_FMUL,
    T_FUNC_SIN,
    T_FUNC_COS,
    T_FUNC_TAN,
    T_FUNC_ASIN,
    T_FUNC_ACOS,
    T_FUNC_ATAN,
    T_FUNC_ATAN2,

    T_STR_MEMBER_COMPARETO,
    T_STR_MEMBER_INDEXOF,
    T_STR_MEMBER_LENGTH,
    T_STR_MEMBER_SLICE,
    T_STR_MEMBER_UPPER,
    T_STR_MEMBER_LOWER,

    T_SYM_MACRO,
    T_SYM_ENDM,        /* Not needed but we have it here just to protect the name */
    T_SYM_EQU,
    T_SYM_EQUS,
    T_SYM_SET,
    T_SYM_GROUP,

    T_GROUP_TEXT,
    T_GROUP_BSS,

    T_DIRECTIVE_FIRST,

    T_DIRECTIVE_RSRESET = T_DIRECTIVE_FIRST,
    T_DIRECTIVE_RSSET,
    T_DIRECTIVE_RB,
    T_DIRECTIVE_RW,
    T_DIRECTIVE_RL,

    T_DIRECTIVE_PRINTT,
    T_DIRECTIVE_PRINTV,
    T_DIRECTIVE_PRINTF,

    T_DIRECTIVE_EXPORT,
    T_DIRECTIVE_IMPORT,
    T_DIRECTIVE_GLOBAL,
    T_DIRECTIVE_PURGE,

    T_DIRECTIVE_FAIL,
    T_DIRECTIVE_WARN,

    T_DIRECTIVE_INCLUDE,
    T_DIRECTIVE_INCBIN,

    T_DIRECTIVE_DSB,
    T_DIRECTIVE_DSW,
    T_DIRECTIVE_DSL,

    T_DIRECTIVE_DB,
    T_DIRECTIVE_DW,
    T_DIRECTIVE_DL,

    T_DIRECTIVE_SECTION,

    T_DIRECTIVE_ORG,

    T_DIRECTIVE_SHIFT,
    T_DIRECTIVE_MEXIT,

    T_DIRECTIVE_REPT,
    T_DIRECTIVE_REXIT,

    T_DIRECTIVE_IFC,
    T_DIRECTIVE_IFNC,
    T_DIRECTIVE_IFD,
    T_DIRECTIVE_IFND,
    T_DIRECTIVE_IF,
    T_DIRECTIVE_IFEQ,
    T_DIRECTIVE_IFGT,
    T_DIRECTIVE_IFGE,
    T_DIRECTIVE_IFLT,
    T_DIRECTIVE_IFLE,
    T_DIRECTIVE_ELSE,
    T_DIRECTIVE_ENDC,

    T_DIRECTIVE_EVEN,
    T_DIRECTIVE_CNOP,

    T_DIRECTIVE_PUSHO,
    T_DIRECTIVE_POPO,

    T_DIRECTIVE_OPT,

    T_DIRECTIVE_PUSHS,
    T_DIRECTIVE_POPS,

    T_DIRECTIVE_LAST = T_DIRECTIVE_POPS,

    T_POP_ENDR,        /* Not needed but we have it here just to protect the name */

    T_FUNC_LOWLIMIT,
    T_FUNC_HIGHLIMIT,

    T_MACROARG0,

    T_POP_END
} EToken;

extern void
tokens_Init(void);

extern bool tokens_ExpandStrings;
extern uint32_t tokens_BinaryVariadicId;

#endif /* XASM_COMMON_TOKENS_H_INCLUDED_ */
