#pragma once
#include "pch.h"
#include "Invoker.h"
#include "errors.h"
#include "chunk.h"
#include "dms_exceptions.h"
#include "Character.h"
#include "enviroment.h"
#include "memory.h"
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
		std::unordered_map<std::string, Invoker*> inv_map;
		std::string entry = "$undefined";
		std::unordered_map<std::string, bool> enables;
		std::size_t cur_line=0;
		std::string cur_file = "";
		int exitcode = 0;
		const double Iversion = 1.0;
		double Sversion; // The version of
		errors::error err;
		character* speaker = nullptr;
		

		dms_state();
		void dump(std::string fn = "dump.bin");
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

		bool assoiateType(std::string type, Invoker* inv);
		bool injectEnv(std::string, enviroment*);
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
		bool next(memory* mem);
		bool run(std::string ent,memory* mem);
		bool run(std::string instance);
		bool error(std::string);
		// This is called once and once only. Dynamically loading code is not a thing!
		void init();
		bool hasError();
	private:
		// From what I gathered
		//std::mutex memory_mutex;

		// Next links
		codes::op n_code;
		cmd* n_c = nullptr;
		bool n_halt = false;
		size_t n_pos = 0;
		size_t n_max = 0;
		std::vector<cmd*> n_cmds;
		size_t n_ln = 0;
		std::string n_temp;
		//
		void pushMem(memory&);
		bool stop = false;
		bool init_init = false;
		void init(chunk* chunk, size_t &pos,size_t &max, std::vector<cmd*>& cmds);
	};
}
