#include "character.h"
#include "utils.h"
namespace dms {
	std::string character::getName() {
		if (nickname != "") {
			return nickname;
		}
		if (seen && lname !="") {
			return utils::concat(fname," ", lname);
		}
		else if (seen) {
			return utils::concat(fname);
		}
		else {
			return unknown;
		}
	}
}