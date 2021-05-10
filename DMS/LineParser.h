#pragma once
#include <vector>
#include <fstream>
#include "pch.h"
#include "codes.h"
#include "cmd.h"
#include "dms_state.h"
#include "chunk.h"
#include "token.h"
#include "utils.h"
#include "errors.h"
#include "comparisons.h"

namespace dms {
	struct value;
	struct tokenstream {
		tokenstream();
		tokenstream(std::vector<tokens::token>*);
		std::vector<tokens::token> tokens;
		size_t pos = 0;
		std::stack<size_t> spos;
		std::stack<size_t> stack;
		void init(std::vector<tokens::token>* ptr);
		tokens::token next();
		void prev();
		void chomp(tokens::tokentype t);
		std::vector<tokens::token> next(tokens::tokentype to, tokens::tokentype tc);
		std::vector<tokens::token> next(tokens::tokentype to, tokens::tokentype tc, bool nonewline);
		tokens::token peek();
		tokens::token last();
		std::vector<tokens::token> next(tokens::tokentype tk);
		bool can();
		bool match(tokens::tokentype t1 = tokens::none, tokens::tokentype t2 = tokens::none, tokens::tokentype t3 = tokens::none, tokens::tokentype t4 = tokens::none, tokens::tokentype t5 = tokens::none, tokens::tokentype t6 = tokens::none, tokens::tokentype t7 = tokens::none, tokens::tokentype t8 = tokens::none, tokens::tokentype t9 = tokens::none, tokens::tokentype t10 = tokens::none, tokens::tokentype t11 = tokens::none, tokens::tokentype t12 = tokens::none);
		bool match(tokens::tokentype* t1 = nullptr, tokens::tokentype* t2 = nullptr, tokens::tokentype* t3 = nullptr, tokens::tokentype* t4 = nullptr, tokens::tokentype* t5 = nullptr, tokens::tokentype* t6 = nullptr, tokens::tokentype* t7 = nullptr, tokens::tokentype* t8 = nullptr, tokens::tokentype* t9 = nullptr, tokens::tokentype* t10 = nullptr, tokens::tokentype* t11 = nullptr, tokens::tokentype* t12 = nullptr);
		bool hasScope(size_t tabs);
		void store(chunk* c);
		bool restore(cmd* c, chunk* cnk);
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
		bool stop = false;
		std::string fn;
		chunk* current_chunk = nullptr;
		std::string chunk_name;
		blocktype chunk_type = bt_block;
		size_t line = 1;
		std::vector<tokens::token> temp;
		std::vector<tokens::token> tdump;
		size_t tabs = 0;
		tokenstream* _stream = nullptr;
		dms_state* state = nullptr;
		void doCheck(passer* stream, std::vector<tokens::token>* t_vec, size_t line, bool& isNum, bool& hasDec, std::vector<uint8_t>* buffer);
		void _Parse(tokenstream* stream);
		bool ParseLoop(tokenstream* stream, size_t count=0);
		std::stack<std::string> lastCall;
		// Block Handling Code
		bool match_process_blocks(tokenstream* stream);
		// Match Process Code
		bool match_process_debug(tokenstream* stream);
		bool match_process_disp(tokenstream* stream);
		bool match_process_choice(tokenstream* stream);
		bool match_process_function(tokenstream* stream, value& v, bool nested = true);
		bool match_process_goto(tokenstream* stream);
		bool match_process_jump(tokenstream* stream);
		bool match_process_exit(tokenstream* stream);
		bool match_process_expression(tokenstream* stream, value& v);
		bool match_process_IFFF(tokenstream* stream);
		bool match_process_ELIF(tokenstream* stream,std::string);
		bool match_process_ELSE(tokenstream* stream,std::string);
		bool match_process_assignment(tokenstream* stream);
		bool match_process_list(tokenstream* stream, value& v);
		bool match_process_wait(tokenstream* stream);
		bool match_process_standard(tokenstream* stream, value& v); // All types that are matchable are handled here!
		bool match_process_index(tokenstream* stream,value& v, bool leftside = false);
		bool match_process_return(tokenstream* stream);
		bool match_process_condition(tokenstream* stream, value& v);
		bool match_process_andor(tokenstream* stream,value& v);
		bool match_process_scope(tokenstream* stream);
		bool match_process_while(tokenstream* stream);
		bool match_process_for(tokenstream* stream);
		bool match_process_number(tokenstream* stream, value& v);
		bool match_process_asm(tokenstream* stream);
		bool match_process_1afunc(tokenstream* stream, value& v);
		bool match_process_ooxoo(tokenstream* stream, value& v);

		// Build
		void buildGoto(std::string g, bool v = false);
		void buildJump(std::string j, bool v = false);
		void buildNoop();
		void buildLabel(std::string l);
		void buildSpeed(double s);
		void buildWait(double w);
		void buildCmd(codes::op,std::vector<value>);

		// Utils
		std::string random_string(std::size_t length);
		value stov(std::string);

		bool manageCount(bool cond, size_t c,size_t&);
		bool notBalanced(std::vector<tokens::token> ts, size_t last_line, tokenstream* stream, std::string o, std::string c);
		void badSymbol(errors::errortype err, tokenstream* stream);
		void badSymbol(tokenstream* stream);
		void badSymbol();

		void tokenDump(std::vector<tokens::token>* v);
		bool createBlock(std::string bk_name, blocktype bk_type);

		bool isBlock();
		bool isBlock(blocktype bk_type);
		void tolower(std::string &str);
		void tokenizer(dms_state* state, std::vector<tokens::token> &tok);
		void debugInvoker(tokenstream* stream);
	public:
		//Refer to streams.cpp for the match_process_CMD code.
		dms_state* Parse();
		dms_state* Parse(std::string l);
		dms_state* Parse(dms_state* state, std::string l);
		LineParser(std::string fn);
		LineParser();
	};
}