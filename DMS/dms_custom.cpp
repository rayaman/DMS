#include "value.h"
#include "utils.h"
//Implement the defaults
namespace dms {
	// These are pointers to other data that should not be deleted.
	extern "C" { // Custom data needs to be c ready
		const value* perNIL = new value;
		void dms_custom::_del() {
			state = nullptr;
			self = nullptr;
		}
		void dms_custom::_set(value* v) {
			self = v;
		}
		void dms_custom::Init(dms_state* state) {
			this->state = state;
		}
		value dms_custom::Index(value data) {
			state->push_error(errors::error{ errors::unknown,"Attempting to index a non enviroment!" });
			return value();
		}
		bool dms_custom::NewIndex(value var, value val) {
			state->push_error(errors::error{ errors::unknown,"Attempting to create a new index on a non enviroment!" });
			return false;
		}
		value dms_custom::Call(dms_args* args) {
			state->push_error(errors::error{ errors::unknown,"Attempting to call a non function value!" });
			return value();
		}
		value dms_custom::ToString() {
			return value(utils::concat("Custom: ", this));
		}
		value dms_custom::ADD(value left, value right) {
			state->push_error(errors::error{ errors::unknown,"Attempting to add a non number value!" });
			return value();
		}
		value dms_custom::SUB(value left, value right) {
			state->push_error(errors::error{ errors::unknown,"Attempting to subtract a non number value!" });
			return value();
		}
		value dms_custom::MUL(value left, value right) {
			state->push_error(errors::error{ errors::unknown,"Attempting to multiply a non number value!" });
			return value();
		}
		value dms_custom::DIV(value left, value right) {
			state->push_error(errors::error{ errors::unknown,"Attempting to divide a non number value!" });
			return value();
		}
		value dms_custom::POW(value left, value right) {
			state->push_error(errors::error{ errors::unknown,"Attempting to raise a non number value!" });
			return value();
		}
		value dms_custom::EQUAL(value left, value right) {
			return value(left.getPrintable() == right.getPrintable());
		}
		value dms_custom::LESS_THAN(value left, value right) {
			return value(left.getPrintable() < right.getPrintable());
		}
		value dms_custom::LESS_THAN_EQUAL(value left, value right) {
			return value(left.getPrintable() <= right.getPrintable());
		}
	}
}