#pragma once
#include "value.h"
#include "utils.h"
#include <iostream>
namespace dms {
	struct choiceHandler
	{
		virtual uint8_t manageChoice(dms_state* state, dms_args choices) const;
	};
}
