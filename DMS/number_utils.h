#pragma once
#include "value.h"
#include "utils.h"
#include "dms_state.h"
#include <limits>
namespace dms::number_utils {
	dms_number* huge(dms_state* state);
	dms_number* tiny(dms_state* state);
	dms_number* abs(dms_state* state, dms_args args);
	dms_number* max(dms_state* state, dms_args args);
	dms_number* pow(dms_state* state, dms_args args);
}


