#include "dms_state.h"
namespace dms {
	dms_state::dms_state() {
		// We should define the defaults for the enables
		enables.insert_or_assign("leaking",false);
		enables.insert_or_assign("debugging",false);
		enables.insert_or_assign("warnings",false); //
		enables.insert_or_assign("statesave",true); // Allows you to save state
		enables.insert_or_assign("omniscient",true); // Allows you to know who's who when you first meet them
	}
	bool dms_state::isEnabled(std::string flag) {
		if (enables.count(flag)) {
			return enables[flag];
		}
		return false;
	}
	void dms_state::dump(errors::error err) {
		std::cout << std::endl << "STATE DUMP" << std::endl << "Number of chunks: " << chunks.size();
		std::ofstream outputFile("dump.bin");
		for (const auto& [key, val] : chunks) {
			std::cout << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
		}
		//If the error has a chunk then we get the data from it
		if (err.current_chunk != nullptr) {
			outputFile << err.current_chunk->name << ":" << std::endl << *err.current_chunk << std::endl;
		}
		outputFile.close();
	}
	void dms_state::dump() {
		std::cout << "Number of chunks: " << chunks.size() << std::endl;
		std::ofstream outputFile("dump.bin");
		for (const auto& [key, val] : chunks) {
			std::cout << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
		}
		outputFile.close();
	}
	void dms_state::push_chunk(std::string s, chunk* c) {
		chunks.insert_or_assign(s, c);
	}
	void dms_state::push_error(errors::error err) {
		std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		if (err.crash)
			std::exit(err.code);
	}
	void dms_state::push_warning(errors::error err) {
		err.crash = false; // Force code to not crash then push the error
		if(enables.count("warnings"))
			push_error(err);
	}
}