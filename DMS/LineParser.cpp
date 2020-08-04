#include "LineParser.h"

using namespace dms::tokens;
namespace dms {
	cmd* tokenizer(std::vector<token>* tok) {

	}
	LineParser::LineParser(std::string f) {
		fn = f;

	}
	LineParser::LineParser() {}
	dms_state* dms::LineParser::Parse() {
		if (fn == "") {
			std::cout << "ERROR: You did not provide the constructor with a file path!" << std::endl;
			return nullptr;
		}
		return Parse(fn);
	}
	dms_state* dms::LineParser::Parse(std::string file) {
		dms_state* state = new dms_state();
		return Parse(state, fn);
	}
	dms_state* dms::LineParser::Parse(dms_state* state, std::string file) {
		std::map<std::string, chunk> chunks;
		std::vector<token> tokens;
		std::string li;
		std::ifstream myfile(file);
		std::stringstream rawdata;
		// Read whole file into a string
		if (myfile.is_open())
		{
			rawdata << myfile.rdbuf();
			myfile.close();
			std::cout << rawdata.str() << std::endl;
		}
		else {
			std::cout << "Unable to open file";
			delete[] state; // Cleanup
			return nullptr;
		} 
		
		// Data is good now let's parse



		return state;
	}
}