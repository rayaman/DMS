#pragma once
#include <unordered_map>
#include <string>
#include "value.h"
namespace dms {
	struct memory
	{
		size_t count(std::string);
		void erase(std::string);
		value& operator[](std::string);
	private:
		std::unordered_map<std::string, value> mem;
	};
}
