#include "dmsstring.h"
#include "value.h"
#include <algorithm>
#include <iterator>
#include "dms_exceptions.h"
#include <iostream>
#include <sstream>

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
	dms_string* reverse(dms_string str) {
		uint8_t* newptr = copyStr(str, 0, str.length);
		std::reverse(newptr, newptr + str.length);
		dms_string* newstr = new dms_string{ str.length,newptr };
		return newstr;
	}
	dms_string* sub(dms_string str, size_t start, size_t size) {
		if (start + size > str.length) {
			throw exceptions::StringBoundsExeception();
		}
		else
		{
			uint8_t* newptr = copyStr(str, start, size);
			dms_string* newstr = new dms_string{ size,newptr };
			return newstr;
		}
	}
	dms_string* upper(dms_string str) {
		uint8_t* newptr = copyStr(str, 0, str.length);
		std::transform(newptr, newptr + str.length, newptr, toupper);
		dms_string* newstr = new dms_string{ str.length, newptr };
		return newstr;
	}
	dms_string* lower(dms_string str) {
		uint8_t* newptr = copyStr(str, 0, str.length);
		std::transform(newptr, newptr + str.length, newptr, tolower);
		dms_string* newstr = new dms_string{ str.length, newptr };
		return newstr;
	}
	dms_boolean* startsWith(dms_string str, dms_string starts) {
		if (starts.length > str.length)
			return buildBool(false);
		for (int i = 0; i < starts.length; i++) {
			if (str.val[i] != starts.val[i]) {
				return buildBool(false);
			}
		}
		return buildBool(true);
	}
	dms_boolean* endsWith(dms_string str, dms_string ends) {
		if (ends.length > str.length)
			return buildBool(false);
		for (int i = ends.length-1; i >= 0; i--) {
			//std::cout << str.val[i +(str.length - ends.length)] << " | " << ends.val[i] << " : " << i << std::endl;
			if (str.val[i + (str.length - ends.length)] != ends.val[i]) {
				return buildBool(false);
			}
		}
		return buildBool(true);
	}
	dms_number* indexOf(dms::dms_string str, dms::dms_string find) {
		bool check=false;
		int ind;
		for (int i = 0; i < str.length; i++) {
			for (int ii = 0; ii < find.length; ii++) {
				if (find.val[ii] == str.val[i+ii]) {
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
	dms_boolean* includes(dms_string str, dms_string inc) {
		return buildBool(indexOf(str, inc)->val != -1);
	}
	dms_string* repeat(dms_string str, dms_number num) {
		std::string newstr = str.getValue();
		std::stringstream temp;
		for (int i = 0; i < num.getValue(); i++)
			temp << newstr;
		return buildString(temp.str());
	}
}