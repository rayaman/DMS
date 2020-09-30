#include "dms_state.h"
#include "utils.h"
#include "Handlers.h"
#include <iostream>
#include <chrono>
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
				if (isEnabled("omniscient")) {
					cc->seen = true;
				}
				else {
					cc->seen = false;
				}
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
							if(!typeAssert(c->args.args[1],datatypes::string))
								return nullptr;
							cc->fname = c->args.args[1]->s->getValue();
						}
						else if (c->args.args[0]->s->getValue() == "lname" && c->args.args[1]->type == datatypes::string) {
							if (!typeAssert(c->args.args[1], datatypes::string))
								return nullptr;
							cc->lname = c->args.args[1]->s->getValue();
						}
						else if (c->args.args[0]->s->getValue() == "unknown" && c->args.args[1]->type == datatypes::string) {
							if (!typeAssert(c->args.args[1], datatypes::string))
								return nullptr;
							cc->unknown = c->args.args[1]->s->getValue();
						}
						else if (c->args.args[0]->s->getValue() == "known" && c->args.args[1]->type == datatypes::boolean) {
							if (!typeAssert(c->args.args[1], datatypes::boolean))
								return nullptr;
							cc->seen = c->args.args[1]->b->getValue();
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
				characters.insert_or_assign(cha, cc);
				// Call Character event!
				(*(Handler*)handler).OnSpeakerCreated(this,cc);
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
					Sversion = c->args.args[0]->n->getValue();
					// Version X.xx
					// X: Will not guarantee compatibality. Code breaking changes should be expected especially on lower versions
					// xx: Will not break compatibality. They may add features as it goes up the ranks, but those changes do not break older code. If they do they will increase the X part and not the xx part
					if (Sversion > Iversion) {
						push_error(errors::error{errors::incompatible_version ,"This script was made for a later version of this interperter!"});
						return false;
					}
					else if ((int)Iversion > (int)(Sversion)) {
						push_warning(errors::error{ errors::incompatible_version ,"This script was made for an older version of this interperter, some features might not work!" });
					}
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
					std::this_thread::sleep_for(std::chrono::milliseconds(700));
					std::cout << std::endl;
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
					//Because we are using void* we must cast our pointers
					if (characterExists(c->args.args[0]->s->getValue())){
						speaker = getCharacter(c->args.args[0]->s->getValue());
						if (!(*(Handler*)handler).handleSpeaker(this, speaker))
							return false;
					}
					else {
						push_error(errors::error{ errors::disp_unknown,concat("Unknown character '",c->args.args[0]->s->getValue(),"'!")});
						return false;
					}
					break;
				case APND:
					utils::write(c->args.args[0]->s->getValue(this));
					break;
				case DISP:
					utils::write(c->args.args[0]->s->getValue(this));
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
					{
						std::vector<std::string> args;
						std::string prompt = c->args.args[0]->s->getValue();
						std::string fn = c->args.args[1]->s->getValue();
						for (size_t i = 2; i < c->args.args.size(); i++)
							args.push_back(c->args.args[i]->resolve(memory)->s->getValue());
						size_t npos = (*(Handler*)handler).handleChoice(this, prompt, args);
						size_t nnpos = seek(concat("CHOI_", fn, "_", npos),cmds,LABL,npos);
						if (!nnpos) {
							push_error(errors::error{ errors::choice_unknown ,utils::concat("Unknown choice!") });
							return false;
						}
						else {
							pos = nnpos;
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
						push_error(errors::error{ errors::invalid_arguments, utils::concat("String expected got ",datatype[set]), true,ln });
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