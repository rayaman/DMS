#pragma once
#include <vector>
#include <functional>
//#include <mutex>
// Todo make this thread safe (I'm trying)

namespace multi {
	template<typename T>
	class connection {
		std::vector<std::function<void(T)>> funcs;
		std::vector<std::function<void()>> _funcs;
		std::string name;
		//std::mutex mtx;
	public:
		connection() { name = std::string("Connection_") + typeid(T).name(); }
		connection(std::string name) {
			this->name = name;
		}
		int32_t connect(std::function<void(T)> func) {
			funcs.push_back(func);
			return (int32_t)funcs.size() - 1;
		}
		int32_t connect(std::function<void()> func) {
			_funcs.push_back(func);
			return (_funcs.size()) * -1;
		}
		void fire(T arg) {
			if (funcs.size() == 0) return;
			// Calls everything in the funcs and _funcs list
			//std::lock_guard<std::mutex> lck(mtx);
			for (size_t i = 0; i < funcs.size(); i++) {
				funcs[i](arg);
			}
			fire(); // Get the _funcs list as well
		}
		void fire(bool lock = true) {
			if (_funcs.size() == 0) return;
			if (lock) {
				//std::lock_guard<std::mutex> lck(mtx);
				for (size_t i = 0; i < _funcs.size(); i++) {
					_funcs[i]();
				}
				return;
			}
			//std::lock_guard<std::mutex> lck(mtx);
			for (size_t i = 0; i < _funcs.size(); i++) {
				_funcs[i]();
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
		int32_t operator+=(std::function<void()> func) {
			return connect(func);
		}
	};
}