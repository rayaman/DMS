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
		uint8_t next() {
			if (stream.size() == pos) {
				return NULL;
			}
			else {
				return stream[pos++];
			}
		}
		void next(uint8_t c) {
			next();
			while (peek() != c) {
				next();
			}
		}
		uint8_t prev() {
			if (0 == pos) {
				return NULL;
			}
			return stream[--pos];
		}
		uint8_t peek() {
			if (stream.size() == pos) {
				return NULL;
			}
			return stream[pos];
		}
		std::string processBuffer(std::vector<uint8_t> buf) {
			return std::string(buf.begin(),buf.end());
		}
	private:
		size_t pos = 0;
	};
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