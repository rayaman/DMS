#pragma once
#include <string>
#include <unordered_map>
#include "value.h"
namespace dms {
	struct character {
		std::string fname = "";
		std::string lname = "";
		bool seen = false;
		std::unordered_map<std::string, std::string> paths;
		std::unordered_map<std::string, value*> values;
	};
}