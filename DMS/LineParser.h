#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <ctype.h>
#include "codes.h"
#include "cmd.h"
#include "dms_state.h"
#include "chunk.h"
#include "token.h"
#include "utils.h"
#include "Scope.h"
#include <stack>


namespace dms {
	struct tokenstream {
		std::vector<tokens::token> tokens;
		size_t pos = 0;
		void init(std::vector<tokens::token>* ptr);
		tokens::token next();
		void prev();
		std::vector<tokens::token> next(tokens::tokentype to,tokens::tokentype tc);
		tokens::token peek();
		tokens::token last();
		std::vector<tokens::token> next(tokens::tokentype tk);
		bool match(tokens::tokentype t1 = tokens::none, tokens::tokentype t2 = tokens::none, tokens::tokentype t3 = tokens::none, tokens::tokentype t4 = tokens::none, tokens::tokentype t5 = tokens::none, tokens::tokentype t6 = tokens::none, tokens::tokentype t7 = tokens::none, tokens::tokentype t8 = tokens::none, tokens::tokentype t9 = tokens::none, tokens::tokentype t10 = tokens::none, tokens::tokentype t11 = tokens::none, tokens::tokentype t12 = tokens::none);
		bool match(tokens::tokentype* t1 = nullptr, tokens::tokentype* t2 = nullptr, tokens::tokentype* t3 = nullptr, tokens::tokentype* t4 = nullptr, tokens::tokentype* t5 = nullptr, tokens::tokentype* t6 = nullptr, tokens::tokentype* t7 = nullptr, tokens::tokentype* t8 = nullptr, tokens::tokentype* t9 = nullptr, tokens::tokentype* t10 = nullptr, tokens::tokentype* t11 = nullptr, tokens::tokentype* t12 = nullptr);
		bool hasScope(size_t tabs);
		bool restore(size_t p) {
			pos = p;
			return false; // This is a convience for something I will be doing so much
		}
	};
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
		chunk* current_chunk = nullptr;
		std::string chunk_name;
		blocktype chunk_type = bt_block;
		std::stack<scope> scopes;
		size_t line = 1;
		std::vector<tokens::token> temp;
		std::vector<tokens::token> tdump;
		size_t tabs = 0;
		tokenstream* _stream;
		dms_state* state;
		void doCheck(passer* stream, std::vector<tokens::token>* t_vec, size_t line, bool& isNum, bool& hasDec, std::vector<uint8_t>* buffer);
		void _Parse(tokenstream stream);
		// Match Process Code
		bool match_process_debug(tokenstream* stream);
		bool match_process_disp(tokenstream* stream);
		bool match_process_choice(tokenstream* stream);
		bool match_process_function(tokenstream* stream, value* v=nullptr, bool nested = true);
		bool match_process_goto(tokenstream* stream);
		bool match_process_jump(tokenstream* stream);
		bool match_process_exit(tokenstream* stream);
		bool match_process_expression(tokenstream* stream, value* v);
		bool match_process_IFFF(tokenstream* stream);
		// Build
		void buildGoto(std::string g, bool v = false);
		void buildNoop();
		void buildLabel(std::string l);
		void buildSpeed(double s);
		void buildWait(double w);

		// Utils
		void badSymbol(errors::errortype err, tokenstream* stream);
		void badSymbol(tokenstream* stream);
		void badSymbol();

		void tokenDump(std::vector<tokens::token>* v);
		bool createBlock(std::string bk_name, blocktype bk_type);

		bool isBlock();
		bool isBlock(blocktype bk_type);
		void tolower(std::string &str);
		void tokenizer(dms_state* state, std::vector<tokens::token> &tok);
	public:
		//Refer to streams.cpp for the match_process_CMD code.
		dms_state* Parse();
		dms_state* Parse(std::string l);
		dms_state* Parse(dms_state* state, std::string l);
		LineParser(std::string fn);
		LineParser();
	};
}