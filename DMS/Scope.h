#pragma once
#include <vector>
#include "cmd.h"
namespace dms {
	enum scopetype {
		none,
		iff,
		elif,
		elsee,
		disp,
		choice,
		forr,
		whilee,
	};
	struct scope {
		std::vector<cmd> cmds;
		scopetype type = scopetype::none;
		void addCmd(cmd c) {
			cmds.push_back(c);
		}
	};
}