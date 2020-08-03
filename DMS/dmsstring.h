#pragma once
#include "value.h"
namespace dms::string_utils {
	dms_string* reverse(dms_string str);
	dms_string* sub(dms_string str, size_t start, size_t size);
	dms_string* upper(dms_string str);
	dms_string* lower(dms_string str);
	dms_boolean* startsWith(dms_string str,dms_string starts);
	dms_boolean* endsWith(dms_string str, dms_string ends);
	dms_number* indexOf(dms_string str, dms_string find);
	dms_boolean* includes(dms_string str, dms_string inc);
	dms_string* repeat(dms_string str, dms_number num);
}