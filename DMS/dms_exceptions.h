#pragma once
#include <exception>
namespace dms::exceptions {
	struct StringBoundsExeception : public std::exception {
		const char* what() const throw () {
			return "Attempt to sub outside of the bounds of the string!";
		}
	};
}

