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

#if !defined(INTEGER_INSTRUCTIONS_SCHIP_)
#define INTEGER_INSTRUCTIONS_SCHIP_

#include <assert.h>
#include <stdbool.h>

#include "lexer.h"
#include "expression.h"
#include "parse.h"
#include "parse_expression.h"
#include "errors.h"
#include "section.h"

#include "schip_parse.h"
#include "schip_tokens.h"

#define MODE_REG    0x01
#define MODE_IMM    0x02
#define MODE_I      0x04
#define MODE_DT     0x08
#define MODE_ST     0x10
#define MODE_I_IND  0x20
#define MODE_IMM_V0 0x40
#define MODE_RPL    0x80

#define REGISTER_NONE 0xFF

typedef struct {
    uint8_t mode;
    uint8_t registerIndex;
    SExpression* expression;
} SAddressMode;

static uint8_t
getRegister(void) {
    if (lex_Current.token >= T_CHIP_REG_V0 && lex_Current.token <= T_CHIP_REG_V15) {
        uint8_t result = (uint8_t) (lex_Current.token - T_CHIP_REG_V0);
        parse_GetToken();

        return result;
    }

    return REGISTER_NONE;
}

static bool
parseAddressMode(SAddressMode* pMode) {
    if ((pMode->registerIndex = getRegister()) != REGISTER_NONE) {
        pMode->mode = MODE_REG;
        if (lex_Current.token != T_OP_ADD)
            return true;

        parse_GetToken();
        if ((pMode->expression = schip_ParseExpressionU12()) != NULL) {
            pMode->mode = MODE_IMM_V0;
            return true;
        }
    } else if (lex_Current.token == T_CHIP_REG_I) {
        parse_GetToken();
        pMode->mode = MODE_I;
        return true;
    } else if (lex_Current.token == T_CHIP_REG_RPL) {
        parse_GetToken();
        pMode->mode = MODE_RPL;
        return true;
    } else if (lex_Current.token == T_CHIP_REG_DT) {
        parse_GetToken();
        pMode->mode = MODE_DT;
        return true;
    } else if (lex_Current.token == T_CHIP_REG_ST) {
        parse_GetToken();
        pMode->mode = MODE_ST;
        return true;
    } else if (lex_Current.token == T_CHIP_REG_I_IND) {
        parse_GetToken();
        pMode->mode = MODE_I_IND;
        return true;
    } else if ((pMode->expression = parse_Expression(1)) != NULL) {
        pMode->mode = MODE_IMM;
        return true;
    }
    return false;
}

static bool
handleModeReg(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputConst16(opcode | (mode1->registerIndex << 8));
    return true;
}

static bool
handleModeRegReg(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputConst16(opcode | (mode1->registerIndex << 8) | (mode2->registerIndex << 4));
    return true;
}

static bool
handleModeImm12(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputExpr16(expr_Or(expr_Const(opcode), expr_CheckRange(mode1->expression, 0, 4095)));
    return true;
}

static bool
handleDRW(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputExpr16(
        expr_Or(
            expr_Const(opcode | (mode1->registerIndex << 8) | (mode2->registerIndex << 4)),
            expr_And(
                expr_CheckRange(mode3->expression, 0, 16),
                expr_Const(15))));
    return true;
}

static bool
handleSCRD(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputExpr16(expr_Or(expr_Const(opcode), expr_CheckRange(mode3->expression, 0, 15)));
    return true;
}

static bool
handleModeRegImm(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputExpr16(
        expr_Or(
            expr_Const(opcode | (mode1->registerIndex << 8)),
            expr_And(
                expr_CheckRange(mode2->expression, -128, 255),
                expr_Const(255))));
    return true;
}

static bool
handleLD(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    if (mode1->mode == MODE_REG && mode2->mode == MODE_IMM)
        return handleModeRegImm(mode1, mode2, mode3, 0x6000);

    if (mode1->mode == MODE_REG && mode2->mode == MODE_REG)
        return handleModeRegReg(mode1, mode2, mode3, 0x8000);

    if (mode1->mode == MODE_I && mode2->mode == MODE_IMM)
        return handleModeImm12(mode2, NULL, NULL, 0xA000);

    if (mode1->mode == MODE_REG && mode2->mode == MODE_DT)
        return handleModeReg(mode1, NULL, NULL, 0xF007);

    if (mode1->mode == MODE_DT && mode2->mode == MODE_REG)
        return handleModeReg(mode2, NULL, NULL, 0xF015);

    if (mode1->mode == MODE_ST && mode2->mode == MODE_REG)
        return handleModeReg(mode2, NULL, NULL, 0xF018);

    return err_Error(ERROR_OPERAND);
}

static bool
handleLDM(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    if (mode1->mode == MODE_REG && mode2->mode == MODE_I_IND)
        return handleModeReg(mode1, NULL, NULL, 0xF065);

    if (mode1->mode == MODE_I_IND && mode2->mode == MODE_REG)
        return handleModeReg(mode2, NULL, NULL, 0xF055);

    if (mode1->mode == MODE_REG && mode2->mode == MODE_RPL)
        return handleModeReg(mode1, NULL, NULL, 0xF085);

    if (mode1->mode == MODE_RPL && mode2->mode == MODE_REG)
        return handleModeReg(mode2, NULL, NULL, 0xF075);

    return err_Error(ERROR_OPERAND);
}

static bool
handleADD(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    if (mode1->mode == MODE_REG && mode2->mode == MODE_REG)
        return handleModeRegReg(mode1, mode2, NULL, 0x8004);

    if (mode1->mode == MODE_REG && mode2->mode == MODE_IMM)
        return handleModeRegImm(mode1, mode2, NULL, 0x7000);

    if (mode1->mode == MODE_I && mode2->mode == MODE_REG)
        return handleModeReg(mode2, NULL, NULL, 0xF01E);

    return err_Error(ERROR_OPERAND);
}

static bool
handleSkips(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    if (mode1->mode == MODE_REG && mode2->mode == MODE_IMM)
        return handleModeRegImm(mode1, mode2, NULL, opcode);

    if (mode1->mode == MODE_REG && mode2->mode == MODE_REG) {
        opcode = (uint16_t) (opcode == 0x3000 ? 0x5000 : 0x9000);
        return handleModeRegReg(mode1, mode2, NULL, opcode);
    }

    return err_Error(ERROR_OPERAND);
}

static bool
handleJP(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    if (mode1->mode == MODE_IMM)
        return handleModeImm12(mode1, NULL, NULL, 0x1000);

    if (mode1->mode == MODE_IMM_V0)
        return handleModeImm12(mode1, NULL, NULL, 0xB000);

    return err_Error(ERROR_OPERAND);
}

static bool
handleModeNone(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode) {
    sect_OutputConst16(opcode);
    return true;
}

typedef bool (* mnemonicHandler)(SAddressMode* mode1, SAddressMode* mode2, SAddressMode* mode3, uint16_t opcode);

typedef struct {
    uint8_t mode1;
    uint8_t mode2;
    uint8_t mode3;
    uint16_t opcode;
    mnemonicHandler fpParser;
} SInstruction;

static SInstruction instructionHandlers[T_CHIP_INSTR_LAST - T_CHIP_INSTR_FIRST + 1] = {
    { MODE_REG, 0, 0, 0xF033, handleModeReg },	// BCD
    { MODE_REG, 0, 0, 0xF029, handleModeReg },	// LDF
    { MODE_REG, 0, 0, 0xF030, handleModeReg },	// LDF10
    { MODE_REG, 0, 0, 0x800E, handleModeReg },	// SHL
    { MODE_REG, 0, 0, 0xE0A1, handleModeReg },	// SKNP
    { MODE_REG, 0, 0, 0xE09E, handleModeReg },	// SKP
    { MODE_REG, 0, 0, 0x8006, handleModeReg },	// SHR
    { MODE_REG, 0, 0, 0xF00A, handleModeReg },	// WKP

    { MODE_REG, MODE_REG, 0, 0x8002, handleModeRegReg },	// AND
    { MODE_REG, MODE_REG, 0, 0x8001, handleModeRegReg },	// OR
    { MODE_REG, MODE_REG, 0, 0x8005, handleModeRegReg },	// SUB
    { MODE_REG, MODE_REG, 0, 0x8007, handleModeRegReg },	// SUBN
    { MODE_REG, MODE_REG, 0, 0x8003, handleModeRegReg },	// XOR

    { MODE_REG, MODE_REG, MODE_IMM, 0xD000, handleDRW },	// DRW

    { MODE_REG | MODE_I | MODE_DT | MODE_ST, MODE_REG | MODE_IMM | MODE_DT, 0, 0, handleLD},	// LD

    { MODE_REG | MODE_I_IND | MODE_RPL, MODE_REG | MODE_I_IND | MODE_RPL, 0, 0, handleLDM},	// LDM

    { MODE_REG | MODE_I, MODE_REG | MODE_IMM, 0, 0, handleADD},	// ADD

    { MODE_REG, MODE_REG | MODE_IMM, 0, 0x3000, handleSkips},	// SE
    { MODE_REG, MODE_REG | MODE_IMM, 0, 0x4000, handleSkips},	// SNE

    { MODE_REG, MODE_IMM, 0, 0xC000, handleModeRegImm},	// RND

    { MODE_IMM, 0, 0, 0x00C0, handleSCRD},	// SCRD

    { MODE_IMM | MODE_IMM_V0, 0, 0, 0x1000, handleJP},	// JP

    { MODE_IMM, 0, 0, 0x2000, handleModeImm12},	// CALL

    { 0, 0, 0, 0x00E0, handleModeNone},	// CLS
    { 0, 0, 0, 0x00FD, handleModeNone},	// EXIT
    { 0, 0, 0, 0x00FE, handleModeNone},	// LO
    { 0, 0, 0, 0x00FF, handleModeNone},	// HI
    { 0, 0, 0, 0x00EE, handleModeNone},	// RET
    { 0, 0, 0, 0x00FB, handleModeNone},	// SCRR
    { 0, 0, 0, 0x00FC, handleModeNone},	// SCRL
};

bool
schip_ParseIntegerInstruction(void) {
    if (T_CHIP_INSTR_FIRST <= lex_Current.token && lex_Current.token <= T_CHIP_INSTR_LAST) {
        SInstruction* instruction = &instructionHandlers[lex_Current.token - T_CHIP_INSTR_FIRST];

        parse_GetToken();

        SAddressMode mode1 = {0, 0, NULL};
        SAddressMode mode2 = {0, 0, NULL};
        SAddressMode mode3 = {0, 0, NULL};

        if (instruction->mode1 != 0 && parseAddressMode(&mode1)) {
            if ((instruction->mode1 & mode1.mode) == 0)
                return err_Error(ERROR_OPERAND);

            if (instruction->mode2 != 0 && parse_ExpectComma() && parseAddressMode(&mode2)) {
                if ((instruction->mode2 & mode2.mode) == 0)
                    return err_Error(ERROR_OPERAND);

                if (instruction->mode3 != 0 && parse_ExpectComma() && parseAddressMode(&mode3)) {
                    if ((instruction->mode3 & mode3.mode) == 0)
                        return err_Error(ERROR_OPERAND);
                }
            }
        }

        return instruction->fpParser(&mode1, &mode2, &mode3, instruction->opcode);
    }

    return false;
}

#endif