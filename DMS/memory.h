#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "value.h"
namespace dms {
	struct memory
	{
		size_t count(std::string);
		void erase(std::string);
		value& operator[](std::string);
		std::vector<value> examine(datatypes dt);
	private:
		std::unordered_map<std::string, value> mem;
	};
}
