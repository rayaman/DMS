#pragma once
#include "value.h"
#include "utils.h"
#include "Character.h"
#include <iostream>
namespace dms {
	struct Handler
	{
		virtual uint8_t manageChoice(dms_state* state, std::string prompt, std::vector<std::string> args) const;
		//virtual bool manageSpeaker(dms_state* state, character* chara) const;
		//virtual bool manageMessage(dms_state* state, value* msg) const;
	};
}
