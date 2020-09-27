#include "dms_state.h"
#include "utils.h"
#include "Handlers.h"
#include <iostream>
using namespace dms::utils;
using namespace dms::exceptions;
using namespace dms::codes;
namespace dms {
	// This is a pointer to a choicehandler! dms_state and utils have codepedencies, so I have to do this.
	void dms_state::setHandler(void* hand) {
		this->handler = hand;
	}
	character* dms_state::getCharacter(std::string cha) {
		if (characters.count(cha)) {
			return characters[cha];
		}
		else {
			if (blockExists(cha)) {
				character* cc = new character;
				cc->fname = cha;
				codes::op code;
				cmd* c = nullptr;
				bool halt = false;
				size_t pos = 0;
				size_t max = 0;
				std::vector<cmd*> cmds;
				init(chunks[cha], pos, max, cmds);
				while (true) {
					c = cmds[pos++];
					code = c->opcode;

					switch (code)
					{
						// Handle flags here
					case STAT:
						if(c->args.args[0]->type == datatypes::variable && c->args.args[1]->type == datatypes::string)
							cc->paths.insert_or_assign(c->args.args[0]->s->getValue(), c->args.args[1]->s->getValue());
						else
							push_error(errors::error{ errors::invalid_type ,utils::concat("Expected variable: string! Got: ",datatype[c->args.args[0]->type],": ",datatype[c->args.args[1]->type]) });
						break;
					case ASGN:
						if (c->args.args[0]->s->getValue() == "fname") {
							cc->fname = c->args.args[1]->s->getValue();
						}
						else if (c->args.args[0]->s->getValue() == "lname") {
							cc->lname = c->args.args[1]->s->getValue();
						}
						else {
							cc->values.insert_or_assign(c->args.args[0]->s->getValue(), c->args.args[1]);
						}
						break;
					default:
						break;
					}

					if (pos == max) {
						// How did we get here? The end of a block?
						break;
					}
				}
				return cc;
			}
			else {
				push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to index a non existing character block!") });
				return nullptr;
			}
		}
		return nullptr;
	}
	size_t dms_state::seek(std::string label, std::vector<cmd*> cmds, codes::op code, size_t pos) {
		for (size_t i = pos; i < cmds.size(); i++) {
			if (cmds[i]->opcode == code && cmds[i]->args.args[0]->s->getValue() == label)
				return i;
		}
		return 0;
	}
	void dms_state::init(chunk* chunk, size_t& pos, size_t& max, std::vector<cmd*>& cmds) {
		pos = 0;
		max = chunk->cmds.size();
		cmds = chunk->cmds;
	}
	// Instance, multiple instances can allow different code to work side by side
	bool dms_state::run(std::string instance) {
		//codes::op code;
		//Spawn thread and run
		return true;
	}
	bool dms_state::run() {
		codes::op code;
		cmd* c = nullptr;
		bool halt = false;
		size_t pos=0;
		size_t max = 0;
		std::vector<cmd*> cmds;
		if (chunks[entry] == NULL) {
			push_error(errors::error{errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",entry,"]")});
			return false;
		}
		init(chunks["$INIT"],pos,max,cmds);

		//TODO: parse the cmds and do stuff
		// If this is running in a thread then stop will force this loop to stop
		size_t ln = 0;
		character* speaker = nullptr;
		std::string temp;
		while (!stop || !halt) {
			c = cmds[pos++];
			code = c->opcode;
			//print("\n(",pos,")> ",*c);
			//wait();
			switch (code)
			{
				// Handle flags here
				case ENTR:
					// When reading code from a file the flags are compiled and we need to load them
					entry = c->args.args[0]->s->getValue();
					break;
				case ENAB:
					enable(c->args.args[0]->s->getValue());
					break;
				case DISA:
					disable(c->args.args[0]->s->getValue());
					break;
				case LOAD:
					// Nothing needs to be done here
					break;
				case VERN:
					Cversion = c->args.args[0]->n->getValue();
					break;
				case USIN:
					// How we add modules into the code. This is the code that actually loads that data!
					break;
				// Flags handled
				case LIST:
					//We need to create an enviroment value then set that
					{
						dms_env* env = new dms_env;
						env->hpart.insert_or_assign("$size", c->args.args[1]);
						value* val = new value;
						val->set(env);
						assign(c->args.args[0], val);
					}
					break;
				case INST:
					{
						value* list = memory[c->args.args[0]->s->getValue()];
						list->e->pushValue(c->args.args[1]);
					}
					break;
				case HALT:
					//wait();
					break;
				case DSPD:
					if (speaker == nullptr) {
						push_error(errors::error{ errors::unknown ,utils::concat("A call to set speaker speed, but a speaker has not been defined!") });
						return false;
					}
					else {
						speaker->spd = c->args.args[0]->n->getValue();
					}
					break;
				case SSPK:
					if (characterExists(c->args.args[0]->s->getValue())){
						speaker = getCharacter(c->args.args[0]->s->getValue());
						if (speaker->lname != "") {
							utils::write(speaker->fname, " ", speaker->lname, ": ");
						}
						else {
							utils::write(speaker->fname, ": ");
						}
					}
					break;
				case APND:
					utils::write(c->args.args[0]->s->getValue());
					break;
				case DISP:
					utils::write(c->args.args[0]->s->getValue());
					break;
				case ASGN:
					assign(c->args.args[0], c->args.args[1]);
					break;
				case LINE:
					cur_line = c->args.args[0]->n->getValue();
					break;
				case NOOP:
					break;
				case CHOI:
					//Because we are using void* we must cast our pointers
					//The implementation of this however should mean that you only ever have to deal with one "ChoiceHandler. One annoying void*"
					{
						std::vector<std::string> args;
						std::string prompt = c->args.args[0]->s->getValue();
						std::string fn = c->args.args[1]->s->getValue();
						for (size_t i = 2; i < c->args.args.size(); i++)
							args.push_back(c->args.args[i]->resolve(memory)->s->getValue());
						size_t npos = (*(Handler*)handler).manageChoice(this, prompt, args);
						print("CHOI_", fn, "_", npos);
						size_t nnpos = seek(concat("CHOI_", fn, "_", npos),cmds,LABL,npos);
						if (!nnpos) {
							push_error(errors::error{ errors::choice_unknown ,utils::concat("Unknown choice!") });
							return false;
						}
						else {
							print(nnpos);
							pos = nnpos;
							wait();
						}
					}
					break;
				case JUMP:
					// Value assert resolves the data so a variable must eventually equal a string
					if (utils::valueassert(c->args, memory, datatypes::string)) {
						std::string block = c->args.args[0]->resolve(memory)->s->getValue();
						if (chunks[block] == NULL) {
							push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",block,"]") });
							return false;
						}
						else if (chunks[block]->type != bt_block) {
							push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non standard block [",block,"] type: ",getBlockType(chunks[block]->type))});
							return false;
						}
						else {
							init(chunks[block], pos, max, cmds);
						}
					}
					else {
						datatypes set = c->args.args[0]->resolve(memory)->type;
						push_error(errors::error{ errors::invalid_arguments, utils::concat("String expected got ",set), true,ln });
						return false;
					}
					break;
				default:
				break;
			}
			if (pos == max) {
				// How did we get here? The end of a block?
				break;
			}
		}

		return true;
	}
}