#include "pch.h"
#include "utils.h"
namespace dms::utils {
	void print(const value val) {
		printf(val.getPrintable().c_str());
	}
	std::vector<std::string> split(const std::string& str, const std::string& delim)
	{
		std::vector<std::string> tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find(delim, prev);
			if (pos == std::string::npos) pos = str.length();
			std::string token = str.substr(prev, pos - prev);
			if (!token.empty()) tokens.push_back(token);
			prev = pos + delim.length();
		} while (pos < str.length() && prev < str.length());
		return tokens;
	}
	std::string concat(value& v1, value& v2, value& v3, value& v4, value& v5, value& v7, value& v8, value& v9, value& v10, value& v11, value& v12) {
		std::vector<value> list{v1,v2,v3,v4,v5,v7,v8,v9,v10,v11,v12};
		std::string str="";
		for (size_t i = 0; i < list.size(); i++) {
			if (list[i] != nullptr)
				str = (str + list[i].toString());
			else
				return str;
		}
		return str;
	}
	void sleep(unsigned int mseconds)
	{
		clock_t goal = mseconds + clock();
		while (goal > clock());
	}
	bool isalphanum(std::string str) {
		for (size_t i = 0; i < str.size(); i++) {
			if (!isalnum(str[i]) && str[i]!='_')
				return false;
		}
		return true;
	}
	bool isalpha(std::string str) {
		for (size_t i = 0; i < str.size(); i++) {
			if (!std::isalpha(str[i]) && str[i] != '_')
				return false;
		}
		return true;
	}
	bool isNum(std::string str) {
		for (size_t i = 0; i < str.size(); i++) {
			if (!std::isdigit(str[i]) && str[i] != '.')
				return false;
		}
		return true;
	}
	std::string random_string(size_t length)
	{
		auto randchar = []() -> char
		{
			const char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			const size_t max_index = (sizeof(charset) - 1);
			return charset[rand() % max_index];
		};
		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);
		return str;
	}
	bool valueassert(dms_args args, dms_state* state, datatypes t1, datatypes t2, datatypes t3, datatypes t4, datatypes t5, datatypes t6, datatypes t7, datatypes t8, datatypes t9, datatypes t10, datatypes t11, datatypes t12) {
		size_t size = args.args.size();
		datatypes types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		if (size >= 4)
			for (size_t i = 0; i < 4; i++) {
				if (args.args[i].resolve(state).type != types[i])
					return false;
			}
		else
			for (size_t i = 0; i < size; i++) {
				if (args.args[i].resolve(state).type != types[i])
					return false;
			}
		return true;
	}
	bool valueassertall(dms_args args, dms_state* state, datatypes t1) {
		size_t size = args.args.size();
		if (size >= 4)
			for (size_t i = 0; i < 4; i++) {
				if (args.args[i].resolve(state).type != t1)
					return false;
			}
		else
			for (size_t i = 0; i < size; i++) {
				if (args.args[i].resolve(state).type != t1)
					return false;
			}
		return true;
	}
	bool typeassert(dms_args args, datatypes t1, datatypes t2, datatypes t3, datatypes t4, datatypes t5, datatypes t6, datatypes t7, datatypes t8, datatypes t9, datatypes t10, datatypes t11, datatypes t12) {
		size_t size = args.args.size();
		datatypes types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		if(size >= 4)
			for (size_t i = 0; i < 4;i++) {
				if (args.args[i].type != types[i])
					return false;
			}
		else
			for (size_t i = 0; i < size; i++) {
				if (args.args[i].type != types[i])
					return false;
			}
		return true;
	}
	bool typeassert(dms_state* state, dms_args args, datatypes t1, datatypes t2, datatypes t3, datatypes t4, datatypes t5, datatypes t6, datatypes t7, datatypes t8, datatypes t9, datatypes t10, datatypes t11, datatypes t12) {
		size_t size = args.args.size();
		datatypes types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		if (size >= 4)
			for (size_t i = 0; i < 4; i++) {
				if (args.args[i].type != types[i]) {
					state->push_error(dms::errors::error{ errors::invalid_arguments, "Invalid arguments! Expected (" + resolveTypes(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) + ") Got: " + resolveTypes(args.args[i].type)});
					return false;
				}
			}
		else
			for (size_t i = 0; i < size; i++) {
				if (args.args[i].type != types[i]) {
					state->push_error(dms::errors::error{ errors::invalid_arguments, "Invalid arguments! Expected (" + resolveTypes(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) + ") Got: " + resolveTypes(args.args[i].type)});
					return false;
				}
			}
		return true;
	}
	std::string resolveTypes(datatypes t1, datatypes t2, datatypes t3, datatypes t4, datatypes t5, datatypes t6, datatypes t7, datatypes t8, datatypes t9, datatypes t10, datatypes t11, datatypes t12) {
		std::stringstream str;
		datatypes types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		for (size_t i = 0; i < 12; i++) {
			switch (types[i])
			{
			case string:
				str << "string, ";
				break;
			case boolean:
				str << "boolean, ";
				break;
			case env:
				str << "env, ";
				break;
			case number:
				str << "number, ";
				break;
			case custom:
				str << "custom, ";
				break;
			default:
				break;
			}
		}
		std::string temp = str.str();
		return temp.substr(1, temp.size() - 3);
	}
	void trim(std::string& s) {
		size_t p = s.find_first_not_of(" \t");
		s.erase(0, p);
		p = s.find_last_not_of(" \t");
		if (std::string::npos != p)
			s.erase(p + 1);
	}
	void wait() {
		std::cin.ignore();
	}
}