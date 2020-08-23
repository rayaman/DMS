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
		std::vector<tokens::token> next(tokens::tokentype to,tokens::tokentype tc);
		tokens::token peek();
		std::vector<tokens::token> next(tokens::tokentype tk);
		bool match(tokens::tokentype t1 = tokens::none, tokens::tokentype t2 = tokens::none, tokens::tokentype t3 = tokens::none, tokens::tokentype t4 = tokens::none, tokens::tokentype t5 = tokens::none, tokens::tokentype t6 = tokens::none, tokens::tokentype t7 = tokens::none, tokens::tokentype t8 = tokens::none, tokens::tokentype t9 = tokens::none, tokens::tokentype t10 = tokens::none, tokens::tokentype t11 = tokens::none, tokens::tokentype t12 = tokens::none);
		bool match(tokens::tokentype* t1 = nullptr, tokens::tokentype* t2 = nullptr, tokens::tokentype* t3 = nullptr, tokens::tokentype* t4 = nullptr, tokens::tokentype* t5 = nullptr, tokens::tokentype* t6 = nullptr, tokens::tokentype* t7 = nullptr, tokens::tokentype* t8 = nullptr, tokens::tokentype* t9 = nullptr, tokens::tokentype* t10 = nullptr, tokens::tokentype* t11 = nullptr, tokens::tokentype* t12 = nullptr);
		bool hasScope(size_t tabs);
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
	public:
		dms_state* Parse();
		dms_state* Parse(std::string l);
		dms_state* Parse(dms_state* state, std::string l);
		LineParser(std::string fn);
		LineParser();
		//Matches tokens from the stream, if the tokens match it will return true and YOU should call next on the stream. This method does not change the current position
		
		bool buildLabel(chunk c, std::string label);

		bool processFunc(tokenstream stream, chunk c);
		bool processFunc(tokenstream stream, chunk c, std::string gotoo);
		bool processExpr(tokenstream stream, chunk c);
		bool processLogic(tokenstream stream, chunk c);

		void tolower(std::string &str);
		tokens::tokentype* expr();
		tokens::tokentype* variable();
		std::map<std::string, chunk*> tokenizer(dms_state* state, std::vector<tokens::token> &tok);
	};
}