#pragma once
#include <functional>
#include <list>
namespace multi {
	/*
		
	*/
	template<class T>
	class connection {
		// bool = func(void* v)
		std::vector<void(*)(T)> funcs;
		std::vector<void(*)(void)> _funcs;
	public:
		int32_t connect(void(*func)(T)) {
			funcs.push_back(func);
			return funcs.size() - 1;
		}
		int32_t connect(void(*func)()) {
			_funcs.push_back(func);
			return (_funcs.size())*-1;
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
		int32_t operator+=(void(*func)(T)) {
			return connect(func);
		}
		int32_t operator+=(void(*func)()) {
			return connect(func);
		}
	};
}