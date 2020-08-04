#pragma once
#include <string>
#include "errors.h"
namespace dms {
	class dms_state
	{
	public:
		void push_error(errors::error err) {};
	};
}
