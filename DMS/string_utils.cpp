#include "string_utils.h"

uint8_t* copyStr(dms::dms_string str, size_t start, size_t size) {
	uint8_t* newptr = new uint8_t[size];
	std::copy(str.val + start, str.val + start + size, newptr);
	return newptr;
}
uint8_t* copyStr(dms::dms_string str, dms::dms_number start, dms::dms_number size) {
	uint8_t* newptr = new uint8_t[size.getValue()];
	std::copy(str.val + (size_t)start.getValue(), str.val + (size_t)start.getValue() + (size_t)size.getValue(), newptr);
	return newptr;
}
namespace dms::string_utils {
	// string
	dms_string* reverse(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil."});
			return buildString("");
		}
		if (utils::typeassert(state, args, string)) {
			size_t size = args.args[0].s->length;
			uint8_t* newptr = copyStr(*args.args[0].s, 0, size);
			std::reverse(newptr, newptr + size);
			dms_string* newstr = new dms_string{ size, newptr };
			return newstr;
		}
		return buildString("");
	}
	//string number number
	dms_string* sub(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil."});
			return buildString("");
		}
		if (utils::typeassert(state, args, string, number, number)) {
			size_t start = args.args[1].n->val;
			size_t size = args.args[2].n->val;
			dms_string str = *args.args[0].s;
			if (start + size > str.length) {
				state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
				return buildString("");
			}
			else
			{
				uint8_t* newptr = copyStr(str, start, size);
				dms_string* newstr = new dms_string{ size,newptr };
				return newstr;
			}
		}
		return buildString("");
	}
	//string
	dms_string* upper(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildString("");
		}
		if (utils::typeassert(state, args, string)) {
			dms_string str = *args.args[0].s;
			uint8_t* newptr = copyStr(str, 0, str.length);
			std::transform(newptr, newptr + str.length, newptr, toupper);
			dms_string* newstr = new dms_string{ str.length, newptr };
			return newstr;
		}
		return buildString("");
	}
	//string
	dms_string* lower(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildString("");
		}
		if (utils::typeassert(state, args, string)) {
			dms_string str = *args.args[0].s;
			uint8_t* newptr = copyStr(str, 0, str.length);
			std::transform(newptr, newptr + str.length, newptr, tolower);
			dms_string* newstr = new dms_string{ str.length, newptr };
			return newstr;
		}
		return buildString("");
	}
	//string string
	dms_boolean* startsWith(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildBool(false);
		}
		if (utils::typeassert(state, args, string, string)) {
			dms_string str = *args.args[0].s;
			dms_string starts = *args.args[1].s;
			if (starts.length > str.length)
				return buildBool(false);
			for (int i = 0; i < starts.length; i++) {
				if (str.val[i] != starts.val[i]) {
					return buildBool(false);
				}
			}
			return buildBool(true);
		}
		return buildBool(false);
	}
	//string string
	dms_boolean* endsWith(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildBool(false);
		}
		if (utils::typeassert(state, args, string, string)) {
			dms_string str = *args.args[0].s;
			dms_string ends = *args.args[1].s;
			if (ends.length > str.length)
				return buildBool(false);
			for (int i = ends.length - 1; i >= 0; i--) {
				//std::cout << str.val[i +(str.length - ends.length)] << " | " << ends.val[i] << " : " << i << std::endl;
				if (str.val[i + (str.length - ends.length)] != ends.val[i]) {
					return buildBool(false);
				}
			}
			return buildBool(true);
		}
		return buildBool(false);
	}
	//string string 
	dms_number* indexOf(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildNumber(-1);
		}
		if (utils::typeassert(state, args, string, string)) {
			bool check = false;
			int ind;
			dms_string str = *args.args[0].s;
			dms_string find = *args.args[1].s;
			for (int i = 0; i < str.length; i++) {
				for (int ii = 0; ii < find.length; ii++) {
					if (find.val[ii] == str.val[i + ii]) {
						check = true;
						ind = i;
					}
					else {
						check = false;
						break;
					}
				}
				if (check) {
					dms_number* newnum = new dms_number{ (double)ind };
					return newnum;
				}
			}
			if (check) {
				dms_number* newnum = new dms_number{ (double)ind };
				return newnum;
			}
			else {
				dms_number* newnum = new dms_number{ -1 };
				return newnum;
			}
		}
		return buildNumber(-1);
	}
	//string string
	dms_boolean* includes(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildBool(false);
		}
		if (utils::typeassert(state, args, string, string)) {
			return buildBool(indexOf(state, args)->val != -1);
		}
		return buildBool(false);
	}
	//string number
	dms_string* repeat(dms_state* state, dms_args args) {
		if (args.args.size() == 0) {
			state->push_error(errors::error{ errors::invalid_arguments,"Expected a string object, got nil." });
			return buildString("");
		}
		if (utils::typeassert(state, args, string, number)) {
			dms_string str = *args.args[0].s;
			dms_number num = *args.args[1].n;
			std::string newstr = str.getValue();
			std::stringstream temp;
			for (int i = 0; i < num.getValue(); i++)
				temp << newstr;
			return buildString(temp.str());
		}
		return buildString("");
	}
}