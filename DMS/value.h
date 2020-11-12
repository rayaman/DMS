#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>

namespace dms {
	struct dms_list;
	struct value;
	struct dms_args;
	struct dms_state;
	extern const std::string datatype[];
	enum datatypes { escape, nil, number, boolean, env, string, custom, variable, block, error };
	struct dms_number {
		double val;
		double getValue() { return val; }
		friend std::ostream& operator << (std::ostream& out, const dms_number& c);
	};
	struct dms_boolean {
		bool val;
		bool getValue() { return val; }
		friend std::ostream& operator << (std::ostream& out, const dms_boolean& c);
	};
	
	struct dms_string {
		std::string val;
		std::string getValue();
		std::string getValue(dms_state* state);
		friend std::ostream& operator << (std::ostream& out, const dms_string& c);
	};
	// Custom data that you can work with by overriding this code
	struct dms_custom {
		void Init(dms_state* state);
		void _set(value* v);
		void _del();
		virtual value Index(value data);
		virtual bool NewIndex(value var, value val);
		virtual value Call(dms_args* args);
		virtual value ToString();
		virtual value ADD(value left, value right);
		virtual value SUB(value left, value right);
		virtual value MUL(value left, value right);
		virtual value DIV(value left, value right);
		virtual value POW(value left, value right);
		virtual value EQUAL(value left, value right);
		virtual value LESS_THAN(value left, value right);
		virtual value LESS_THAN_EQUAL(value left, value right);
	private:
		dms_state* state=nullptr;
		value* self;
	};
	dms_string* buildString(std::string str);
	dms_boolean* buildBool(bool b);
	struct value {
	public:
		datatypes type = datatypes::nil;
		bool b=false;
		double n=0;
		dms_string* s = nullptr;
		dms_list* e = nullptr;
		dms_custom* c = nullptr;
		value();
		value(datatypes);
		value(char const*,datatypes);
		value(std::string, datatypes);
		value(std::string);
		value(char const*);
		value(double);
		value(int);
		value(bool);
		~value();
		value(const value& other);
		bool isNil();
		value& operator=(value& other);
		value& operator=(const value& other);
		friend bool operator==(const value& lhs, const value& rhs);
		friend value operator+(const value& lhs, const value& rhs);
		friend value operator-(const value& lhs, const value& rhs);
		friend value operator/(const value& lhs, const value& rhs);
		friend value operator*(const value& lhs, const value& rhs);
		friend bool operator!=(const value& lhs, const value& rhs);
		value resolve(dms_state*);
		void nuke();
		void set(value*);
		void set(dms_string* str);
		void set(bool bo);
		void set(double num);
		void set(dms_list* en);
		void set(dms_custom* cus);
		void set();
		bool typeMatch(const value* o) const;
		std::string getPrintable() const;
		std::string toString() const;
		friend std::ostream& operator << (std::ostream& out, const value& c);
	};

	struct dms_args {
		std::vector<value> args;
		void push(value val);
		size_t size();
		friend std::ostream& operator << (std::ostream& out, const dms_args& c);
		std::string toString();
	};
}
