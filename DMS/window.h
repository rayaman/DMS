#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "dms_state.h"
#include "utils.h"
#include <stdio.h>
#include "Connection.h"
namespace dms::window {
	multi::connection<sf::RenderWindow*> OnWindowCreated;
	void init(dms_state*);
	bool windowIsReady = false;
	std::vector<sf::RenderWindow*> windows;
	value newWindow(void*, dms_state*, dms_args*);
}