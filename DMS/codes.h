#pragma once
#include <vector>
#include <string>
namespace dms::codes {
	enum op {
		NOOP,
		ENTR,
		ENAB,
		DISA,
		LOAD,
		VERN,
		USIN,
		STAT,
		DISP,
		ASGN,
		LABL,
		CHOI,
		OPTN,
		FORE,
		WHLE,
		FUNC,
		IFFF,
		ELIF,
		ELSE,
		DEFN,
		SKIP,
		COMP,
		INDX,
		JMPZ,
		INST,
		ERRO,
		GOTO,
		JUMP,
		RETN,
		EXIT,
		DEBG,
		DSPD,
		DACT,
		WAIT,
		APND,
		SSPK,
		ADD,
		SUB,
		MUL,
		DIV,
		POW,
		MOD,
		LIST,
		LINE,
		HALT,
		FILE,
		GC
	};
	extern const std::string list[];
	static bool isControl(const op code) {
		return (code == STAT || code == CHOI || code == FORE || code == WHLE || code == IFFF || code == ELIF || code == ELSE);
	};
}