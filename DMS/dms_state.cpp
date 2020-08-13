#include "dms_state.h"
namespace dms {
	void dms_state::push_error(errors::error err) {
		std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		if(err.crash)
			std::exit(err.code);
	}
	void dms_state::push_warning(errors::error err) {
		err.crash = false; // Force code to not crash then push the error
		if(enables.count("warnings"))
			push_error(err);
	}
}