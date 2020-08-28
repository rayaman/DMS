#pragma once
#include "cmd.h"
#include <vector>
namespace dms {
	enum blocktype {
		bt_block,
		bt_method,
		bt_env,
		bt_character,
		bt_menu
	};
	struct chunk
	{
		blocktype type = bt_block;
		dms_args params;
		std::string name = "";
		std::vector<cmd*> cmds;
		size_t pos = 0;
		size_t line = 0;
		void addCmd(cmd* c);
		chunk(){}
	};
}

