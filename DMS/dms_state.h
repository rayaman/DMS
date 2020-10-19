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
namespace dms {
	struct Handler;
	struct dms_state
	{
		Handler* handler = nullptr;
		bool hasFirst = false;
		Invoker invoker;
		std::unordered_map<std::string, value*> memory;
		std::vector<value*> garbage;
		std::unordered_map<std::string, chunk*> chunks;
		std::unordered_map<std::string, character*> characters;
		std::unordered_map<std::string, enviroment*> enviroments;
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
		bool run(std::string instance);
		double version = 1.0;
		void enable(std::string flag);
		void disable(std::string flag);
		bool isEnabled(std::string flag);
		
		void setHandler(Handler* hand);

		bool Inject(std::string var, value* val);

		// Gets or creates a character
		character* getCharacter(std::string c);
		enviroment* getEnviroment(std::string c);

		bool assign(std::unordered_map<std::string, value*>* mem,value* var, value* val);
		size_t seek(std::string label,std::vector<cmd*> cmds ,codes::op code, size_t pos);
		bool characterExists(std::string bk_name);
		bool blockExists(std::string bk_name);
		bool typeAssert(value* val, datatypes type);
		bool run();
		bool run(std::string ent,std::unordered_map<std::string, value*>* mem);
		// This is called once and once only. Dynamically loading code is not a thing!
		void init();

		bool hasError();
	private:
		// From what I gathered
		//std::mutex memory_mutex;
		bool stop = false;
		bool init_init = false;
		void init(chunk* chunk, size_t &pos,size_t &max, std::vector<cmd*>& cmds);
	};
}
