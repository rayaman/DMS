#pragma once
#include "pch.h"
#include "cmd.h"
namespace dms {
	enum blocktype {
		bt_block,
		bt_method,
		bt_env,
		bt_character,
		bt_menu
	};
	std::string getBlockType(const blocktype b);
	struct chunk
	{
		blocktype type = bt_block;
		dms_args params;
		std::string name = "";
		std::vector<cmd*> cmds;
		size_t pos = 0;
		size_t line = 0;
		void addCmd(cmd* c);
		std::map<std::string, size_t> labels; // Each chunk contains its own label data
		chunk(){}
		void addLabel(std::string name);
		friend std::ostream& operator << (std::ostream& out, const chunk& c) {
			for (size_t i = 0; i < c.cmds.size(); i++) {
				out << *(c.cmds[i]) << std::endl;// (char)255 << (char)254;
			}
			return out;
		}
	};
}

