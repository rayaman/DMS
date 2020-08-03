#pragma once
#include "codes.h"
#include "byte.h"
#include "cmd.h"
#include <vector>
namespace dms {
	class LineParser
	{
		std::string fn;
	public:
		std::vector<dms::cmd> Parse();
		std::vector<dms::cmd> Parse(std::string l);
		LineParser(std::string fn);
	};
}
/*
NOOP
ENTR
ENAB
DISA
LOAD
VERN
USIN
STAT
DISP
ASGN
LABL
CHOI
OPTN
FORE
UNWN
WHLE
FNWR
FNNR
IFFF
ELIF
ELSE
DEFN
SKIP
COMP
INDX
JMPZ
INST
*/
