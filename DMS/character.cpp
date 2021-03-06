#include "pch.h"
#include "character.h"
#include "utils.h"
namespace dms {
	value character_setName(void* self, dms_state* state, dms_args* args) {
		if(args->args[0].type == datatypes::string){
			character* me = (character*)self;
			me->set("fname", args->args[0]);
		}
		return value();
	}
	character::character() {
		// Set the self so the registered function can access themselves
		_init(this);
		// Here we register the functions that the character object should have
		registerFunction("setName", character_setName);
	}
	std::string character::getName() {
		if (has("nickname")) {
			return values["nickname"].getPrintable();
		}
		if (seen && has("fname") && has("lname") && fullname) {
			return utils::concat(values["fname"].getPrintable()," ", values["lname"].getPrintable());
		}
		else if (seen && has("fname")) {
			return utils::concat(values["fname"].getPrintable());
		}
		else if (!seen && has("unknown")) {
			return values["unknown"].getPrintable();
		}
		else {
			return "unknown";
		}
	}
}