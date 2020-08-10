#include "chunk.h"
namespace dms {
	void chunk::addCmd(cmd c) {
		cmds.push_back(c);
	}
}