#pragma once
#include "dms_state.h"
namespace dms {
	namespace core {
		void init(dms_state* state);
		value print(void* self, dms_state* state, dms_args* args);
		value type(void* self, dms_state* state, dms_args* args);
	}
}