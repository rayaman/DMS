#pragma once
#include <string>
#include "errors.h"
#include <map>
#include <iostream>
namespace dms {
	class dms_state
	{
	public:
		void push_error(errors::error err);
		void push_warning(errors::error err);
		double version=1.0;
		std::string entry = "start";
		std::map<std::string, bool> enables;
	};
}
