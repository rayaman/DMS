#pragma once
namespace dms::errors {
	enum errortype {
		unknown,
		string_out_of_bounds,
		invalid_arguments,
		invalie_type,
		array_out_of_bounds,
	};
	struct error {
		errortype code=unknown;
		bool crash = false; // Set if you would like the state to exit
	};
}