#include "chunk.h"
namespace dms {
	void chunk::addCmd(cmd* c) {
		cmds.push_back(c);
	}
	void chunk::addLabel(std::string name) {
		size_t size = cmds.size(); // Get the command position for the label. This should be the command we created for the label
		labels.insert_or_assign(name,size); // Here we set the name of the label so we can find it as well as the position to jump to
	}
}