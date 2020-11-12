#include "enviroment.h"
namespace dms {
	bool enviroment::has(std::string index) {
		return values.count(index);
	}
	void enviroment::set(std::string index, value val) {
		values[index] = val;
	}
	value enviroment::get(std::string index) {
		return values[index];
	}
}