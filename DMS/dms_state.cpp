#include "pch.h"
#include "dms_state.h"
#include "Handlers.h"
#include "core.h"
#include "sound.h"
namespace dms {
	value dms::blockInvoke(void* self, dms_state* state, dms_args* args) {
		std::string func = state->call_stack.top();
		if (state->functionExists(func)) {
			state->call_stack.pop();
			value ret;
			state->pushMem();
			memory* Fmem = state->getMem();
			// Only the nil argument
			if (args->args.size())
				for (int i = 0; i < state->chunks[func]->params.args.size(); i++)
					(*Fmem)[state->chunks[func]->params.args.at(i).getPrintable()] = args->args.at(i).resolve(state);

			if (!state->run(func, Fmem)) {
				std::vector<value> err = Fmem->examine(datatypes::error);
				if (err.size() > 0)
					state->push_error(errors::error{ errors::unknown ,err[0].getString() });
				else
					state->push_error(errors::error{ errors::unknown ,"Function Returned an error!" });
				state->popMem();// We need to restore the stack
				return value("Function returned an error", datatypes::error);
			}
			ret = state->return_stack.top();
			state->return_stack.pop();
			state->popMem();
			return ret;
		}
		return value();
	}



	void dms_state::init() {
		if (init_init || stop)
			return;

		init_init = true;
		cmd* c = new cmd;
		for (const auto& [key, val] : chunks) {
			if (val->type == bt_character || val->type == bt_env) {
				c->opcode = codes::ASGN;
				c->args.push(value(key, datatypes::variable));
				c->args.push(value(key, datatypes::block));
				chunks["$INIT"]->addCmd(c);
				if (val->type == bt_character) {
					c = new cmd;
					c->opcode = codes::CHAR;
					c->args.push(value(key));
					chunks["$INIT"]->addCmd(c);
				}
				c = new cmd;
			}
			else if (val->type == bt_method) {
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
		dms::core::init(this);
		if (!handler->OnStateInit(this))
			stop = true;
	}
	memory* dms_state::getMem() {
		return &mem_stack.top();
	}
	void dms_state::pushMem() {
		memory mem = memory();
		if (!mem_stack.empty()) {
			
			mem.parent = getMem();
		}
		mem_stack.push(mem);
	}
	void dms_state::pushMem(memory &mem) {
		mem_stack.push(mem);
	}
	void dms_state::popMem() {
		mem_stack.pop();
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
		c->type = bt_block;
		cmd* bn = new cmd;
		bn->opcode = codes::BLCK;
		bn->args.push(value("$END"));
		bn->args.push(value(bt_block));
		c->addCmd(bn);
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
		return (chunks.count(bk_name) && chunks[bk_name]->type == bt_character);
	}
	bool dms_state::environmentExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == bt_env);
	}
	bool dms_state::functionExists(std::string bk_name) {
		return (chunks.count(bk_name) && chunks[bk_name]->type == bt_method);
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
	bool dms_state::hasError() {
		return stop;
	}

	bool dms_state::Inject(std::string var, value* val) {
		return false;
	}

	bool dms_state::assoiateType(std::string type, Invoker* inv)
	{
		if(inv_map.count(type)) // Already exists!
			return false;

		inv_map.insert_or_assign(type,inv); // Create it
		return true;
	}

	bool dms_state::injectEnv(std::string name, enviroment* env)
	{
		std::string ename = name;
		assign(value(name, datatypes::variable), value(ename, datatypes::block));
		environments.insert_or_assign(ename, env);
		chunk* c = new chunk;
		c->type = bt_env;
		c->name = ename;
		cmd* cc = new cmd;
		cc->opcode = codes::NOOP;
		c->addCmd(cc);
		push_chunk(ename,c);
		return false;
	}

	bool dms_state::assign(value var, value val) {
		if (val.type == datatypes::error) {
			(*getMem())[var.getPrintable()] = val;
			push_error(errors::error{ errors::unknown ,val.getString() });
			return false;
		}
		val.state = this;
		var.state = this;
		(*getMem())[var.getPrintable()] = val;
		return true;
	}
	void dms_state::dump(std::string fn) {
		std::ofstream outputFile(fn);
		for (const auto& [key, val] : chunks) {
			outputFile << *val << std::endl;
		}
		outputFile.close();
	}
	void dms_state::push_chunk(std::string s, chunk* c) {
		chunks.insert_or_assign(s, c);
	}
	void dms_state::push_error(errors::error err) {
		if (stop)
			return;
		if (err.linenum != 0) {
			std::cout << err.err_msg << " On Line <" << err.linenum << ">" << std::endl;
		}
		else if (isEnabled("debugging")) {
			std::cout << err.err_msg << " In file: '" << cur_file << "' on Line <" << cur_line << ">" << std::endl;
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