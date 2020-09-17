#pragma once
#include "errors.h"
#include "chunk.h"
#include <fstream>
#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
namespace dms {
	struct dms_state
	{
		std::unordered_map<std::string, value*> memory;
		std::map<std::string, chunk*> chunks;
		std::string entry = "$undefined";
		std::map<std::string, bool> enables;
		errors::error err;
		bool stop = false;

		dms_state();
		void dump(errors::error err);
		void dump();
		void push_error(errors::error err);
		void push_warning(errors::error err);
		void push_chunk(std::string s, chunk* c);
		bool run(std::string instance = "Main");
		double version=1.0;
		void enable(std::string flag);
		void disable(std::string flag);
		bool isEnabled(std::string flag);
	};
}
