#include "dms_state.h"
namespace dms {
	// Instance, multiple instances can allow different code to work side by side
	bool dms_state::run(std::string instance) {
		codes::op code;
		//TODO: parse the cmds and do stuff

		/*switch (code)
		{
		case codes::NOOP:

			break;
		default:
			break;
		}*/
		return true;
	} 
}