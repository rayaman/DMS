#pragma once
#include "pch.h"
#include "value.h"
#include "memory.h"
#include "Invoker.h"
namespace dms {
	struct enviroment : Invoker {
		std::string name = "";
		memory values;
		bool has(std::string index);
		void set(std::string index, value val);
		value get(std::string index);
	};
}