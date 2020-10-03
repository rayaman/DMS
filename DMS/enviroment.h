#pragma once
#include <string>
#include <unordered_map>
#include "value.h"
namespace dms {
	enum class env_type {
		env,
		character,
		function
	};
	struct enviroment {
		std::string name="";
		std::unordered_map<std::string, value*> values;
		env_type type = env_type::env;
		bool has(std::string index);
		void set(std::string index, value* val);
		value* get(std::string index);
	};
}