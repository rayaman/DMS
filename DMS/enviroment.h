#pragma once
#include <string>
#include <unordered_map>
#include "value.h"
namespace dms {
	struct enviroment {
		std::string name="";
		std::unordered_map<std::string, value*> values;
		bool has(std::string index);
		void set(std::string index, value* val);
		value* get(std::string index);
	};
}