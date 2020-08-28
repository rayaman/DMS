#pragma once
#include "errors.h"
#include "chunk.h"
#include <string>
#include <iostream>
#include <unordered_map>
namespace dms {
	class dms_state
	{
	public:
		void push_error(errors::error err);
		void push_warning(errors::error err);
		double version=1.0;
		std::unordered_map<std::string, chunk*> chunks;
		std::string entry = "start";
		std::unordered_map<std::string, bool> enables;
	};
}
