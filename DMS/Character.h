#pragma once
#include <string>
#include <unordered_map>
#include "value.h"
namespace dms {
	struct character {
		std::string fname = "";
		std::string lname = "";
		std::string unknown = "Unknown";
		std::string nickname = "";
		bool seen = false;
		double spd = 100;
		std::unordered_map<std::string, std::string> paths;
		std::unordered_map<std::string, value*> values;
		std::string getName();
	};
}