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

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// From util
#include "asmotor.h"
#include "fmath.h"
#include "mem.h"
#include "strbuf.h"

// From xasm
#include "xasm.h"
#include "lexer.h"
#include "options.h"
#include "filestack.h"
#include "project.h"
#include "symbol.h"

bool tokens_expandStrings;
uint32_t tokens_binaryConstId;

/*	Private data */

static SLexerTokenDefinition staticTokens[] = {
        {"||",        T_OP_LOGICOR},
        {"&&",        T_OP_LOGICAND},
        {"==",        T_OP_LOGICEQU},
        {">",         T_OP_LOGICGT},
        {"<",         T_OP_LOGICLT},
        {">=",        T_OP_LOGICGE},
        {"<=",        T_OP_LOGICLE},
        {"~=",        T_OP_LOGICNE},
        {"~!",        T_OP_LOGICNOT},
        {"|",         T_OP_OR},
        {"!",         T_OP_OR},
        {"^",         T_OP_XOR},
        {"&",         T_OP_AND},
        {"<<",        T_OP_SHL},
        {">>",        T_OP_SHR},
        {"+",         T_OP_ADD},
        {"-",         T_OP_SUB},
        {"*",         T_OP_MUL},
        {"/",         T_OP_DIV},
        {"~/",        T_OP_MOD},
        {"~",         T_OP_NOT},

        {"def",       T_FUNC_DEF},

        {"//",        T_FUNC_FDIV},
        {"**",        T_FUNC_FMUL},

        {"sin",       T_FUNC_SIN},
        {"cos",       T_FUNC_COS},
        {"tan",       T_FUNC_TAN},
        {"asin",      T_FUNC_ASIN},
        {"acos",      T_FUNC_ACOS},
        {"atan",      T_FUNC_ATAN},
        {"atan2",     T_FUNC_ATAN2},

        {"compareto", T_FUNC_COMPARETO},
        {"indexof",   T_FUNC_INDEXOF},
        {"slice",     T_FUNC_SLICE},
        {"length",    T_FUNC_LENGTH},
        {"toupper",   T_FUNC_TOUPPER},
        {"tolower",   T_FUNC_TOLOWER},

        {"printt",    T_POP_PRINTT},
        {"printv",    T_POP_PRINTV},
        {"printf",    T_POP_PRINTF},
        {"export",    T_POP_EXPORT},
        {"xdef",      T_POP_EXPORT},
        {"import",    T_POP_IMPORT},
        {"xref",      T_POP_IMPORT},
        {"global",    T_POP_GLOBAL},

        {"rsreset",   T_POP_RSRESET},
        {"rsset",     T_POP_RSSET},

        {"set",       T_POP_SET},
        {"=",         T_POP_SET},

        {"section",   T_POP_SECTION},
        {"group",     T_POP_GROUP},
        {"text",      T_GROUP_TEXT},
        {"ram",       T_GROUP_BSS},
        {"org",       T_POP_ORG},

        {"equ",       T_POP_EQU},
        {"equs",      T_POP_EQUS},

        {"purge",     T_POP_PURGE},

        {"fail",      T_POP_FAIL},
        {"warn",      T_POP_WARN},

        {"include",   T_POP_INCLUDE},
        {"incbin",    T_POP_INCBIN},

        {"rept",      T_POP_REPT},
        {"endr",      T_POP_ENDR},    /*	Not needed but we have it here just to protect the name */
        {"rexit",     T_POP_REXIT},

        {"if",        T_POP_IF},
        {"ifc",       T_POP_IFC},
        {"ifd",       T_POP_IFD},
        {"ifnc",      T_POP_IFNC},
        {"ifnd",      T_POP_IFND},
        {"ifne",      T_POP_IF},
        {"ifeq",      T_POP_IFEQ},
        {"ifgt",      T_POP_IFGT},
        {"ifge",      T_POP_IFGE},
        {"iflt",      T_POP_IFLT},
        {"ifle",      T_POP_IFLE},
        {"else",      T_POP_ELSE},
        {"endc",      T_POP_ENDC},

        {"macro",     T_POP_MACRO},
        {"endm",      T_POP_ENDM},    /*	Not needed but we have it here just to protect the name */
        {"shift",     T_POP_SHIFT},
        {"mexit",     T_POP_MEXIT},

        {"pushs",     T_POP_PUSHS},
        {"pops",      T_POP_POPS},
        {"pusho",     T_POP_PUSHO},
        {"popo",      T_POP_POPO},

        {"opt",       T_POP_OPT},

        {"lowlimit",  T_FUNC_LOWLIMIT},
        {"highlimit", T_FUNC_HIGHLIMIT},

        {"even",      T_POP_EVEN},
        {"cnop",      T_POP_CNOP},

        {"end",       T_POP_END},

        {NULL,        0}
};

static int32_t
binaryCharToInt(char ch) {
    for (int32_t i = 0; i <= 1; ++i) {
        if (opt_Current->binaryLiteralCharacters[i] == ch)
            return i;
    }

    return 0;
}

static int32_t
hexCharToInt(char ch) {
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;

    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;

    if (ch >= '0' && ch <= '9')
        return ch - '0';

    return 0;
}

typedef int32_t (* charToInt_t)(char ch);

static int32_t
textToBinary(size_t len) {
    int32_t result = 0;

    int32_t radix;
    charToInt_t charToInt;
    size_t index = 0;
    switch (lex_PeekChar(index)) {
        case '$':
            radix = 16;
            ++index;
            --len;
            charToInt = hexCharToInt;
            break;
        case '%':
            ++index;
            --len;
            radix = 2;
            charToInt = binaryCharToInt;
            break;
        default:
            radix = 10;
            charToInt = hexCharToInt;
            break;
    }

    while (lex_PeekChar(index) != '\0' && len-- > 0) {
        result = result * radix + charToInt(lex_PeekChar(index++));
    }

    return result;
}

static bool
ParseNumber(size_t size) {
    lex_Current.value.integer = textToBinary(size);
    return true;
}

static bool
matchChar(size_t* index, char ch) {
    if (lex_PeekChar(*index) == ch) {
        *index += 1;
        return true;
    } else {
        return false;
    }
}

static bool
parseDecimal(size_t size) {
    size_t index = 0;
    uint32_t value = 0;

    while (lex_PeekChar(index) >= '0' && lex_PeekChar(index) <= '9' && size-- > 0)
        value = value * 10 + lex_PeekChar(index++) - '0';

    if (matchChar(&index, '.')) {
        uint32_t fraction = 0;
        uint32_t d = 1;
        --size;
        while (lex_PeekChar(index) >= '0' && lex_PeekChar(index) <= '9' && size-- > 0) {
            fraction = fraction * 10 + lex_PeekChar(index++) - '0';
            d *= 10;
        }

        value = (value << 16u) + imuldiv(fraction, 65536, d);
    }

    lex_Current.length -= size;
    lex_Current.value.integer = value;
    return true;
}

static bool
parseSymbol(size_t size) {
    string_buffer* pBuffer = strbuf_Create();
    string* pString;
    bool r;

    for (size_t index = 0; index < size; ++index) {
        char ch = lex_PeekChar(index);

        if (ch == '\\') {
            if (index + 1 < size) {
                ch = lex_PeekChar(++index);

                string* marg = NULL;
                if (ch == '@')
                    marg = fstk_GetMacroUniqueId();
                else if (ch >= '0' && ch <= '9')
                    marg = fstk_GetMacroArgValue(ch);

                if (marg != NULL) {
                    strbuf_AppendString(pBuffer, marg);
                    str_Free(marg);
                    continue;
                }
            }

            prj_Fail(ERROR_ID_MALFORMED);
            strbuf_Free(pBuffer);
            return false;
        }

        strbuf_AppendChar(pBuffer, ch);
    }

    pString = strbuf_String(pBuffer);
    strbuf_Free(pBuffer);

    if (tokens_expandStrings && sym_IsString(pString)) {
        string* pValue = sym_GetStringValueByName(pString);
        size_t len = str_Length(pValue);

        lex_SkipBytes(size);
        lex_UnputString(str_String(pValue));

        for (size_t i = 0; i < len; ++i) {
            if (str_CharAt(pValue, i) == '\n')
                fstk_Current->lineNumber -= 1;
        }

        r = false;
        str_Free(pValue);
    } else {
        strcpy(lex_Current.value.string, str_String(pString));
        r = true;
    }

    str_Free(pString);
    return r;
}

static bool
parseMacroArgumentSymbol(size_t size) {
    string* arg = fstk_GetMacroArgValue(lex_PeekChar(1));
    lex_SkipBytes(size);
    if (arg != NULL) {
        lex_UnputString(str_String(arg));
        str_Free(arg);
    }
    return false;
}

static bool
parseUniqueIdSymbol(size_t size) {
    string* id = fstk_GetMacroUniqueId();
    lex_SkipBytes(size);
    if (id != NULL) {
        lex_UnputString(str_String(id));
        str_Free(id);
    }
    return false;
}

enum {
    T_LEX_MACROARG = 3000,
    T_LEX_MACROUNIQUE
};

static SVariadicWordDefinition s_sMacroArgToken = {
        parseMacroArgumentSymbol, T_LEX_MACROARG
};

static SVariadicWordDefinition s_sMacroUniqueToken = {
        parseUniqueIdSymbol, T_LEX_MACROUNIQUE
};

static SVariadicWordDefinition s_sDecimal = {
        parseDecimal, T_NUMBER
};

static SVariadicWordDefinition s_sNumberToken = {
        ParseNumber, T_NUMBER
};

static SVariadicWordDefinition s_sIDToken = {
        parseSymbol, T_ID
};

void
tokens_Init(void) {
    tokens_expandStrings = true;

    lex_Init();

    lex_DefineTokens(staticTokens);

    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken("__RSB", T_POP_RB);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken("__RSW", T_POP_RW);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken("__RSL", T_POP_RL);

    if (g_pConfiguration->pszNameRB && g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken(g_pConfiguration->pszNameRB, T_POP_RB);
    if (g_pConfiguration->pszNameRW && g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken(g_pConfiguration->pszNameRW, T_POP_RW);
    if (g_pConfiguration->pszNameRL && g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken(g_pConfiguration->pszNameRL, T_POP_RL);

    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken("__DSB", T_POP_DSB);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken("__DSW", T_POP_DSW);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken("__DSL", T_POP_DSL);

    if (g_pConfiguration->pszNameDSB && g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken(g_pConfiguration->pszNameDSB, T_POP_DSB);
    if (g_pConfiguration->pszNameDSW && g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken(g_pConfiguration->pszNameDSW, T_POP_DSW);
    if (g_pConfiguration->pszNameDSL && g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken(g_pConfiguration->pszNameDSL, T_POP_DSL);

    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken("__DCB", T_POP_DB);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken("__DCW", T_POP_DW);
    if (g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken("__DCL", T_POP_DL);

    if (g_pConfiguration->pszNameDB && g_pConfiguration->eMinimumWordSize <= MINSIZE_8BIT)
        lex_DefineToken(g_pConfiguration->pszNameDB, T_POP_DB);
    if (g_pConfiguration->pszNameDW && g_pConfiguration->eMinimumWordSize <= MINSIZE_16BIT)
        lex_DefineToken(g_pConfiguration->pszNameDW, T_POP_DW);
    if (g_pConfiguration->pszNameDL && g_pConfiguration->eMinimumWordSize <= MINSIZE_32BIT)
        lex_DefineToken(g_pConfiguration->pszNameDL, T_POP_DL);

    if (g_pConfiguration->bSupportBanks)
        lex_DefineToken("bank", T_FUNC_BANK);

    /* Local ID */
    uint32_t id;

    id = lex_VariadicCreateWord(&s_sIDToken);
    lex_VariadicAddCharRange(id, '.', '.', 0);
    lex_VariadicAddCharRange(id, 'a', 'z', 1);
    lex_VariadicAddCharRange(id, 'A', 'Z', 1);
    lex_VariadicAddCharRange(id, '_', '_', 1);
    lex_VariadicAddCharRange(id, '#', '#', 1);
    lex_VariadicAddCharRange(id, '\\', '\\', 1);
    lex_VariadicAddCharRangeRepeating(id, 'a', 'z', 2);
    lex_VariadicAddCharRangeRepeating(id, 'A', 'Z', 2);
    lex_VariadicAddCharRangeRepeating(id, '0', '9', 2);
    lex_VariadicAddCharRangeRepeating(id, '_', '_', 2);
    lex_VariadicAddCharRangeRepeating(id, '\\', '\\', 2);
    lex_VariadicAddCharRangeRepeating(id, '@', '@', 2);
    lex_VariadicAddCharRangeRepeating(id, '#', '#', 2);

    id = lex_VariadicCreateWord(&s_sIDToken);
    lex_VariadicAddCharRangeRepeating(id, '0', '9', 0);
    lex_VariadicAddCharRangeRepeating(id, '\\', '\\', 0);
    lex_VariadicAddCharRangeRepeating(id, '@', '@', 1);
    lex_VariadicAddSuffix(id, '$');

    /* ID's */

    id = lex_VariadicCreateWord(&s_sIDToken);
    lex_VariadicAddCharRange(id, 'a', 'z', 0);
    lex_VariadicAddCharRange(id, 'A', 'Z', 0);
    lex_VariadicAddCharRange(id, '_', '_', 0);
    lex_VariadicAddCharRange(id, '@', '@', 0);
    lex_VariadicAddCharRange(id, '\\', '\\', 0);
    lex_VariadicAddCharRangeRepeating(id, 'a', 'z', 1);
    lex_VariadicAddCharRangeRepeating(id, 'A', 'Z', 1);
    lex_VariadicAddCharRangeRepeating(id, '0', '9', 1);
    lex_VariadicAddCharRangeRepeating(id, '_', '_', 1);
    lex_VariadicAddCharRangeRepeating(id, '\\', '\\', 1);
    lex_VariadicAddCharRangeRepeating(id, '@', '@', 1);
    lex_VariadicAddCharRangeRepeating(id, '#', '#', 1);

    /* Macro arguments */

    id = lex_VariadicCreateWord(&s_sMacroArgToken);
    lex_VariadicAddCharRange(id, '\\', '\\', 0);
    lex_VariadicAddCharRange(id, '0', '9', 1);

    id = lex_VariadicCreateWord(&s_sMacroUniqueToken);
    lex_VariadicAddCharRange(id, '\\', '\\', 0);
    lex_VariadicAddCharRange(id, '@', '@', 1);

    /* Decimal constants */

    id = lex_VariadicCreateWord(&s_sDecimal);
    lex_VariadicAddCharRange(id, '0', '9', 0);
    lex_VariadicAddCharRangeRepeating(id, '0', '9', 1);
    lex_VariadicAddCharRangeRepeating(id, '.', '.', 1);

    /* Hex constants */

    id = lex_VariadicCreateWord(&s_sNumberToken);
    lex_VariadicAddCharRange(id, '$', '$', 0);
    lex_VariadicAddCharRangeRepeating(id, '0', '9', 1);
    lex_VariadicAddCharRangeRepeating(id, 'A', 'F', 1);
    lex_VariadicAddCharRangeRepeating(id, 'a', 'f', 1);

    /* Binary constants */

    tokens_binaryConstId = id = lex_VariadicCreateWord(&s_sNumberToken);
    lex_VariadicAddCharRange(id, '%', '%', 0);
    lex_VariadicAddCharRangeRepeating(id, '0', '1', 1);
}