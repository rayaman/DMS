#pragma once
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <string>
#include "value.h"
namespace dms::utils {
	template<class... Args>
	void print(Args... args) {
		(std::cout << ... << args) << "\n";
	}
	template<class... Args>
	std::string concat(Args... args) {
		std::stringstream str;
		(str << ... << args);
		return str.str();
	}
	bool typeassert(dms_args args, datatypes t1=nil, datatypes t2 = nil, datatypes t3 = nil, datatypes t4 = nil, datatypes t5 = nil, datatypes t6 = nil, datatypes t7 = nil, datatypes t8 = nil, datatypes t9 = nil, datatypes t10 = nil, datatypes t11 = nil, datatypes t12 = nil); //Type asserting is mostly an internal thing for build in methods. It's not needed for dms code!
}