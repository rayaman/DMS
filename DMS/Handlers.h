#pragma once
#include "value.h"
#include "utils.h"
#include "character.h"
#include <iostream>
namespace dms {
	struct Handler
	{
		virtual uint8_t handleChoice(dms_state* state, std::string prompt, std::vector<std::string> args) const;
		virtual bool handleSpeaker(dms_state* state, character* chara) const;
		virtual bool handleMessageDisplay(dms_state* state, std::string msg) const;
		virtual bool handleMessageAppend(dms_state* state, std::string msg) const;
		//virtual bool manageMessage(dms_state* state, value* msg) const;

		// Simple Events: only one connector!
		virtual bool OnSpeakerCreated(dms_state* state, character* chara) const;
		virtual bool OnStateInit(dms_state* state) const;
	};
}
