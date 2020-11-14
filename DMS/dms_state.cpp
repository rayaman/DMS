#include "dms_state.h"
#include "Handlers.h"
namespace dms {
	void dms_state::init() {
		if (init_init)
			return;
		init_init = true;
		cmd* c = new cmd;
		for (const auto& [key, val] : chunks) {
			if (val->type == blocktype::bt_character || val->type == blocktype::bt_env) {
				c->opcode = codes::ASGN;
				c->args.push(value(key, datatypes::variable));
				c->args.push(value(key, datatypes::block));
				chunks["$INIT"]->addCmd(c);
				c = new cmd;
			}
			else if (val->type == blocktype::bt_method) {
				c->opcode = codes::RETN;
				c->args.push(value());
				val->addCmd(c);
				c = new cmd;
			}
		}

		c->opcode = codes::JUMP;
		if (entry != "$undefined")
			c->args.push(value(entry));
		else
			c->args.push(value(chunks.begin()->first));
		chunks["$INIT"]->addCmd(c);
		if (!handler->OnStateInit(this))
			stop = true;
	}
	memory* dms_state::getMem() {
		return &mem_stack.top();
	}
	void dms_state::pushMem() {
		mem_stack.push(memory());
	}
	void dms_state::popMem() {
		mem_stack.pop();
	}
	value dms::blockInvoke(void* self, dms_state* state, dms_args* args) {
		std::string func = state->call_stack.top();
		if (state->functionExists(func)) {
			state->call_stack.pop();
			value ret;
			state->pushMem();
			memory* Fmem = state->getMem();
			for (int i = 0; i < state->chunks[func]->params.args.size(); i++) {
				(*Fmem)[state->chunks[func]->params.args[i].getPrintable()] = args->args[i].resolve(state);
			}
			state->run(func, Fmem);
			ret = state->return_stack.top();
			state->return_stack.pop();
			state->popMem();
			return ret;
		}
		return value();
	}
	dms_state::dms_state() {
		// We should define the defaults for the enables
		pushMem(); // Main memory
		disable("forwardlabels");
		disable("leaking");
		disable("debugging");
		disable("warnings");
		disable("omniscient");
		disable("fullname");
		enable("statesave");
		chunk* c = new chunk;
		c->name = "$END";
		c->type = blocktype::bt_block;
		cmd* cc = new cmd;
		cc->opcode = codes::EXIT;
		cc->args.push(value(0));
		c->addCmd(cc);
		push_chunk("$END", c);
		setHandler(new Handler); // Use the default implementation
		invoker.registerFunction("$BlockInvoke$", blockInvoke);
	}
	bool dms_state::typeAssert(value val, datatypes type) {
		if (val.type != type) {
			push_error(errors::error{ errors::invalid_type ,utils::concat("Expected a ",datatype[type]," got a ",datatype[val.type],"!") });
			return false;
		}
		return true;
	}
	bool dms_state::characterExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == blocktype::bt_character);
	}
	bool dms_state::environmentExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == blocktype::bt_env);
	}
	bool dms_state::functionExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == blocktype::bt_method);
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
	bool dms_state::hasError() {
		return stop;
	}

	bool dms_state::Inject(std::string var, value* val) {
		return false;
	}

	bool dms_state::assign(value var, value val) {
		(*getMem())[var.getPrintable()] = val;
		return true;
	}
	void dms_state::dump(bool print) {
		if (print)
			std::cout << "Number of chunks: " << chunks.size() << std::endl;
		std::ofstream outputFile("dump.bin");
		for (const auto& [key, val] : chunks) {
			if(print)
				std::cout << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
			outputFile << "Key: " << key << "<" << getBlockType(val->type) << ">" << std::endl << *val << std::endl;
		}
		outputFile.close();
	}
	void dms_state::push_chunk(std::string s, chunk* c) {
		chunks.insert_or_assign(s, c);
	}
	void dms_state::push_error(errors::error err) {
		if (err.linenum != 0) {
			std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		}
		else if (isEnabled("debugging")) {
			std::cout << err.err_msg << " On Line <" << cur_line << ">" << std::endl;
		}
		else {
			std::cout << err.err_msg << std::endl;
		}
		this->err = err;
		if (err.crash)
			stop = true;
	}
	void dms_state::push_warning(errors::error err) {
		err.crash = false; // Force code to not crash then push the error
		if (isEnabled("warnings")) {
			err.err_msg = utils::concat("Warning: ", err.err_msg);
			push_error(err);
		}
	}
}