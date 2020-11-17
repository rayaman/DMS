#include "value.h"
#include "dms_state.h"
#include "utils.h"
namespace dms {
	const std::string datatype[] = { "escape", "nil", "number", "boolean", "env", "string", "custom", "variable", "block" };
	value::value() {
		// Nothing to do here yet!
	}
	value::value(char const* str, datatypes t) {
		type = t;
		s = buildString(str);
	}
	value::value(char const* str) {
		type = datatypes::string;
		s = buildString(str);
	}
	value::value(std::string str) {
		type = datatypes::string;
		s = buildString(str);
	}
	value::value(std::string str,datatypes t) {
		type = t;
		s = buildString(str);
	}
	value::value(double d) {
		type = datatypes::number;
		n = d;
	}
	value::value(int d) {
		type = datatypes::number;
		n = d;
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
			set(buildString(utils::concat("$",++count)));
		}
		type = t;
	}
	value::value(const value& other) {
		if (this != &other) {
			type = other.type;
			switch (other.type) {
			case datatypes::block:
				s = buildString(other.s->val);
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
				s = buildString(other.s->val);
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::string:
				s = buildString(other.s->val);
				break;
			case datatypes::variable:
				s = buildString(other.s->val);
				break;
			default:
				break;
			}
		}
	}
	value& value::operator=(value& other) {
		if (this != &other) {
			nuke(); // Delete it all
			type = other.type;
			switch (other.type) {
			case datatypes::block:
				s = buildString(other.s->val);
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
				s = buildString(other.s->val);
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::string:
				s = buildString(other.s->val);
				break;
			case datatypes::variable:
				s = buildString(other.s->val);
				break;
			default:
				break;
			}
			//other.nuke();
		}
		// by convention, always return *this
		return *this;
	}
	bool value::isNil() {
		return type == datatypes::nil;
	}
	value& value::operator=(const value& other) {
		if (this != &other) {
			nuke();
			type = other.type;
			switch (other.type) {
			case datatypes::block:
				s = buildString(other.s->val);
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
				s = buildString(other.s->val);
				break;
			case datatypes::nil:
				// No need to do anything
				break;
			case datatypes::number:
				n = other.n;
				break;
			case datatypes::string:
				s = buildString(other.s->val);
				break;
			case datatypes::variable:
				s = buildString(other.s->val);
				break;
			default:
				break;
			} 
		}
		// by convention, always return *this
		return *this;
	}
	bool operator==(const value& lhs, const value& rhs) {
		return lhs.getPrintable() == rhs.getPrintable();
	}
	value operator+(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n + rhs.n);
		}
		else {
			return lhs.getPrintable() + rhs.getPrintable();
		}
	}
	value operator-(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n - rhs.n);
		}
		else {
			return value(datatypes::error);
		}
	}
	value operator/(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n / rhs.n);
		}
		else {
			return value(datatypes::error);
		}
	}
	value operator*(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return value(lhs.n * rhs.n);
		}
		else {
			return value(datatypes::error);
		}
	}
	bool operator!=(const value& lhs, const value& rhs) {
		return !(lhs.getPrintable() == rhs.getPrintable());
	}
	bool operator>(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n > rhs.n;
		}
		return false;
	}
	bool operator<(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n < rhs.n;
		}
		return false;
	}
	bool operator>=(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n >= rhs.n;
		}
		return false;
	}
	bool operator<=(const value& lhs, const value& rhs) {
		if (lhs.type == datatypes::number && rhs.type == datatypes::number) {
			return lhs.n <= rhs.n;
		}
		return false;
	}
	value value::resolve(dms_state* state) {
		if (type == datatypes::variable && (*this)!=(*state->getMem())[getPrintable()]) {
			return (*state->getMem())[getPrintable()].resolve(state);
		}
		return *this;
	}
	void dms_args::push(value val) {
		args.push_back(val);
	}
	size_t dms_args::size() {
		return args.size();
	}
	std::string dms_string::getValue(dms_state* state) {
		std::stringstream temp;
		std::stringstream var;
		std::stringstream ind;
		bool varStart = false;
		bool indStart = false;
		for (size_t i = 0; i < val.size(); i++) {
			if (indStart && val[i] == '`') {
				std::string lookup = var.str();
				std::string index = ind.str();
				var.str("");
				var.clear();
				ind.str("");
				ind.clear();
				varStart = false;
				indStart = false;
				if (state->getMem()->count(lookup)) {
					value v = (*state->getMem())[lookup];
					if (v.type == datatypes::block) {
						if (state->getCharacter(v.getPrintable()) != nullptr) {
							character* cha = state->getCharacter(v.getPrintable());
							if (cha->values.count(index)) {
								temp << cha->values[index].getPrintable();
							}
							else {
								temp << cha->getName();
							}
						}
						else if (state->getEnvironment(v.getPrintable())!=nullptr) {
							enviroment* env = state->getEnvironment(v.getPrintable());
							if (env->values.count(index)) {
								temp << env->values[index].getPrintable();
							}
							else {
								temp << env;
							}
						}
						else {
							temp << "nil";
						}
					}
					else if (v.resolve(state).type == datatypes::env) {
						if(v.resolve(state).e->ipart.size()> std::stoi(index))
							temp << v.resolve(state).e->ipart[std::stoi(index)-(int64_t)1].getPrintable();
						else
							temp << "nil";
					}
					else {
						temp << v.resolve(state).getPrintable();
					}
				}
				else {
					temp << "nil";
				}
			}
			else if (indStart && val[i] == ':') {
				state->push_error(errors::error{ errors::badtoken,"Cannot index more than once in a string injection!" });
				varStart = false;
				indStart = false;
				var.str("");
				var.clear();
				ind.str("");
				ind.clear();
				return "";
			}
			else if (indStart) {
				ind << val[i];
			}
			else if (val[i] == '`' && !varStart) {
				varStart = true;

			}
			else if (val[i] == '`' && varStart) {
				std::string lookup = var.str();
				var.str("");
				var.clear();
				varStart = false;
				if (state->getMem()->count(lookup)) {
					value v = (*state->getMem())[lookup];
					if (v.type == datatypes::block) {
						if (state->getCharacter(v.s->getValue())) {
							temp << state->characters[v.s->getValue()]->getName();
						}
						else {
							temp << "nil";
						}
					}
					else {
						temp << v.resolve(state).getPrintable();
					}
				}
				else {
					temp << "nil";
				}
			}
			// A space is not allowed at all
			else if (val[i] == ':' && varStart) {
				indStart = true;
			}
			else if (val[i] == ' ' && varStart) {
				temp << var.str();
				varStart = false;
				var.str("");
				var.clear();
			}
			else if (varStart) {
				var << val[i];
			}
			else {
				temp << val[i];
			}
		}
		return temp.str();
	}
	std::string value::getPrintable() const {
		std::stringstream out;
		if (type == string) {
			out << s->getValue();
		}
		else if (type == number) {
			out << n;
		}
		else if (type == nil) {
			out << "nil";
		}
		else if (type == boolean) {
			if (b)
				out << "true";
			else
				out << "false";
		}
		else if (type == env) {
			out << "Env: " << this;
		}
		else if (type == custom) {
			out << "Custom Data: " << this;
		}
		else if (type == block) {
			out << s->getValue();
		}
		else if (type == datatypes::variable) {
			out << s->getValue(); // Do the lookup
		}
		return out.str();
	}
	dms_string* buildString(std::string str) {
		std::stringstream newstr;
		for (int i = 0; i < str.size(); i++)
			newstr << str[i];
		dms_string* dms_str = new dms_string{ newstr.str() };
		return dms_str;
	}
	std::string value::toString() const {
		std::stringstream temp;
		temp << this;
		return temp.str();
	}
	// Compile time
	void value::nuke() {
		delete s;
		delete e;
		delete c;
		s = nullptr;
		e = nullptr;
		c = nullptr;
	}
	std::ostream& operator << (std::ostream& out, const value& c) {
		if (c.type == string) {
			out << (char)c.type << c.s->getValue() << (char)0;
		}
		else if (c.type == number) {
			out << (char)c.type << c.n;
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
			out << (char)c.type << c.s->getValue();
		}
		else if (c.type == datatypes::variable) {
			out << (char)c.type << c.s->getValue(); // Do the lookup
		}
		return out;
	};
	// Fixed issue with memory not properly being cleaned up
	bool value::typeMatch(const value* o) const {
		return type == o->type;
	}
	void value::set(value* val) {
		if (type == datatypes::number) {
			n = val->n;
		}
		else if (type == datatypes::string || type == datatypes::block || type == datatypes::variable) {
			s->val = val->s->val;
		}
		else if (type == datatypes::boolean) {
			b = val->b;
		}
		else { // Handle custom and env
			return;
		}
		type = val->type;
	}
	void value::set(dms_string* str) {
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
	std::string dms_string::getValue() {
		return val;
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
	std::ostream& operator << (std::ostream& out, const dms_string& c) {
		out << c.val;
		return out;
	}
	std::ostream& operator << (std::ostream& out, const dms_boolean& c) {
		out << c.val;
		return out;
	}
	std::ostream& operator << (std::ostream& out, const dms_number& c) {
		out << c.val;
		return out;
	}
}