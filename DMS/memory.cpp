#include "memory.h"
#include <iostream>
namespace dms {
	value& memory::operator[](std::string str) {
		return mem[str];
	}
	size_t memory::count(std::string str) {
		return mem.count(str);
	}
	void memory::erase(std::string str) {
		mem.erase(str);
	}
}