#pragma once
#include "pch.h"
#include "value.h"
#include "enviroment.h"
namespace dms {
	struct character : enviroment {
		character();
		bool seen = false;
		double spd = 100;
		bool fullname = true;
		std::unordered_map<std::string, std::string> paths;
		std::string getName();
	};
}