#pragma once
#include <vector>
#include <string>
namespace dms::codes {
	enum op {
		NOOP, // Done
		ENTR, // Done
		ENAB, // Done
		DISA, // Done
		LOAD, // Done
		VERN, // Done
		USIN, // TODO
		STAT,
		DISP, // Done
		ASGN, // Done
		LABL,
		CHOI, // Done
		OPTN,
		FORE,
		WHLE,
		FUNC, // Done
		IFFF,
		ELIF,
		ELSE,
		DEFN,
		SKIP,
		COMP,
		INDX, // Done
		JMPZ,
		INST, // Done
		ERRO,
		GOTO,
		JUMP, // Done
		RETN,
		EXIT, // Done
		DEBG,
		DSPD, // Todo
		DACT,
		WAIT, // Done
		APND, // Done
		SSPK, // Done
		ADD, // Done
		SUB, // Done
		MUL, // Done
		DIV, // Done
		POW, // Done
		MOD, // Done
		LIST, // Done
		LINE, // Done
		HALT, // Todo
		FILE,
		GC,
		ASID, // Done
		OFUN // Done
	};
	extern const std::string list[];
	static bool isControl(const op code) {
		return (code == STAT || code == CHOI || code == FORE || code == WHLE || code == IFFF || code == ELIF || code == ELSE);
	};
}