#include "value.h"
#include "dms_state.h"
#include "utils.h"
namespace dms {
	const std::string datatype[] = { "escape", "nil", "number", "int", "boolean", "env", "string", "custom", "variable", "block" , "error"};
	value::value() {
		// Nothing to do here!
	}
	value::value(char const* str, datatypes t) {
		type = t;
		s = str;
	}
	value::value(char const* str) {
		type = datatypes::string;
		s = str;
	}
	value::value(size_t val) {
		type = datatypes::int64;
		i = val;
	}
	value::value(std::string str) {
		type = datatypes::string;
		s = str;
	}
	value::value(int64_t n) {
		type = datatypes::int64;
		i = n;
	}
	value::value(std::string str,datatypes t) {
		type = t;
		s = str;
	}
	value::value(double d) {
		type = datatypes::number;
		n = d;
	}
	value::value(int d) {
		type = datatypes::int64;
		i = d;
	}
	value::value(bool bo) {
		type = datatypes::boolean;
		b = bo;
	}
	value::~value() {
		nuke();
	}
	size_t count = 0;
	value::value(datatypes t) : value() {
		if (t == datatypes::variable) {
			set(utils::concat("$",++count));
		}
		type = t;
	}
	value::value(const value& other) {
		if (this != &other) {
			type = other.type;
			state = other.state;
			switch (other.type) {
			case datatypes::block:
				s = other.s;
				break;
			case datatypes::boolean:
				b = other.b;
				break;
			case datatypes::custom:
				// Handle this later
				break;
			case datatypes::env:
				// Handle this later
				break;
			case datatypes::escape:
				s = other.s;
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::int64:
				i = other.i;
				break;
			case datatypes::string:
				s = other.s;
				break;
			case datatypes::variable:
				s = other.s;
				break;
			case datatypes::error:
				s = other.s;
			default:
				break;
			}
		}
	}
	value& value::operator=(value& other) {
		if (this != &other) {
			nuke(); // Delete it all
			type = other.type;
			state = other.state;
			switch (other.type) {
			case datatypes::block:
				s = other.s;
				break;
			case datatypes::boolean:
				b = other.b;
				break;
			case datatypes::custom:
				// Handle this later
				break;
			case datatypes::env:
				// Handle this later
				break;
			case datatypes::escape:
				s = other.s;
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::int64:
				i = other.i;
				break;
			case datatypes::string:
				s = other.s;
				break;
			case datatypes::variable:
				s = other.s;
				break;
			case datatypes::error:
				s = other.s;
			default:
				break;
			}
			//other.nuke();
		}
		// by convention, always return *this
		return *this;
	}
	bool value::isNil() const {
		return type == datatypes::nil;
	}
	value& value::operator=(const value& other) {
		if (this != &other) {
			nuke();
			type = other.type;
			state = other.state;
			switch (other.type) {
			case datatypes::block:
				s = other.s;
				break;
			case datatypes::boolean:
				b = other.b;
				break;
			case datatypes::custom:
				// Handle this later
				break;
			case datatypes::env:
				// Handle this later
				break;
			case datatypes::escape:
				s = other.s;
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::int64:
				i = other.i;
				break;
			case datatypes::string:
				s = other.s;
				break;
			case datatypes::variable:
				s = other.s;
				break;
			case datatypes::error:
				s = other.s;
			default:
				break;
			} 
		}
		// by convention, always return *this
		return *this;
	}
	bool value::isNum() const
	{
		return (type == datatypes::number || type == datatypes::int64);
	}
	bool operator==(const value& lhs, const value& rhs) {
		return lhs.getPrintable() == rhs.getPrintable();
	}
	value operator+(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::nil || rhs.type == datatypes::nil) {
			return value("Attempt to concat a nil value!",datatypes::error);
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n + rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return value(lhs.i + rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return value(lhs.i + rhs.i);
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return value(lhs.n + rhs.i);
		}
		else if (lhs.type == datatypes::boolean && rhs.type == datatypes::boolean) {
			return value((bool)(lhs.b + rhs.b));
		}
		else if ((lhs.type == datatypes::string || rhs.type == datatypes::string)) {
			return lhs.getPrintable() + rhs.getPrintable();
		}
		else {
			return value("Invalid use of '+'!", datatypes::error);
		}
	}
	value operator-(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n - rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return value(lhs.i - rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return value(lhs.i - rhs.i);
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return value(lhs.n - rhs.i);
		}
		else {
			if(lhs.type!=datatypes::number)
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[lhs.type] ," value!"),datatypes::error);
			else
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[rhs.type], " value!"), datatypes::error);
		}
	}
	value operator/(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n / rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return value(lhs.i / rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return value(lhs.i / rhs.i);
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return value(lhs.n / rhs.i);
		}
		else {
			if (lhs.type != datatypes::number)
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[lhs.type], " value!"), datatypes::error);
			else
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[rhs.type], " value!"), datatypes::error);
		}
	}
	value operator*(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n * rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return value(lhs.i * rhs.n);
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return value(lhs.i * rhs.i);
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return value(lhs.n * rhs.i);
		}
		else if (lhs.type == datatypes::boolean && rhs.type == datatypes::boolean) {
			return value((bool)(lhs.b * rhs.b));
		}
		else {
			if (lhs.type != datatypes::number)
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[lhs.type], " value!"), datatypes::error);
			else
				return value(utils::concat("Attempted to perform arithmetic on a ", datatype[rhs.type], " value!"), datatypes::error);
		}
	}
	bool operator!=(const value& lhs, const value& rhs) {
		return !(lhs.getPrintable() == rhs.getPrintable());
	}
	bool operator>(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n > rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return lhs.i > rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return lhs.i > rhs.i;
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return lhs.n > rhs.i;
		}
		return false;
	}
	bool operator<(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n < rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return lhs.i < rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return lhs.i < rhs.i;
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return lhs.n < rhs.i;
		}
		return false;
	}
	bool operator>=(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n >= rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return lhs.i >= rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return lhs.i >= rhs.i;
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return lhs.n >= rhs.i;
		}
		return false;
	}
	bool operator<=(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n <= rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::number) {
			return lhs.i <= rhs.n;
		}
		else if (lhs.type == datatypes::int64 && rhs.type == datatypes::int64) {
			return lhs.i <= rhs.i;
		}
		else if (lhs.type == datatypes::number && rhs.type == datatypes::int64) {
			return lhs.n <= rhs.i;
		}
		return false;
	}
	value value::resolve(dms_state* _state) {
		if (type == datatypes::variable && (*this)!=(*_state->getMem())[s]) {
			return (*_state->getMem())[s].resolve(_state);
		}
		return *this;
	}
	value value::resolve() {
		return resolve(state);
	}
	void dms_args::push(value val) {
		args.push_back(val);
	}
	size_t dms_args::size() {
		return args.size();
	}
	std::string value::getPrintable() const {
		if (type == string) {
			return s;
		}
		else if (type == number) {
			std::string temp = std::to_string(n);
			while (temp.find(".") != std::string::npos   // !=string::npos is important!!!
				&& temp.substr(temp.length() - 1, 1) == "0"
				|| temp.substr(temp.length() - 1, 1) == ".")
				{
					temp.pop_back();
				}
			return temp;
		}
		else if (type == int64) {
			return std::to_string(i);
		}
		else if (type == nil) {
			return "nil";
		}
		else if (type == boolean) {
			if (b)
				return "true";
			else
				return "false";
		}
		else if (type == env) {
			return "env";
		}
		else if (type == custom) {
			return "custom";
		}
		else if (type == block) {
			if (state->characterExists(s)) {
				auto cha = state->getCharacter(s);
				return cha->getName();
			}
			return s;
		}
		else if (type == datatypes::variable) {
			return s; // Do the lookup
		}
		else if (type == datatypes::error) {
			return std::string("ERROR: ") + s;
		}
		else if (type == datatypes::escape) {
			return "";
		}
		return "unknown";
	}
	std::string value::toString() const {
		return getPrintable();
	}
	// Compile time
	void value::nuke() {
		delete e;
		delete c;
		e = nullptr;
		c = nullptr;
	}
	std::ostream& operator << (std::ostream& out, const value& c) {
		if (c.type == string) {
			out << (char)c.type << c.s << (char)0;
		}
		else if (c.type == number) {
			out << (char)c.type << c.n;
		}
		else if (c.type == int64) {
			out << (char)c.type << c.i;
		}
		else if (c.type == nil) {
			out << (char)c.type << "nil";
		}
		else if (c.type == boolean) {
			if(c.b)
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
			out << (char)c.type << c.s;
		}
		else if (c.type == datatypes::variable) {
			out << (char)c.type << c.s; // Do the lookup
		}
		else if (c.type == datatypes::escape) {
			out << (char)0;
		}
		return out;
	};
	// Fixed issue with memory not properly being cleaned up
	bool value::typeMatch(const value* o) const {
		return type == o->type;
	}
	void value::set(value val) {
		type = val.type;
		s = val.s;
		n = val.n;
		i = val.i;
		b = val.b;
		e = val.e;
		c = val.c;
	}
	void value::set(std::string str) {
		nuke();
		s = str;
		type = string;
	}
	void value::set(bool bo) {
		nuke();
		b = bo;
		type = boolean;
	}
	void value::set(double num) {
		nuke();
		n = num;
		type = number;
	}
	void dms::value::set(dms_list* en) {
		nuke();
		e = en;
		type = env;
	}
	void dms::value::set(dms_custom* cus) {
		nuke();
		c = cus;
		c->_set(this);
		type = custom;
	}
	void value::set() {
		nuke();
		type = nil;
	}
	void dms_list::pushValue(value val) {
		ipart.push_back(val);
	}
	value dms_list::getValue(value ind) {
		if (ind.type == number) {
			return ipart.at((int)ind.n);
		}
		else {
			return new value();
		}
	}
	std::ostream& operator << (std::ostream& out, const dms_args& c) {
		for (size_t i = 0; i < c.args.size(); i++) {
			if (i == c.args.size() - 1)
				out << c.args[i];
			else
				out << c.args[i] << ", ";
		}
		return out;
	}
	std::string dms_args::toString() {
		std::stringstream str;
		for (size_t i = 0; i < args.size(); i++) {
			str << args[i];
		}
		return str.str();
	}
}