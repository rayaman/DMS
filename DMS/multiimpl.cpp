#include "pch.h"
#include "actors.h"
namespace multi {
	bool runner::update() {
		if (!active)
			return false;
		size_t id = 0;
		for (auto mobj : mObjects) {
			mobj->act(id++);
		}
		return active;
	}
	void runner::mainloop() {
		while (update());
	}
}