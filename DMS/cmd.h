#pragma once
#include "value.h"
#include "codes.h"
#include <vector>
namespace dms {
	struct cmd
	{
		dms::codes::op opcode;
		dms_args args;
	};
}
