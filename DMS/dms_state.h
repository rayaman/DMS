#pragma once
#include "errors.h"
#include "chunk.h"
#include <string>
#include <iostream>
namespace dms {
	class dms_state
	{
	public:
		void push_error(errors::error err);
		void push_warning(errors::error err);
		double version=1.0;
		std::map<std::string, chunk*> chunks;
		std::string entry = "start";
		std::map<std::string, bool> enables;
	};
}
