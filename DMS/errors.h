#pragma once
#include <string>
namespace dms::errors {
	enum errortype {
		unknown,
		string_out_of_bounds,
		invalid_arguments,
		invalie_type,
		array_out_of_bounds,
		badtoken,
		block_already_defined,
		choice_unknown
	};
	struct error {
		errortype code=unknown;
		std::string err_msg;
		bool crash = true; // Set if you would like the state to exit
		size_t linenum = 0;
	};
}