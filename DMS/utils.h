#pragma once
#include "pch.h"
#include "value.h"
#include "dms_state.h"
namespace dms {
	namespace utils {
		template<class... Args>
		void debug(const Args... args) {
			(std::cout << ... << args) << std::endl;
		}
		template<class... Args>
		void write(const Args... args) {
			(std::cout << ... << args);
		}
		void print(const value values...);
		template<class... Args>
		std::string concat(Args... args) {
			std::stringstream str;
			(str << ... << args);
			return str.str();
		}
		std::vector<std::string> split(const std::string& str, const std::string& delim);
		void sleep(unsigned int mseconds);
		std::string random_string(size_t length);
		bool typeassert(dms_args args, datatypes t1 = nil, datatypes t2 = nil, datatypes t3 = nil, datatypes t4 = nil, datatypes t5 = nil, datatypes t6 = nil, datatypes t7 = nil, datatypes t8 = nil, datatypes t9 = nil, datatypes t10 = nil, datatypes t11 = nil, datatypes t12 = nil); //Type asserting is mostly an internal thing for build in methods. It's not needed for dms code!
		bool typeassert(dms_state* state, dms_args args, datatypes t1 = nil, datatypes t2 = nil, datatypes t3 = nil, datatypes t4 = nil, datatypes t5 = nil, datatypes t6 = nil, datatypes t7 = nil, datatypes t8 = nil, datatypes t9 = nil, datatypes t10 = nil, datatypes t11 = nil, datatypes t12 = nil);
		bool valueassert(dms_args args, dms_state* state, datatypes t1 = nil, datatypes t2 = nil, datatypes t3 = nil, datatypes t4 = nil, datatypes t5 = nil, datatypes t6 = nil, datatypes t7 = nil, datatypes t8 = nil, datatypes t9 = nil, datatypes t10 = nil, datatypes t11 = nil, datatypes t12 = nil);
		bool valueassertall(dms_args args, dms_state* state, datatypes t1);
		std::string resolveTypes(datatypes t1 = nil, datatypes t2 = nil, datatypes t3 = nil, datatypes t4 = nil, datatypes t5 = nil, datatypes t6 = nil, datatypes t7 = nil, datatypes t8 = nil, datatypes t9 = nil, datatypes t10 = nil, datatypes t11 = nil, datatypes t12 = nil);
		bool isalphanum(std::string str);
		bool isalpha(std::string str);
		bool isNum(std::string str);
		void trim(std::string& s);
		void wait();
	}
}