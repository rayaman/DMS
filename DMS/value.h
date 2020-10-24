#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>

namespace dms {
	struct dms_env;
	struct value;
	struct dms_args;
	extern const std::string datatype[];
	enum datatypes { escape, nil, number, boolean, env, string, custom, variable, block, error };
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
	struct dms_state;
	struct dms_string {
		size_t length = 0;
		uint8_t* val = nullptr;
		std::string getValue();
		std::string getValue(dms_state* state);
		friend std::ostream& operator << (std::ostream& out, const dms_string& c) {
			for (size_t i = 0; i < c.length; i++) {
				std::cout << c.val[i];
			}
			return out;
		};
	};
	// Custom data that you can work with by overriding this code
	struct dms_custom {
		void Init(dms_state* state);
		void _set(value* v);
		void _del();

		virtual value* Index(value* data);
		virtual bool NewIndex(value* var, value* val);
		virtual value* Call(dms_args* args);
		virtual value* ToString();
		virtual value* ADD(value* left, value* right);
		virtual value* SUB(value* left, value* right);
		virtual value* MUL(value* left, value* right);
		virtual value* DIV(value* left, value* right);
		virtual value* POW(value* left, value* right);
		virtual value* EQUAL(value* left, value* right);
		virtual value* LESS_THAN(value* left, value* right);
		virtual value* LESS_THAN_EQUAL(value* left, value* right);
	private:
		dms_state* state=nullptr;
		value* self;
	};
	dms_string* buildString(std::string str);
	dms_boolean* buildBool(bool b);
	dms_number* buildNumber(double num);
	struct value {
	public:
		datatypes type = nil;
		dms_boolean* b = nullptr;
		dms_number* n = nullptr;
		dms_string* s = nullptr;
		dms_env* e = nullptr;
		dms_custom* c = nullptr;
		value();
		value* resolve(dms_state*);
		value* copy();
		void nuke();
		void set(dms_string* str);
		void set(dms_boolean* bo);
		void set(dms_number* num);
		void set(dms_env* en);
		void set(dms_custom* cus);
		void set();
		bool typeMatch(const value* o) const;
		std::string getPrintable() const;
		std::string toString() const;
		friend std::ostream& operator << (std::ostream& out, const value& c) {
			if (c.type == string) {
				out << (char)c.type << c.s->getValue() << (char)0;
			}
			else if (c.type == number) {
				out << (char)c.type << c.n->getValue();
			}
			else if (c.type == nil) {
				out << (char)c.type << "nil";
			}
			else if (c.type == boolean) {
				if(c.b->getValue())
					out << (char)c.type << "true";
				else
					out << (char)c.type << "false";
			}
			else if (c.type == env) {
				out << (char)c.type << "Env: " << c;
			}
			else if (c.type == custom) {
				out << (char)c.type << "Custom Data: " << c;
			}
			else if (c.type == block) {
				out << (char)c.type << c.s->getValue();
			}
			else if (c.type == datatypes::variable) {
				out << (char)c.type << c.s->getValue(); // Do the lookup
			}
			return out;
		};
	};
	value* buildValue();
	value* buildValue(char const* s);
	value* buildNil();
	value* buildVariable(std::string str);
	value* buildVariable();
	value* buildValue(std::string str);
	value* buildValue(double dbl);
	value* buildValue(int i);
	value* buildValue(bool b);
	value* buildBlock(std::string str);

	struct dms_args {
		std::vector<value*> args;
		void push(value* val);
		size_t size();
		friend std::ostream& operator << (std::ostream& out, const dms_args& c) {
			for (size_t i=0; i < c.args.size(); i++) {
				if(i==c.args.size()-1)
					out << c.args[i];
				else
					out << c.args[i] << ", ";
			}
			return out;
		}
		std::string toString() {
			std::stringstream str;
			for (size_t i = 0; i < args.size(); i++) {
				str << *args[i];
			}
			return str.str();
		}
	};
	struct dms_env
	{
		std::unordered_map<std::string, value*> hpart;
		std::vector<value*> ipart;
		void pushValue(value* val);
		void pushValue(value* ind, value* val);
		value* getValue(value* val);
	private:
		size_t count = 0;
	};
}
