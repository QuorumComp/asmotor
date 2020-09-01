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

#include "xasm.h"
#include "lexer_constants.h"

#include "m68k_tokens.h"

static SLexConstantsWord g_tokens[] = {
	{ "ABCD",	T_68K_ABCD	},
	{ "ADD",	T_68K_ADD	},
	{ "ADDA",	T_68K_ADD	},
	{ "ADDI",	T_68K_ADDI	},
	{ "ADDQ",	T_68K_ADDQ	},
	{ "ADDX",	T_68K_ADDX	},
	{ "AND",	T_68K_AND	},
	{ "ANDI",	T_68K_ANDI	},
	{ "ASL",	T_68K_ASL	},
	{ "ASR",	T_68K_ASR	},

	// BCC START
	{ "BCC",	T_68K_BCC	},
	{ "BHS",	T_68K_BCC	},
	{ "BCS",	T_68K_BCS	},
	{ "BLO",	T_68K_BCS	},
	{ "BEQ",	T_68K_BEQ	},
	{ "BGE",	T_68K_BGE	},
	{ "BGT",	T_68K_BGT	},
	{ "BHI",	T_68K_BHI	},
	{ "BLE",	T_68K_BLE	},
	{ "BLS",	T_68K_BLS	},
	{ "BLT",	T_68K_BLT	},
	{ "BMI",	T_68K_BMI	},
	{ "BNE",	T_68K_BNE	},
	{ "BPL",	T_68K_BPL	},
	{ "BVC",	T_68K_BVC	},
	{ "BVS",	T_68K_BVS	},
	// BCC END

	{ "BCHG",	T_68K_BCHG		},
	{ "BCLR",	T_68K_BCLR		},
	{ "BFCHG",	T_68K_BFCHG		},
	{ "BFCLR",	T_68K_BFCLR		},
	{ "BFEXTS",	T_68K_BFEXTS	},
	{ "BFEXTU",	T_68K_BFEXTU	},
	{ "BFFFO",	T_68K_BFFFO		},
	{ "BFINS",	T_68K_BFINS		},
	{ "BFSET",	T_68K_BFSET		},
	{ "BFTST",	T_68K_BFTST		},
	{ "BKPT",	T_68K_BKPT		},
	{ "BRA",	T_68K_BRA		},
	{ "BSET",	T_68K_BSET		},
	{ "BSR",	T_68K_BSR		},
	{ "BTST",	T_68K_BTST		},
	{ "CALLM",	T_68K_CALLM		},
	{ "CAS",	T_68K_CAS		},
	{ "CAS2",	T_68K_CAS2		},
	{ "CHK",	T_68K_CHK		},
	{ "CHK2",	T_68K_CHK2		},
	{ "CINVA",	T_68K_CINVA		},
	{ "CINVL",	T_68K_CINVL		},
	{ "CINVP",	T_68K_CINVP		},
	{ "CLR",	T_68K_CLR		},
	{ "CMP",	T_68K_CMP		},
	{ "CMPA",	T_68K_CMPA		},
	{ "CMPI",	T_68K_CMPI		},
	{ "CMPM",	T_68K_CMPM		},
	{ "CMP2",	T_68K_CMP2		},
	{ "CPUSHA",	T_68K_CPUSHA	},
	{ "CPUSHL",	T_68K_CPUSHL	},
	{ "CPUSHP",	T_68K_CPUSHP	},

	{ "DBCC",	T_68K_DBCC	},
	{ "DBHS",	T_68K_DBCC	},
	{ "DBCS",	T_68K_DBCS	},
	{ "DBLO",	T_68K_DBCS	},
	{ "DBEQ",	T_68K_DBEQ	},
	{ "DBF",	T_68K_DBF	},
	{ "DBRA",	T_68K_DBF	},
	{ "DBGE",	T_68K_DBGE	},
	{ "DBGT",	T_68K_DBGT	},
	{ "DBHI",	T_68K_DBHI	},
	{ "DBLE",	T_68K_DBLE	},
	{ "DBLS",	T_68K_DBLS	},
	{ "DBLT",	T_68K_DBLT	},
	{ "DBMI",	T_68K_DBMI	},
	{ "DBNE",	T_68K_DBNE	},
	{ "DBPL",	T_68K_DBPL	},
	{ "DBT",	T_68K_DBT	},
	{ "DBVC",	T_68K_DBVC	},
	{ "DBVS",	T_68K_DBVS	},

	{ "DIVS",	T_68K_DIVS	},
	{ "DIVSL",	T_68K_DIVSL	},
	{ "DIVU",	T_68K_DIVU	},
	{ "DIVUL",	T_68K_DIVUL	},
	{ "EOR",	T_68K_EOR	},
	{ "EORI",	T_68K_EORI	},
	{ "EXG",	T_68K_EXG	},
	{ "EXT",	T_68K_EXT	},
	{ "EXTB",	T_68K_EXTB	},
	{ "ILLEGAL",	T_68K_ILLEGAL	},
	{ "JMP",	T_68K_JMP	},
	{ "JSR",	T_68K_JSR	},
	{ "LEA",	T_68K_LEA	},
	{ "LINK",	T_68K_LINK	},
	{ "LSL",	T_68K_LSL	},
	{ "LSR",	T_68K_LSR	},
	{ "MOVE",	T_68K_MOVE	},
	{ "MOVEA",	T_68K_MOVEA	},
	{ "MOVEC",	T_68K_MOVEC	},
	{ "MOVE16",	T_68K_MOVE16	},
	{ "MOVEM",	T_68K_MOVEM	},
	{ "MOVEP",	T_68K_MOVEP	},
	{ "MOVEQ",	T_68K_MOVEQ	},
	{ "MOVES",	T_68K_MOVES	},
	{ "MULS",	T_68K_MULS	},
	{ "MULU",	T_68K_MULU	},
	{ "NBCD",	T_68K_NBCD	},
	{ "NEG",	T_68K_NEG	},
	{ "NEGX",	T_68K_NEGX	},
	{ "NOP",	T_68K_NOP	},
	{ "NOT",	T_68K_NOT	},
	{ "OR",		T_68K_OR	},
	{ "ORI",	T_68K_ORI	},
	{ "PACK",	T_68K_PACK	},
	{ "PEA",	T_68K_PEA	},
	{ "RESET",	T_68K_RESET	},
	{ "ROL",	T_68K_ROL	},
	{ "ROR",	T_68K_ROR	},
	{ "ROXL",	T_68K_ROXL	},
	{ "ROXR",	T_68K_ROXR	},
	{ "RTD",	T_68K_RTD	},
	{ "RTE",	T_68K_RTE	},
	{ "RTM",	T_68K_RTM	},
	{ "RTR",	T_68K_RTR	},
	{ "RTS",	T_68K_RTS	},
	{ "SBCD",	T_68K_SBCD	},

	{ "SCC",	T_68K_SCC	},
	{ "SHS",	T_68K_SCC	},
	{ "SCS",	T_68K_SCS	},
	{ "SLO",	T_68K_SCS	},
	{ "SEQ",	T_68K_SEQ	},
	{ "SF",		T_68K_SF	},
	{ "SGE",	T_68K_SGE	},
	{ "SGT",	T_68K_SGT	},
	{ "SHI",	T_68K_SHI	},
	{ "SLE",	T_68K_SLE	},
	{ "SLS",	T_68K_SLS	},
	{ "SLT",	T_68K_SLT	},
	{ "SMI",	T_68K_SMI	},
	{ "SNE",	T_68K_SNE	},
	{ "SPL",	T_68K_SPL	},
	{ "ST",		T_68K_ST	},
	{ "SVC",	T_68K_SVC	},
	{ "SVS",	T_68K_SVS	},

	{ "STOP",	T_68K_STOP	},
	{ "SUB",	T_68K_SUB	},
	{ "SUBA",	T_68K_SUBA	},
	{ "SUBI",	T_68K_SUBI	},
	{ "SUBQ",	T_68K_SUBQ	},
	{ "SUBX",	T_68K_SUBX	},
	{ "SWAP",	T_68K_SWAP	},
	{ "TAS",	T_68K_TAS	},
	{ "TRAP",	T_68K_TRAP	},

	{ "TRAPCC",	T_68K_TRAPCC	},
	{ "TRAPHS",	T_68K_TRAPCC	},
	{ "TRAPCS",	T_68K_TRAPCS	},
	{ "TRAPLO",	T_68K_TRAPCS	},
	{ "TRAPEQ",	T_68K_TRAPEQ	},
	{ "TRAPF",	T_68K_TRAPF		},
	{ "TRAPGE",	T_68K_TRAPGE	},
	{ "TRAPGT",	T_68K_TRAPGT	},
	{ "TRAPHI",	T_68K_TRAPHI	},
	{ "TRAPLE",	T_68K_TRAPLE	},
	{ "TRAPLS",	T_68K_TRAPLS	},
	{ "TRAPLT",	T_68K_TRAPLT	},
	{ "TRAPMI",	T_68K_TRAPMI	},
	{ "TRAPNE",	T_68K_TRAPNE	},
	{ "TRAPPL",	T_68K_TRAPPL	},
	{ "TRAPT",	T_68K_TRAPT		},
	{ "TRAPVC",	T_68K_TRAPVC	},
	{ "TRAPVS",	T_68K_TRAPVS	},

	{ "TRAPV",	T_68K_TRAPV	},
	{ "TST",	T_68K_TST	},
	{ "UNLK",	T_68K_UNLK	},
	{ "UNPK",	T_68K_UNPK	},
	{ "UNPACK",	T_68K_UNPK	},

	{ "D0",	T_68K_REG_D0	},
	{ "D1",	T_68K_REG_D1	},
	{ "D2",	T_68K_REG_D2	},
	{ "D3",	T_68K_REG_D3	},
	{ "D4",	T_68K_REG_D4	},
	{ "D5",	T_68K_REG_D5	},
	{ "D6",	T_68K_REG_D6	},
	{ "D7",	T_68K_REG_D7	},

	{ "D8",  T_68K_REG_D8	},
	{ "D9",  T_68K_REG_D9	},
	{ "D10", T_68K_REG_D10	},
	{ "D11", T_68K_REG_D11	},
	{ "D12", T_68K_REG_D12	},
	{ "D13", T_68K_REG_D13	},
	{ "D14", T_68K_REG_D14	},
	{ "D15", T_68K_REG_D15	},

	{ "D16", T_68K_REG_D16	},
	{ "D17", T_68K_REG_D17	},
	{ "D18", T_68K_REG_D18	},
	{ "D19", T_68K_REG_D19	},
	{ "D20", T_68K_REG_D20	},
	{ "D21", T_68K_REG_D21	},
	{ "D22", T_68K_REG_D22	},
	{ "D23", T_68K_REG_D23	},

	{ "D24", T_68K_REG_D24	},
	{ "D25", T_68K_REG_D25	},
	{ "D26", T_68K_REG_D26	},
	{ "D27", T_68K_REG_D27	},
	{ "D28", T_68K_REG_D28	},
	{ "D29", T_68K_REG_D29	},
	{ "D30", T_68K_REG_D30	},
	{ "D31", T_68K_REG_D31	},

	{ "A0",	T_68K_REG_A0	},
	{ "A1",	T_68K_REG_A1	},
	{ "A2",	T_68K_REG_A2	},
	{ "A3",	T_68K_REG_A3	},
	{ "A4",	T_68K_REG_A4	},
	{ "A5",	T_68K_REG_A5	},
	{ "A6",	T_68K_REG_A6	},
	{ "A7",	T_68K_REG_A7	},
	{ "SP",	T_68K_REG_A7	},

	{ "A8",  T_68K_REG_A8	},
	{ "A9",  T_68K_REG_A9	},
	{ "A10", T_68K_REG_A10	},
	{ "A11", T_68K_REG_A11	},
	{ "A12", T_68K_REG_A12	},
	{ "A13", T_68K_REG_A13	},
	{ "A14", T_68K_REG_A14	},
	{ "A15", T_68K_REG_A15	},

	{ "CCR",	T_68K_REG_CCR	},
	{ "SR",		T_68K_REG_SR	},
	{ "BC",		T_68K_REG_BC	},
	{ "DC",		T_68K_REG_DC	},
	{ "IC",		T_68K_REG_IC	},
	{ "SFC",	T_68K_REG_SFC	},
	{ "DFC",	T_68K_REG_DFC	},
	{ "USP",	T_68K_REG_USP	},
	{ "VBR",	T_68K_REG_VBR	},
	{ "CACR",	T_68K_REG_CACR	},
	{ "CAAR",	T_68K_REG_CAAR	},
	{ "MSP",	T_68K_REG_MSP	},
	{ "ISP",	T_68K_REG_ISP	},
	{ "TC",		T_68K_REG_TC	},
	{ "ITT0",	T_68K_REG_ITT0	},
	{ "ITT1",	T_68K_REG_ITT1	},
	{ "DTT0",	T_68K_REG_DTT0	},
	{ "DTT1",	T_68K_REG_DTT1	},
	{ "MMUSR",	T_68K_REG_MMUSR	},
	{ "URP",	T_68K_REG_URP	},
	{ "SRP",	T_68K_REG_SRP	},
	{ "IACR0",	T_68K_REG_IACR0	},
	{ "IACR1",	T_68K_REG_IACR1	},
	{ "DACR0",	T_68K_REG_DACR0	},
	{ "DACR1",	T_68K_REG_DACR1	},
	{ "BUSCR",	T_68K_REG_BUSCR	},
	{ "PCR",	T_68K_REG_PCR	},
	{ "BRK",	T_68K_REG_BRK	},
	{ "CYC",	T_68K_REG_CYC	},
	{ "BPC",	T_68K_REG_BPC	},
	{ "BPW",	T_68K_REG_BPW	},

	{ "PC",		T_68K_REG_PC		},
	{ "(PC)",	T_68K_REG_PC_IND	},

	{ "(A0)",	T_68K_REG_A0_IND	},
	{ "(A1)",	T_68K_REG_A1_IND	},
	{ "(A2)",	T_68K_REG_A2_IND	},
	{ "(A3)",	T_68K_REG_A3_IND	},
	{ "(A4)",	T_68K_REG_A4_IND	},
	{ "(A5)",	T_68K_REG_A5_IND	},
	{ "(A6)",	T_68K_REG_A6_IND	},
	{ "(A7)",	T_68K_REG_A7_IND	},
	{ "(SP)",	T_68K_REG_A7_IND	},

	{ "(A8)",	T_68K_REG_A8_IND	},
	{ "(A9)",	T_68K_REG_A9_IND	},
	{ "(A10)",	T_68K_REG_A10_IND	},
	{ "(A11)",	T_68K_REG_A11_IND	},
	{ "(A12)",	T_68K_REG_A12_IND	},
	{ "(A13)",	T_68K_REG_A13_IND	},
	{ "(A14)",	T_68K_REG_A14_IND	},
	{ "(A15)",	T_68K_REG_A15_IND	},

	{ "-(A0)",	T_68K_REG_A0_DEC	},
	{ "-(A1)",	T_68K_REG_A1_DEC	},
	{ "-(A2)",	T_68K_REG_A2_DEC	},
	{ "-(A3)",	T_68K_REG_A3_DEC	},
	{ "-(A4)",	T_68K_REG_A4_DEC	},
	{ "-(A5)",	T_68K_REG_A5_DEC	},
	{ "-(A6)",	T_68K_REG_A6_DEC	},
	{ "-(A7)",	T_68K_REG_A7_DEC	},
	{ "-(SP)",	T_68K_REG_A7_DEC	},

	{ "-(A8)",	T_68K_REG_A8_DEC	},
	{ "-(A9)",	T_68K_REG_A9_DEC	},
	{ "-(A10)",	T_68K_REG_A10_DEC	},
	{ "-(A11)",	T_68K_REG_A11_DEC	},
	{ "-(A12)",	T_68K_REG_A12_DEC	},
	{ "-(A13)",	T_68K_REG_A13_DEC	},
	{ "-(A14)",	T_68K_REG_A14_DEC	},
	{ "-(A15)",	T_68K_REG_A15_DEC	},

	{ "(A0)+",	T_68K_REG_A0_INC	},
	{ "(A1)+",	T_68K_REG_A1_INC	},
	{ "(A2)+",	T_68K_REG_A2_INC	},
	{ "(A3)+",	T_68K_REG_A3_INC	},
	{ "(A4)+",	T_68K_REG_A4_INC	},
	{ "(A5)+",	T_68K_REG_A5_INC	},
	{ "(A6)+",	T_68K_REG_A6_INC	},
	{ "(A7)+",	T_68K_REG_A7_INC	},
	{ "(SP)+",	T_68K_REG_A7_INC	},

	{ "(A8)+",	T_68K_REG_A8_INC	},
	{ "(A9)+",	T_68K_REG_A9_INC	},
	{ "(A10)+",	T_68K_REG_A10_INC	},
	{ "(A11)+",	T_68K_REG_A11_INC	},
	{ "(A12)+",	T_68K_REG_A12_INC	},
	{ "(A13)+",	T_68K_REG_A13_INC	},
	{ "(A14)+",	T_68K_REG_A14_INC	},
	{ "(A15)+",	T_68K_REG_A15_INC	},

	{ "MC68000",	T_68K_MC68000	},
	{ "MC68010",	T_68K_MC68010	},
	{ "MC68020",	T_68K_MC68020	},
	{ "MC68030",	T_68K_MC68030	},
	{ "MC68040",	T_68K_MC68040	},
	{ "MC68060",	T_68K_MC68060	},
	{ "MC68080",	T_68K_MC68080	},

	{ "FPU68881",	T_68K_FPU6888X	},
	{ "FPU68882",	T_68K_FPU6888X	},
	{ "FPU6888X",	T_68K_FPU6888X	},
	{ "FPU68040",	T_68K_FPU68040	},
	{ "FPU68060",	T_68K_FPU68060	},
	{ "FPU68080",	T_68K_FPU68080	},

	{ "REGMASK",	T_68K_REGMASK	},

	{ "FABS",	T_FPU_FABS		},
	{ "FSABS",	T_FPU_FSABS		},
	{ "FDABS",	T_FPU_FDABS		},
	{ "FACOS",	T_FPU_FACOS		},
	{ "FADD",	T_FPU_FADD		},
	{ "FSADD",	T_FPU_FSADD		},
	{ "FDADD",	T_FPU_FDADD		},
	{ "FASIN",	T_FPU_FASIN		},
	{ "FATAN",	T_FPU_FATAN		},
	{ "FATANH",	T_FPU_FATANH	},

	{ "FBF",	T_FPU_FBF	},
	{ "FBEQ",	T_FPU_FBEQ	},
	{ "FBOGT",	T_FPU_FBOGT	},
	{ "FBOGE",	T_FPU_FBOGE	},
	{ "FBOLT",	T_FPU_FBOLT	},
	{ "FBOLE",	T_FPU_FBOLE	},
	{ "FBOGL",	T_FPU_FBOGL	},
	{ "FBOR",	T_FPU_FBOR	},

	{ "FBUN",	T_FPU_FBUN	},
	{ "FBUEQ",	T_FPU_FBUEQ	},
	{ "FBUGT",	T_FPU_FBUGT	},
	{ "FBUGE",	T_FPU_FBUGE	},
	{ "FBULT",	T_FPU_FBULT	},
	{ "FBULE",	T_FPU_FBULE	},
	{ "FBNE",	T_FPU_FBNE	},
	{ "FBT",	T_FPU_FBT	},

	{ "FBSF",	T_FPU_FBSF	},
	{ "FBSEQ",	T_FPU_FBSEQ	},
	{ "FBGT",	T_FPU_FBGT	},
	{ "FBGE",	T_FPU_FBGE	},
	{ "FBLT",	T_FPU_FBLT	},
	{ "FBLE",	T_FPU_FBLE	},
	{ "FBGL",	T_FPU_FBGL	},
	{ "FBGLE",	T_FPU_FBGLE	},

	{ "FBNGLE",	T_FPU_FBNGLE	},
	{ "FBNGL",	T_FPU_FBNGL		},
	{ "FBNLE",	T_FPU_FBNLE		},
	{ "FBNLT",	T_FPU_FBNLT		},
	{ "FBNGE",	T_FPU_FBNGE		},
	{ "FBNGT",	T_FPU_FBNGT		},
	{ "FBSNE",	T_FPU_FBSNE		},
	{ "FBST",	T_FPU_FBST		},

	{ "FCMP",	T_FPU_FCMP	},
	{ "FCOS",	T_FPU_FCOS	},
	{ "FCOSH",	T_FPU_FCOSH	},

	{ "FDBF",	T_FPU_FDBF		},
	{ "FDBEQ",	T_FPU_FDBEQ		},
	{ "FDBOGT",	T_FPU_FDBOGT	},
	{ "FDBOGE",	T_FPU_FDBOGE	},
	{ "FDBOLT",	T_FPU_FDBOLT	},
	{ "FDBOLE",	T_FPU_FDBOLE	},
	{ "FDBOGL",	T_FPU_FDBOGL	},
	{ "FDBOR",	T_FPU_FDBOR		},

	{ "FDBUN",	T_FPU_FDBUN		},
	{ "FDBUEQ",	T_FPU_FDBUEQ	},
	{ "FDBUGT",	T_FPU_FDBUGT	},
	{ "FDBUGE",	T_FPU_FDBUGE	},
	{ "FDBULT",	T_FPU_FDBULT	},
	{ "FDBULE",	T_FPU_FDBULE	},
	{ "FDBNE",	T_FPU_FDBNE		},
	{ "FDBT",	T_FPU_FDBT		},

	{ "FDBSF",	T_FPU_FDBSF		},
	{ "FDBSEQ",	T_FPU_FDBSEQ	},
	{ "FDBGT",	T_FPU_FDBGT		},
	{ "FDBGE",	T_FPU_FDBGE		},
	{ "FDBLT",	T_FPU_FDBLT		},
	{ "FDBLE",	T_FPU_FDBLE		},
	{ "FDBGL",	T_FPU_FDBGL		},
	{ "FDBGLE",	T_FPU_FDBGLE	},

	{ "FDBNGLE",	T_FPU_FDBNGLE	},
	{ "FDBNGL",		T_FPU_FDBNGL	},
	{ "FDBNLE",		T_FPU_FDBNLE	},
	{ "FDBNLT",		T_FPU_FDBNLT	},
	{ "FDBNGE",		T_FPU_FDBNGE	},
	{ "FDBNGT",		T_FPU_FDBNGT	},
	{ "FDBSNE",		T_FPU_FDBSNE	},
	{ "FDBST",		T_FPU_FDBST		},

	{ "FDIV",		T_FPU_FDIV		},
	{ "FSDIV",		T_FPU_FSDIV		},
	{ "FDDIV",		T_FPU_FDDIV		},
	{ "FETOX",		T_FPU_FETOX		},
	{ "FETOXM1",	T_FPU_FETOXM1	},
	{ "FGETEXP",	T_FPU_FGETEXP	},
	{ "FGETMAN",	T_FPU_FGETMAN	},
	{ "FINT",		T_FPU_FINT		},
	{ "FINTRZ",		T_FPU_FINTRZ	},
	{ "FLOG10",		T_FPU_FLOG10	},
	{ "FLOG2",		T_FPU_FLOG2		},
	{ "FLOGN",		T_FPU_FLOGN		},
	{ "FLOGNP1",	T_FPU_FLOGNP1	},
	{ "FMOD",		T_FPU_FMOD		},
	{ "FMOVE",		T_FPU_FMOVE		},
	{ "FSMOVE",		T_FPU_FSMOVE	},
	{ "FDMOVE",		T_FPU_FDMOVE	},
	{ "FMOVECR",	T_FPU_FMOVECR	},
	{ "FMOVEM",		T_FPU_FMOVEM	},
	{ "FMUL",		T_FPU_FMUL		},
	{ "FSMUL",		T_FPU_FSMUL		},
	{ "FDMUL",		T_FPU_FDMUL		},
	{ "FNEG",		T_FPU_FNEG		},
	{ "FSNEG",		T_FPU_FSNEG		},
	{ "FDNEG",		T_FPU_FDNEG		},
	{ "FNOP",		T_FPU_FNOP		},
	{ "FREM",		T_FPU_FREM		},
	{ "FRESTORE",	T_FPU_FRESTORE	},
	{ "FSAVE",		T_FPU_FSAVE		},
	{ "FSCALE",		T_FPU_FSCALE	},

	{ "FSF",	T_FPU_FSF	},
	{ "FSEQ",	T_FPU_FSEQ	},
	{ "FSOGT",	T_FPU_FSOGT	},
	{ "FSOGE",	T_FPU_FSOGE	},
	{ "FSOLT",	T_FPU_FSOLT	},
	{ "FSOLE",	T_FPU_FSOLE	},
	{ "FSOGL",	T_FPU_FSOGL	},
	{ "FSOR",	T_FPU_FSOR	},

	{ "FSUN",	T_FPU_FSUN	},
	{ "FSUEQ",	T_FPU_FSUEQ	},
	{ "FSUGT",	T_FPU_FSUGT	},
	{ "FSUGE",	T_FPU_FSUGE	},
	{ "FSULT",	T_FPU_FSULT	},
	{ "FSULE",	T_FPU_FSULE	},
	{ "FSNE",	T_FPU_FSNE	},
	{ "FST",	T_FPU_FST	},

	{ "FSSF",	T_FPU_FSSF	},
	{ "FSSEQ",	T_FPU_FSSEQ	},
	{ "FSGT",	T_FPU_FSGT	},
	{ "FSGE",	T_FPU_FSGE	},
	{ "FSLT",	T_FPU_FSLT	},
	{ "FSLE",	T_FPU_FSLE	},
	{ "FSGL",	T_FPU_FSGL	},
	{ "FSGLE",	T_FPU_FSGLE	},

	{ "FSNGLE",	T_FPU_FSNGLE	},
	{ "FSNGL",	T_FPU_FSNGL		},
	{ "FSNLE",	T_FPU_FSNLE		},
	{ "FSNLT",	T_FPU_FSNLT		},
	{ "FSNGE",	T_FPU_FSNGE		},
	{ "FSNGT",	T_FPU_FSNGT		},
	{ "FSSNE",	T_FPU_FSSNE		},
	{ "FSST",	T_FPU_FSST		},

	{ "FSGLDIV",	T_FPU_FSGLDIV	},
	{ "FSGLMUL",	T_FPU_FSGLMUL	},
	{ "FSIN",		T_FPU_FSIN		},
	{ "FSINCOS",	T_FPU_FSINCOS	},
	{ "FSINH",		T_FPU_FSINH		},
	{ "FSQRT",		T_FPU_FSQRT		},
	{ "FSSQRT",		T_FPU_FSSQRT	},
	{ "FDSQRT",		T_FPU_FDSQRT	},
	{ "FSUB",		T_FPU_FSUB		},
	{ "FSSUB",		T_FPU_FSSUB		},
	{ "FDSUB",		T_FPU_FDSUB		},
	{ "FTAN",		T_FPU_FTAN		},
	{ "FTANH",		T_FPU_FTANH		},
	{ "FTENTOX",	T_FPU_FTENTOX	},

	{ "FTRAPF",		T_FPU_FTRAPF	},
	{ "FTRAPEQ",	T_FPU_FTRAPEQ	},
	{ "FTRAPOGT",	T_FPU_FTRAPOGT	},
	{ "FTRAPOGE",	T_FPU_FTRAPOGE	},
	{ "FTRAPOLT",	T_FPU_FTRAPOLT	},
	{ "FTRAPOLE",	T_FPU_FTRAPOLE	},
	{ "FTRAPOGL",	T_FPU_FTRAPOGL	},
	{ "FTRAPOR",	T_FPU_FTRAPOR	},

	{ "FTRAPUN",	T_FPU_FTRAPUN	},
	{ "FTRAPUEQ",	T_FPU_FTRAPUEQ	},
	{ "FTRAPUGT",	T_FPU_FTRAPUGT	},
	{ "FTRAPUGE",	T_FPU_FTRAPUGE	},
	{ "FTRAPULT",	T_FPU_FTRAPULT	},
	{ "FTRAPULE",	T_FPU_FTRAPULE	},
	{ "FTRAPNE",	T_FPU_FTRAPNE	},
	{ "FTRAPT",		T_FPU_FTRAPT	},

	{ "FTRAPSF",	T_FPU_FTRAPSF	},
	{ "FTRAPSEQ",	T_FPU_FTRAPSEQ	},
	{ "FTRAPGT",	T_FPU_FTRAPGT	},
	{ "FTRAPGE",	T_FPU_FTRAPGE	},
	{ "FTRAPLT",	T_FPU_FTRAPLT	},
	{ "FTRAPLE",	T_FPU_FTRAPLE	},
	{ "FTRAPGL",	T_FPU_FTRAPGL	},
	{ "FTRAPGLE",	T_FPU_FTRAPGLE	},

	{ "FTRAPNGLE",	T_FPU_FTRAPNGLE	},
	{ "FTRAPNGL",	T_FPU_FTRAPNGL	},
	{ "FTRAPNLE",	T_FPU_FTRAPNLE	},
	{ "FTRAPNLT",	T_FPU_FTRAPNLT	},
	{ "FTRAPNGE",	T_FPU_FTRAPNGE	},
	{ "FTRAPNGT",	T_FPU_FTRAPNGT	},
	{ "FTRAPSNE",	T_FPU_FTRAPSNE	},
	{ "FTRAPST",	T_FPU_FTRAPST	},

	{ "FTST",		T_FPU_FTST		},
	{ "FTWOTOX",	T_FPU_FTWOTOX	},

	{ "FP0",	T_FPUREG_0 },
	{ "FP1",	T_FPUREG_1 },
	{ "FP2",	T_FPUREG_2 },
	{ "FP3",	T_FPUREG_3 },
	{ "FP4",	T_FPUREG_4 },
	{ "FP5",	T_FPUREG_5 },
	{ "FP6",	T_FPUREG_6 },
	{ "FP7",	T_FPUREG_7 },

	{ "FPSR",	T_FPU_FPSR  },
	{ "FPCR",	T_FPU_FPCR  },
	{ "FPIAR",	T_FPU_FPIAR },

	{ NULL, 0 }
};

void
m68k_DefineTokens(void) {
	lex_ConstantsDefineWords(g_tokens);
}
