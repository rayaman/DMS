#include "dms_state.h"
namespace dms {
	void dms_state::dump(errors::error err) {
		std::cout << "Number of chunks: " << chunks.size();
		std::ofstream outputFile("dump.bin");
		for (const auto& [key, val] : chunks) {
			std::cout << "Key: " << key << " Value: " << *val << '\n';
		}
		//If the error has a chunk then we get the data from it
		if (err.current_chunk != nullptr) {
			outputFile << err.current_chunk->name << ":" << std::endl << *err.current_chunk << std::endl;
		}
		outputFile.close();
	}
	void dms_state::dump() {
		std::cout << "Number of chunks: " << chunks.size();
		std::ofstream outputFile("dump.bin");
		for (const auto& [key, val] : chunks) {
			std::cout << "Key: " << key << " Value: " << *val << '\n';
		}
		outputFile.close();
	}
	void dms_state::push_chunk(std::string s, chunk* c) {
		std::cout << "Pushing Chunk: " << s << std::endl;
		chunks.insert_or_assign(s, c);
	}
	void dms_state::push_error(errors::error err) {
		std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		if (err.crash) {
			dump(err);
			std::exit(err.code);
		}
	}
	void dms_state::push_warning(errors::error err) {
		err.crash = false; // Force code to not crash then push the error
		if(enables.count("warnings"))
			push_error(err);
	}
}