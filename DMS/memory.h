#pragma once
#include "pch.h"
#include "value.h"
namespace dms {
	struct memory
	{
		size_t count(std::string);
		void erase(std::string);
		value& operator[](std::string);
		std::vector<value> examine(datatypes dt);
		memory* parent = nullptr;
		memory(const memory& other);
		memory();
	private:
		std::unordered_map<std::string, value> mem;
	};
}
