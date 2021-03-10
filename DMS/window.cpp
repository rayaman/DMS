#include "pch.h"
#include "window.h"
namespace dms::window {
	Invoker* inv = new Invoker;
	void init(dms_state* state) {
		/*
		inv->registerFunction("getStatus", getStatus);

		state->assoiateType("audiostream",inv);
		state->invoker.registerFunction("loadMusic", loadMusic);
		*/
	}
	value newWindow(void* self, dms_state* state, dms_args* args) {
		if (dms::utils::valueassert(*args, state, datatypes::number, datatypes::number, datatypes::number)) {

		}
		else if (dms::utils::valueassert(*args, state, datatypes::int64, datatypes::int64, datatypes::int64)) {

		}
		sf::RenderWindow window(sf::VideoMode(1024, 768, 32), "Background Test");
	}
}