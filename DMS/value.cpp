#include "value.h"
#include "utils.h"
#include "string"
namespace dms {
	const std::string datatype[] = { "nil", "number", "boolean", "env", "string", "custom", "variable", "block" };
	std::vector<value*> _VALUES;
	value::value() {
		_VALUES.push_back(this); // Used for the interperter! In the end everything is a value
		// We need to clean up this stuff when it all comes crashing.
		// We also might clean things while the code is running.
		// Values at runtime aren't "Deleted, they are set to nil"
		// At the end we actually delete them!
	}
	value* value::resolve(std::unordered_map<std::string, value*> memory) {
		if (type == datatypes::variable) {
			return memory[s->getValue()]->resolve(memory); // Variable types return the value
		}
		return this;
	}
	void dms_args::push(value* val) {
		args.push_back(val);
	}
	std::string dms_string::getValue(dms_state* state) {
		std::stringstream temp;
		std::stringstream var;
		std::stringstream ind;
		bool varStart = false;
		bool indStart = false;
		for (size_t i = 0; i < length; i++) {
			if (indStart && val[i] == '`') {
				std::string lookup = var.str();
				std::string index = ind.str();
				var.str("");
				var.clear();
				ind.str("");
				ind.clear();
				varStart = false;
				indStart = false;
				if (state->memory.count(lookup)) {
					value* v = state->memory[lookup];
					if (v->type == datatypes::block) {
						if (state->getCharacter(v->s->getValue())!=nullptr) {
							character* cha = state->getCharacter(v->s->getValue());
							if (cha->values.count(index)) {
								temp << cha->values[index]->getPrintable();
							}
							else {
								temp << cha->getName();
							}
						}
						else {
							temp << "nil";
						}
					}
					else {
						temp << v->getPrintable();
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
				if (state->memory.count(lookup)) {
					value* v = state->memory[lookup];
					if (v->type == datatypes::block) {
						if (state->getCharacter(v->s->getValue())) {
							temp << state->characters[v->s->getValue()]->getName();
						}
						else {
							temp << "nil";
						}
					}
					else {
						temp << v->getPrintable();
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
			out << n->getValue();
		}
		else if (type == nil) {
			out << "nil";
		}
		else if (type == boolean) {
			if (b->getValue())
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
		size_t len = str.length();
		uint8_t* arr = new uint8_t[len];
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
	std::string value::toString() const {
		std::stringstream temp;
		temp << this;
		return temp.str();
	}
	value* buildValue() {
		return new value;
	}
	size_t count = 0;
	value* buildVariable() {
		count++;
		std::string val = utils::concat("$",count);
		return buildVariable(val);
	}
	value* buildBlock(std::string str) {
		value* val = new value{};
		val->set(buildString(str));
		val->type = block;
		return val;
	}
	value* buildVariable(std::string str) {
		value* val = new value{};
		val->set(buildString(str));
		val->type = variable;
		return val;
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
	void value::nuke() {
		delete[] s;
		delete[] b;
		delete[] n;
		delete[] e;
		delete[] c;
		s = nullptr;
		b = nullptr;
		n = nullptr;
		e = nullptr;
		c = nullptr;
	}
	// Fixed issue with memory not properly being cleaned up
	bool value::typeMatch(const value* o) const {
		return type == o->type;
	}
	void value::set(dms_string* str) {
		nuke();
		s = str;
		type = string;
	}
	void value::set(dms_boolean* bo) {
		nuke();
		b = bo;
		type = boolean;
	}
	void value::set(dms_number* num) {
		nuke();
		n = num;
		type = number;
	}
	void dms::value::set(dms_env* en) {
		nuke();
		e = en;
		type = env;
	}
	void value::set() {
		nuke();
		type = nil;
	}
	std::string dms_string::getValue() {
		std::stringstream temp;
		for (size_t i = 0; i < length; i++) {
			temp << val[i];
		}
		return temp.str();
	}
	void dms_env::pushValue(value* val) {
		ipart.push_back(val);
	}
	void dms_env::pushValue(value* ind, value* val) {
		if (val->type == nil) {
			hpart[ind->toString()]->nuke();
			hpart.erase(ind->toString());
			count--;
		}
		else {
			hpart.insert_or_assign(ind->toString(), val);
			count++;
		}
	}
	value* dms_env::getValue(value* ind) {
		if (ind->type == number) {
			return ipart.at((int)ind->n->getValue());
		}
		else if (ind->type == number) {
			return new value{}; // Return a nil value
		}
		else {
			return hpart.at(ind->toString());
		}
	}
}