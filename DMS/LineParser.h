#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include<stdio.h>
#include<ctype.h>
#include "codes.h"
#include "cmd.h"
#include "dms_state.h"
#include "chunk.h"
#include "token.h"
#include "utils.h"

namespace dms {
	struct passer {
		std::string stream;
		uint8_t next();
		void next(uint8_t c);
		uint8_t prev();
		uint8_t peek();
		std::string processBuffer(std::vector<uint8_t> buf);
	private:
		size_t pos = 0;
	};
	class LineParser
	{
		std::string fn;
	public:
		dms_state* Parse();
		dms_state* Parse(std::string l);
		dms_state* Parse(dms_state* state, std::string l);
		LineParser(std::string fn);
		LineParser();
		void tolower(std::string &str);
		std::vector<chunk> tokenizer(std::vector<tokens::token> &tok);
		cmd* getPattern(std::vector<tokens::token>& tok);
	};
}