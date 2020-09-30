#pragma once
#include "errors.h"
#include "chunk.h"
#include "dms_exceptions.h"
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
// Threading Stuff
#include <thread>
#include <mutex>
#include <chrono>
#include "Character.h"
namespace dms {
	struct dms_state
	{
		void* handler = nullptr;
		bool hasFirst = false;
		std::unordered_map<std::string, value*> memory;
		std::vector<value*> garbage;
		std::map<std::string, chunk*> chunks;
		std::map<std::string, character*> characters;
		std::map<std::string, size_t> labels;
		std::string entry = "$undefined";
		std::map<std::string, bool> enables;
		std::size_t cur_line=0;
		const double Iversion = 1.0;
		double Sversion; // The version of
		errors::error err;
		bool stop = false;

		dms_state();
		void dump(errors::error err);
		void dump(bool print=false);
		void push_error(errors::error err);
		void push_warning(errors::error err);
		void push_chunk(std::string s, chunk* c);
		bool run(std::string instance);
		double version=1.0;
		void enable(std::string flag);
		void disable(std::string flag);
		bool isEnabled(std::string flag);
		void setHandler(void* hand);

		// Gets or creates a character
		character* getCharacter(std::string c);

		bool assign(value* var, value* val);
		size_t seek(std::string label,std::vector<cmd*> cmds ,codes::op code, size_t pos);
		bool characterExists(std::string bk_name);
		bool blockExists(std::string bk_name);
		bool typeAssert(value* val, datatypes type);
		bool run();
	private:
		// From what I gathered
		//std::mutex memory_mutex;
		void init(chunk* chunk, size_t &pos,size_t &max, std::vector<cmd*>& cmds);
	};
}
