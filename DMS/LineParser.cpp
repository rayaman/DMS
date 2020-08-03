#include "LineParser.h"
#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include "cmd.h"
#include <vector>
dms::LineParser::LineParser(std::string f) {
	fn = f;
}
std::vector<dms::cmd> dms::LineParser::Parse() {
	return Parse(fn);
}
std::vector<dms::cmd> dms::LineParser::Parse(std::string file) {
	std::vector<cmd> cmds;
	std::string li;
	std::ifstream myfile(file);
	if (myfile.is_open())
	{
		int l = 0;
		while (std::getline(myfile, li))
		{
			std::cout << li << std::endl;
		}
		myfile.close();
	} else {
		std::cout << "Unable to open file";
	}
	return cmds;
}
