#pragma once
#include "value.h"
// This file converts the c++ objects into a form that c is happy with
extern "C" {
	using namespace dms;
	// All structs types in this file will have an s_typename infront of it
	// All methods will have an sf_methodname infron of it as well
	struct s_value {
		datatypes type = nil; // The type of the value
		unsigned int slen; // Length of string
		const char* s; // Contains the string part. 
		uint64_t i; // The int portion of the code
		double n;
		void setInt(int n); // Define value as an int
		void setDouble(double n); // Define value as a double
		void setString(const char* s); // Define value as a string
		void setBlock(const char* s); // Define value as a block
		void setVariable(const char* s); // Define value as a variable
		void setNil(); // Define value as nil
	};
}