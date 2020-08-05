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
		UNWN,
		WHLE,
		FNWR,
		FNNR,
		IFFF,
		ELIF,
		ELSE,
		DEFN,
		SKIP,
		COMP,
		INDX,
		JMPZ,
		INST,
		ERRO
	};
	extern const std::string list[];
	static bool isControl(const op code) {
		return (code == STAT || code == CHOI || code == FORE || code == WHLE || code == IFFF || code == ELIF || code == ELSE);
	};
}