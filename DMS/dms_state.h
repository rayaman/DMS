#pragma once
#include "errors.h"
#include "chunk.h"
#include <fstream>
#include <string>
#include <iostream>
#include <map>
namespace dms {
	class dms_state
	{
	public:
		dms_state();
		void dump(errors::error err);
		void dump();
		void push_error(errors::error err);
		void push_warning(errors::error err);
		void push_chunk(std::string s, chunk* c);
		double version=1.0;
		std::map<std::string, chunk*> chunks;
		std::string entry = "start";
		std::map<std::string, bool> enables;
		bool isEnabled(std::string flag);
	};
}
