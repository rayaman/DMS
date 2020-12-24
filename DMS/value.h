#pragma once
#include "pch.h"

namespace dms {
	struct dms_list;
	struct value;
	struct dms_args;
	struct dms_state;
	extern const std::string datatype[];
	enum datatypes { escape, nil, number, int64, boolean, env, string, custom, variable, block, error };
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
	struct value {
	public:
		datatypes type = datatypes::nil;
		bool b=false;
		double n=0;
		int64_t i=0;
		std::string s;
		dms_list* e = nullptr;
		void* c = nullptr; // A custom link to void data. Must be void* for compatiablity with c-API
		std::string ctype; // The type of the custom data
		dms_state* state = nullptr;
		value();
		value(void* cdata,std::string t);
		value(datatypes);
		value(char const*,datatypes);
		value(std::string, datatypes);
		value(std::string);
		value(char const*);
		value(double);
		value(int);
		value(int64_t);
		value(size_t);
		value(bool);
		~value();
		value(const value& other);
		bool isNil() const;
		bool isNum() const;
		value& operator=(value& other);
		value& operator=(const value& other);
		friend bool operator==(const value& lhs, const value& rhs);
		friend bool operator!=(const value& lhs, const value& rhs);
		friend bool operator>(const value& lhs, const value& rhs);
		friend bool operator<(const value& lhs, const value& rhs);
		friend bool operator>=(const value& lhs, const value& rhs);
		friend bool operator<=(const value& lhs, const value& rhs);

		friend value operator+(const value& lhs, const value& rhs);
		friend value operator-(const value& lhs, const value& rhs);
		friend value operator/(const value& lhs, const value& rhs);
		friend value operator*(const value& lhs, const value& rhs);
		value resolve(dms_state*);
		value resolve();
		void nuke();
		void set(value);
		void set(std::string str);
		void set(bool bo);
		void set(double num);
		void set(dms_list* en);
		void set(void* cus);
		void set();
		bool typeMatch(const value* o) const;
		std::string getPrintable() const;
		std::string toString() const;
		double getDouble() const;
		int64_t getInt() const;
		friend std::ostream& operator << (std::ostream& out, const value& c);
	};

	struct dms_args {
		dms_args(int n);
		dms_args();
		std::vector<value> args;
		void push(value val);
		size_t size();
		friend std::ostream& operator << (std::ostream& out, const dms_args& c);
		std::string toString();
	};
}
