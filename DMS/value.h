#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

namespace dms {
	struct dms_env;
	enum datatypes { nil, number, boolean, env, string, custom };
	struct dms_number {
		double val;
		double getValue() { return val; }
		friend std::ostream& operator << (std::ostream& out, const dms_number& c) {
			out << c.val;
			return out;
		};
	};
	struct dms_boolean {
		bool val;
		bool getValue() { return val; }
		friend std::ostream& operator << (std::ostream& out, const dms_boolean& c) {
			out << c.val;
			return out;
		};
	};
	struct dms_string {
		size_t length = 0;
		uint8_t* val = nullptr;
		std::string getValue();
		friend std::ostream& operator << (std::ostream& out, const dms_string& c) {
			for (size_t i = 0; i < c.length; i++) {
				std::cout << c.val[i];
			}
			return out;
		};
	};
	struct dms_custom {

	};
	dms_string* buildString(std::string str);
	dms_boolean* buildBool(bool b);
	dms_number* buildNumber(double num);
	struct value {
		datatypes type = nil;
		dms_boolean* b = nullptr;
		dms_number* n = nullptr;
		dms_string* s = nullptr;
		dms_env* e = nullptr;
		dms_custom* c = nullptr;
		bool set(dms_string* str);
		bool set(dms_boolean* bo);
		bool set(dms_number* num);
		bool set(dms_env* en);
		bool typeMatch(const value o) const;
		std::string toString();
		friend bool operator<(const value& l, const value& r)
		{
			if (l.typeMatch(r)) {
				if (l.type == number) {
					return l.n->val > r.n->val;
				}
			}
			else {
				return false; // We might want to throw an error if this were to happen. Mixing incompatable types
			}
		}
		friend std::ostream& operator << (std::ostream& out, const value& c) {
			if (c.type == string) {
				out << c.s->getValue();
			}
			else if (c.type == number) {
				out << c.n->getValue();
			}
			else if (c.type == nil) {
				out << "nil";
			}
			else if (c.type == boolean) {
				out << c.b->getValue();
			}
			else if (c.type == env) {
				out << "Env: " << c;
			}
			else if (c.type == custom) {
				out << "Custom Data: " << c;
			}
			return out;
		};
	};
	struct dms_args {
		std::vector<value> args;
	};
	struct dms_env
	{
		std::map<std::string, value> hpart;
		std::map<double, value> ipart;
		void pushValue(value val);
		void pushValue(value str, value val);
		value getValue(value val);
	};
}
