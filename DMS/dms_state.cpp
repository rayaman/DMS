#include "dms_state.h"
#include "Handlers.h"
namespace dms {
	dms_state::dms_state() {
		// We should define the defaults for the enables
		enables.insert_or_assign("leaking",false);
		enables.insert_or_assign("debugging",false);
		enables.insert_or_assign("warnings",false); //
		enables.insert_or_assign("statesave",true); // Allows you to save state
		enables.insert_or_assign("omniscient",true); // Allows you to know who's who when you first meet them
		chunk* c = new chunk;
		c->name = "$END";
		c->type = blocktype::bt_block;
		cmd* cc = new cmd;
		cc->opcode = codes::EXIT;
		cc->args.push(buildValue(0));
		c->addCmd(cc);
		push_chunk("$END", c);
		setChoiceHandler(new choiceHandler); // Use the default implementation
	}
	bool dms_state::characterExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == blocktype::bt_character);
	}
	bool dms_state::blockExists(std::string bk_name) {
		return (chunks.count(bk_name));
	}
	void dms_state::enable(std::string flag) {
		enables[flag] = true;
	}
	void dms_state::disable(std::string flag) {
		enables[flag] = false;
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
			outputFile << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
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
			outputFile << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
		}
		outputFile.close();
	}
	void dms_state::push_chunk(std::string s, chunk* c) {
		chunks.insert_or_assign(s, c);
	}
	void dms_state::push_error(errors::error err) {
		std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		this->err = err;
		if (err.crash)
			stop = true;
	}
	void dms_state::push_warning(errors::error err) {
		err.crash = false; // Force code to not crash then push the error
		if(isEnabled("warnings"))
			push_error(err);
	}
}