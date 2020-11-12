#pragma once
#include "memory.h"
namespace dms {
	struct dms_list
	{
		memory hpart;
		std::vector<value> ipart;
		void pushValue(value val);
		value getValue(value val);
	};
}