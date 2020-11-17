#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include "Invoker.h"
#include "errors.h"
#include "chunk.h"
#include "dms_exceptions.h"
#include "Character.h"
#include "enviroment.h"
#include "memory.h"
#include <stack>
#include "dms_list.h"
#include "comparisons.h"
namespace dms {
	struct Handler;
	value blockInvoke(void*, dms_state*, dms_args*);
	struct dms_state
	{
		friend class LineParser;
		Handler* handler = nullptr;
		bool hasFirst = false;
		Invoker invoker;
		std::stack<std::string> call_stack;
		std::stack<value> return_stack;
		std::stack<memory> mem_stack;
		std::vector<value*> garbage;
		std::unordered_map<std::string, chunk*> chunks;
		std::unordered_map<std::string, character*> characters;
		std::unordered_map<std::string, enviroment*> environments;
		std::unordered_map<std::string, size_t> labels;
		std::string entry = "$undefined";
		std::unordered_map<std::string, bool> enables;
		std::size_t cur_line=0;
		int exitcode = 0;
		const double Iversion = 1.0;
		double Sversion; // The version of
		errors::error err;

		dms_state();
		void dump(errors::error err);
		void dump(bool print=false);
		void push_error(errors::error err);
		void push_warning(errors::error err);
		void push_chunk(std::string s, chunk* c);
		double version = 1.0;
		void enable(std::string flag);
		void disable(std::string flag);
		bool isEnabled(std::string flag);
		
		void setHandler(Handler* hand);

		bool Inject(std::string var, value* val);

		// Gets or creates a character
		character* getCharacter(std::string c);
		enviroment* getEnvironment(std::string c);

		bool assign(value var, value val);
		size_t seek(std::string label,std::vector<cmd*> cmds ,codes::op code, size_t pos);
		bool characterExists(std::string bk_name);
		bool environmentExists(std::string bk_name);
		bool functionExists(std::string bk_name);
		bool blockExists(std::string bk_name);
		bool typeAssert(value val, datatypes type);
		memory* getMem();
		void pushMem();
		void popMem();
		bool run();
		bool run(std::string ent,memory* mem);
		bool run(std::string instance);
		// This is called once and once only. Dynamically loading code is not a thing!
		void init();
		bool hasError();
	private:
		// From what I gathered
		//std::mutex memory_mutex;
		void pushMem(memory&);
		bool stop = false;
		bool init_init = false;
		void init(chunk* chunk, size_t &pos,size_t &max, std::vector<cmd*>& cmds);
	};
}
