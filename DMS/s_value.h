#pragma once
extern "C" {
	// All structs types in this file will have an s_typename infront of it
	// All methods will have an sf_methodname infron of it as well
	const char sv_escape	= 0x00;
	const char sv_nil		= 0x01;
	const char sv_number	= 0x02;
	const char sv_int		= 0x03;
	const char sv_boolean	= 0x04;
	const char sv_env		= 0x05;
	const char sv_string	= 0x06;
	const char sv_custom	= 0x07;
	const char sv_variable	= 0x08;
	const char sv_block		= 0x09;
	const char sv_error		= 0x10;

	struct s_value {
		char type = sv_nil; // The type of the value
		const char* s; // Contains the string part.
		long long int i; // The int portion of the code
		long double n;
	};

	void sm_value_setEscape(s_value& sv) { sv.type = sv_escape; } // An escape type, used at the end of an array of values.
	void sm_value_setInt(s_value& sv, int n) { sv.type = sv_int; sv.i = n; } // Define value as an int
	void sm_value_setDouble(s_value& sv, long double d) { sv.type = sv_number; sv.n = d; } // Define value as a double
	void sm_value_setString(s_value& sv, const char* str) { sv.type = sv_string; sv.s = str; } // Define value as a string
	void sm_value_setBlock(s_value& sv, const char* str) { sv.type = sv_block; sv.s = str; }  // Define value as a block
	void sm_value_setVariable(s_value& sv, const char* str) { sv.type = sv_variable; sv.s = str; }  // Define value as a variable
	void sm_value_setNil(s_value& sv) { sv.type = sv_nil; } // Define value as nil
}