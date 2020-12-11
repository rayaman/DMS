#pragma once
#include "enviroment.h"
// This file converts the c++ objects into a form that c is happy with
extern "C" {
	using namespace dms;
	// All structs types in this file will have an s_typename infront of it
	// All methods will have an sf_methodname infron of it as well
	struct s_value {
		datatypes type = nil; // The type of the value
		const char* s; // Contains the string part.
		uint64_t i; // The int portion of the code
		double n;
	};
	void s_value_setInt(s_value& sv, int n) { sv.type = int64; sv.i = n; } // Define value as an int
	void s_value_setDouble(s_value& sv, double d) { sv.type = number; sv.n = d; } // Define value as a double
	void s_value_setString(s_value& sv, const char* str) { sv.type = string; sv.s = str; } // Define value as a string
	void s_value_setBlock(s_value& sv, const char* str) { sv.type = block; sv.s = str; }  // Define value as a block
	void s_value_setVariable(s_value& sv, const char* str) { sv.type = variable; sv.s = str; }  // Define value as a variable
	void s_value_setNil(s_value& sv) { sv.type = nil; } // Define value as nil
	struct s_enviroment {

	};
}