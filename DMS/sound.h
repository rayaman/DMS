#pragma once
#include <SFML/Audio.hpp>
#include "dms_state.h"
namespace dms::audio {
	void init(dms_state*);
	value loadMusic(void*, dms_state*, dms_args*);
	// I want to do more with this, like effects and stuff. For now only simple things are implemented
}