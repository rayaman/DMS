#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

namespace dms {
	struct dms_env;
	enum datatypes { nil, number, boolean, env, string, custom, variable };
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
		/*~value() {
			nuke();
		}*/
		datatypes type = nil;
		dms_boolean* b = nullptr;
		dms_number* n = nullptr;
		dms_string* s = nullptr;
		dms_env* e = nullptr;
		dms_custom* c = nullptr;
		void nuke();
		void set(dms_string* str);
		void set(dms_boolean* bo);
		void set(dms_number* num);
		void set(dms_env* en);
		void set();
		bool typeMatch(const value* o) const;
		std::string toString() const;
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
			else if (c.type == datatypes::variable) {
				out << c.s->getValue(); // Do the lookup
			}
			return out;
		};
	};
	value* buildValue(std::string str);
	value* buildValue(double dbl);
	value* buildValue(int i);
	value* buildValue(bool b);

	struct dms_args {
		std::vector<value*> args;
		void push(value* val);
		friend std::ostream& operator << (std::ostream& out, const dms_args& c) {
			for (size_t i=0; i < c.args.size(); i++) {
				if(i==c.args.size()-1)
					out << c.args[i];
				else
					out << c.args[i] << ", ";
			}
			return out;
		}
	};
	struct dms_env
	{
		std::map<std::string, value*> hpart;
		std::map<double, value*> ipart;
		void pushValue(value* val);
		void pushValue(value* ind, value* val);
		value* getValue(value* val);
	private:
		size_t count = 0;
	};
}
