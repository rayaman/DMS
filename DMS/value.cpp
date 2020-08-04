#include "value.h"
#include "utils.h"
#include "string"
#include <sstream>
#include <map>
#include <vector>
namespace dms {
	dms_string* buildString(std::string str) {
		size_t len = str.length();
		char16_t* arr = new char16_t[len];
		for (size_t i = 0; i < len; i++) {
			arr[i] = str.at(i);
		}
		dms_string* dms_str = new dms_string{ str.length(), arr };
		return dms_str;
	}
	dms_boolean* buildBool(bool b) {
		dms_boolean* dms_bool = new dms_boolean{b};
		return dms_bool;
	}
	dms_number* buildNumber(double num) {
		dms_number* dms_num = new dms_number{ num };
		return dms_num;
	}
	std::string value::toString() {
		std::stringstream temp;
		temp << this;
		return temp.str();
	}
	value* buildValue(std::string str) {
		value* val = new value{};
		val->set(buildString(str));
		return val;
	}
	value* buildValue(double dbl) {
		value* val = new value{};
		val->set(buildNumber(dbl));
		return val;
	}
	value* buildValue(int i) {
		value* val = new value{};
		val->set(buildNumber((double)i));
		return val;
	}
	value* buildValue(bool b) {
		value* val = new value{};
		val->set(buildBool(b));
		return val;
	}
	bool value::typeMatch(const value o) const {
		return type == o.type;
	}
	bool value::set(dms_string* str) {
		try {
			s = str;
			delete[] b;
			delete[] n;
			delete[] e;
			delete[] c;
			b = nullptr;
			n = nullptr;
			e = nullptr;
			c = nullptr;
			type = dms::string;
			return true;
		}
		catch (...) {
			return false;
		}
	}
	bool value::set(dms_boolean* bo) {
		try {
			b = bo;
			delete[] s;
			delete[] n;
			delete[] e;
			delete[] c;
			s = nullptr;
			n = nullptr;
			e = nullptr;
			c = nullptr;
			type = dms::boolean;
			return true;
		}
		catch (...) {
			return false;
		}
	}
	bool value::set(dms_number* num) {
		try {
			n = num;
			delete[] b;
			delete[] s;
			delete[] e;
			delete[] c;
			b = nullptr;
			s = nullptr;
			e = nullptr;
			c = nullptr;
			type = dms::number;
			return true;
		}
		catch (...) {
			return false;
		}
	}
	bool dms::value::set(dms_env* en) {
		try {
			e = en;
			delete[] b;
			delete[] s;
			delete[] n;
			delete[] c;
			b = nullptr;
			n = nullptr;
			s = nullptr;
			c = nullptr;
			type = dms::env;
			return true;
		}
		catch (...) {
			return false;
		}
	}
	std::string dms_string::getValue() {
		std::stringstream temp;
		for (size_t i = 0; i < length; i++) {
			temp << val[i];
		}
		return temp.str();
	}
	void dms_env::pushValue(value val) {
		double count = 0;
		while (ipart[count++].type != nil) {}
		ipart.insert_or_assign(count-1, val);
	}
	void dms_env::pushValue(value ind, value val) {
		if (ind.type == number) {
			size_t size = ipart.size();
			ipart.insert_or_assign(ind.n->val,val);
		}
		else {
			hpart.insert_or_assign(ind.toString(), val);
		}
	}
	value dms_env::getValue(value ind) {
		if (ind.type == number) {
			return ipart.at(ind.n->getValue());
		}
		else if (ind.type == number) {
			return *(new value{}); // Return a nil value
		}
		else {
			return hpart.at(ind.toString());
		}
	}
}