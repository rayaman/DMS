#include "character.h"
#include "utils.h"
namespace dms {
	std::string character::getName() {
		if (has("nickname")) {
			return values["nickname"]->getPrintable();
		}
		if (seen && has("fname") && has("lname")) {
			return utils::concat(values["fname"]->getPrintable()," ", values["lname"]->getPrintable());
		}
		else if (seen && has("fname")) {
			return utils::concat(values["fname"]->getPrintable());
		}
		else if (!seen && has("unknown")) {
			return values["unknown"]->getPrintable();
		}
		else {
			return "unknown";
		}
	}
}