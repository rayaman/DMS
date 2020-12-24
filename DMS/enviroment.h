#pragma once
#include "pch.h"
#include "value.h"
#include "memory.h"
#include "Invoker.h"
namespace dms {
	enum class env_type {
		env,
		character,
		function
	};
	struct enviroment : Invoker {
		std::string name = "";
		memory values;
		env_type type = env_type::env;
		bool has(std::string index);
		void set(std::string index, value val);
		value get(std::string index);
	};
}