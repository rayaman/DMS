#pragma once
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "value.h"
#include "dms_state.h"
#include "dms_exceptions.h"
#include "utils.h"
#include "errors.h"
namespace dms::string_utils {
	dms_string* reverse(dms_state* state, dms_args args);
	dms_string* sub(dms_state* state, dms_args args);
	dms_string* upper(dms_state* state, dms_args args);
	dms_string* lower(dms_state* state, dms_args args);
	dms_boolean* startsWith(dms_state* state, dms_args args);
	dms_boolean* endsWith(dms_state* state, dms_args args);
	dms_number* indexOf(dms_state* state, dms_args args);
	dms_boolean* includes(dms_state* state, dms_args args);
	dms_string* repeat(dms_state* state, dms_args args);
}