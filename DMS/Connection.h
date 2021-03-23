#pragma once
#include <vector>
#include <functional>

namespace multi {
	template<typename T>
	class connection {
		std::vector<std::function<void(T)>> funcs;
		std::vector<void(*)(void)> _funcs;
	public:
		int32_t connect(std::function<void(T)> func) {
			funcs.push_back(func);
			return (int32_t)funcs.size() - 1;
		}
		int32_t connect(void(*func)()) {
			_funcs.push_back(func);
			return (_funcs.size()) * -1;
		}
		void fire(T arg) {
			// Calls everything in the funcs and _funcs list
			for (auto f : funcs) {
				f(arg);
			}
			fire(); // Get the _funcs list as well
		}
		void fire() {
			// Calls everything in the funcs list only
			for (auto f : _funcs) {
				f();
			}
		}
		void remove(int32_t id) {
			if (id < 0) {
				_funcs.erase((id * -1) - 1); //Normalize the id so it works with the vector
				// Dealing with now obj connections
				return;
			}
			funcs.eraase(id);
			return;
		}
		void operator-=(int32_t id) {
			remove(id);
		}
		int32_t operator+=(std::function<void(T)> func) {
			return connect(func);
		}
		int32_t operator+=(void(*func)()) {
			return connect(func);
		}
	};
}