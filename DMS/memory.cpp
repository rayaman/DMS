#include "pch.h"
#include "memory.h"
#include "utils.h"
namespace dms {
	value& memory::operator[](std::string str) {
		/*if (!mem[str].isNil())
			return mem[str];
		else if (parent != nullptr && parent->count(str)) {
			return parent->mem[str];
		}*/
		return mem[str];
	}
	std::vector<value> memory::examine(datatypes dt)
	{
		std::vector<value> arr;
		for (const auto& p : mem)
		{
			if (p.second.type == dt) {
				arr.push_back(p.second);
			}
		}
		return arr;
	}
	size_t memory::count(std::string str) {
		return mem.count(str);
	}
	void memory::erase(std::string str) {
		mem.erase(str);
	}
	memory::memory(const memory& other) {
		mem = other.mem;
		parent = other.parent;
	}
	memory::memory(){}
}