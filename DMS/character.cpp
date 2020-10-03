#include "character.h"
#include "utils.h"
namespace dms {
	std::string character::getName() {
		if (values.count("nickname")) {
			return values["nickname"]->getPrintable();
		}
		if (seen && values.count("lname")) {
			return utils::concat(values["fname"]->getPrintable()," ", values["lname"]->getPrintable());
		}
		else if (seen) {
			return utils::concat(values["fname"]->getPrintable());
		}
		else {
			return values["unknown"]->getPrintable();
		}
	}
}