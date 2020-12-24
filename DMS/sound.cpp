#include "pch.h"
#include "sound.h"

//sf::Music music;
//if (!music.openFromFile("test2.ogg"))
//return -1; // error
//music.setPitch(.6);
//music.setLoop(true);
//music.play();

namespace dms::audio {
	value play(void* self, dms_state* state, dms_args* args) {
		// Self, since it is a custom object will be equal to whatever we declared to the c pointer!
		sf::Music* music = (sf::Music*)self;
		music->play();
		return value();
	}
	value pause(void* self, dms_state* state, dms_args* args) {
		sf::Music* music = (sf::Music*)self;
		music->pause();
		return value();
	}
	value stop(void* self, dms_state* state, dms_args* args) {
		sf::Music* music = (sf::Music*)self;
		music->stop();
		return value();
	}
	value setLoop(void* self, dms_state* state, dms_args* args) {
		if (args->size() || args->args[0].resolve(state).type == datatypes::boolean) {
			sf::Music* music = (sf::Music*)self;
			music->setLoop(args->args[0].resolve(state).b);
			return NULL;
		}
	}
	value setPitch(void* self, dms_state* state, dms_args* args) {
		if (args->size() || args->args[0].resolve(state).isNum()) {
			sf::Music* music = (sf::Music*)self;
			music->setPitch(args->args[0].resolve(state).getDouble());
			return NULL;
		}
	}
	value setVolume(void* self, dms_state* state, dms_args* args) {
		if (args->size() || args->args[0].resolve(state).isNum()) {
			sf::Music* music = (sf::Music*)self;
			music->setVolume(args->args[0].resolve(state).getDouble());
			return NULL;
		}
	}
	value getStatus(void* self, dms_state* state, dms_args* args) {
		sf::Music* music = (sf::Music*)self;
		if (music->getStatus() == sf::Music::Status::Paused) {
			return "paused";
		}
		else if (music->getStatus() == sf::Music::Status::Playing) {
			return "playing";
		}
		else {
			return "stopped";
		}
	}
	// The customtype will be "sound"
	// We need to assoiate the sound type with an invoker so the interperter knows how to handle it

	Invoker* inv = new Invoker;
	
	void init(dms_state* state) {
		inv->registerFunction("play",play);
		inv->registerFunction("resume",play);
		inv->registerFunction("pause", pause);
		inv->registerFunction("stop", stop);
		inv->registerFunction("setLoop",setLoop);
		inv->registerFunction("setPitch", setPitch);
		inv->registerFunction("setVolume", setVolume);
		inv->registerFunction("getStatus", getStatus);

		state->assoiateType("audiostream",inv);
		state->invoker.registerFunction("loadMusic", loadMusic);
	}

	value loadMusic(void* self, dms_state* state, dms_args* args)
	{
		if (args->size() || args->args[0].resolve(state).type == datatypes::string) {
			
			sf::Music* music = new sf::Music;
			if (!music->openFromFile(args->args[0].s)) {
				return value("Cannot open audio stream!", datatypes::error);
			}
			return value(music, "audiostream");
		}
		return value("First argument must be a string!",datatypes::error);
	}
}