#pragma once
#include <string>
#include <unordered_map>
#include "value.h"
#include "enviroment.h"
namespace dms {
	struct character : enviroment {
		bool seen = false;
		double spd = 100;
		bool fullname = true;
		std::unordered_map<std::string, std::string> paths;
		std::string getName();
	};
}