#pragma once
#include "pch.h"
#include "value.h"
#include "codes.h"
namespace dms {
	struct cmd
	{
		dms::codes::op opcode = codes::NOOP;
		dms_args args;
		friend std::ostream& operator << (std::ostream& out, const cmd& c) {
			out << codes::list[c.opcode] << " ";
			for (size_t i = 0; i < c.args.args.size(); i++) {
				out << c.args.args[i] << " ";
			}
			return out;
		}
		// For writing to a file
		std::string toString() {
			std::stringstream str;
			str << opcode << args.toString() << (char)0;
			return str.str();
		}
	};
}
