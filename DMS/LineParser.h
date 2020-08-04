#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include "codes.h"
#include "cmd.h"
#include "dms_state.h"
#include "chunk.h"
#include "token.h"

namespace dms {
	class LineParser
	{
		std::string fn;
		std::vector<chunk>* chunks = new std::vector<chunk>();
		cmd* tokenizer(std::vector<tokens::token>* tok);
	public:
		dms_state* Parse();
		dms_state* Parse(std::string l);
		dms_state* Parse(dms_state* state, std::string l);
		LineParser(std::string fn);
		LineParser();
	};
}