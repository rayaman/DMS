#pragma once
#include "pch.h"
#include "chunk.h"
namespace dms {
	namespace errors {
		enum errortype {
			unknown,
			string_out_of_bounds,
			invalid_arguments,
			invalid_type,
			array_out_of_bounds,
			badtoken,
			block_already_defined,
			choice_unknown,
			nested_function,
			disp_unknown,
			non_existing_block,
			incompatible_version,
			non_existing_function
		};
		struct error {
			errortype code = unknown;
			std::string err_msg;
			bool crash = true; // Set if you would like the state to exit
			size_t linenum = 0;
			chunk* current_chunk = nullptr;
		};
	}
}