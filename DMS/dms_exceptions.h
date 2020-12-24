#pragma once
#include "pch.h"
namespace dms {
	namespace exceptions {
		struct StringBoundsException : public std::exception {
			const char* what() const throw () {
				return "Attempt to sub outside of the bounds of the string!";
			}
		};
		struct BadChoiceHandlerException : public std::exception {
			const char* what() const throw () {
				return "ChoiceHandler Expected got ???";
			}
		};
		struct InvalidChoice : public std::exception {
			const char* what() const throw () {
				return "Invalid Choice!";
			}
		};
	}
}