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
	void dms_state::setHandler(Handler* hand) {
		this->handler = hand;
	}
	void checkCharacter(character* cc,std::string index, datatypes type) {
		value* val = cc->get(index);
		if (val==nullptr) {
			if (type == datatypes::string)
				cc->values[index] = buildValue("");
			else if (type == datatypes::boolean) {
				cc->values[index] = buildValue(false);
			}
		}
		else if (val != nullptr) {
			if (val->type != type) {
				cc->values.erase(index);
			}
		}
	}
	enviroment* dms_state::getEnviroment(std::string env) {
		if (enviroments.count(env)) {
			return enviroments[env];
		}
		else {
			if (blockExists(env)) {
				enviroment* e = new enviroment;
				if (!run(env, &e->values)) {
					return nullptr;
				}
				enviroments.insert_or_assign(env, e);
				handler->OnEnviromentCreated(this, e);
				return e;
			}
			else {
				push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to index a non existing enviroment block!") });
				return nullptr;
			}
		}
		return nullptr;
	}
	character* dms_state::getCharacter(std::string cha) {
		if (characters.count(cha)) {
			characters[cha]->seen = true;
			return characters[cha];
		}
		else {
			if (blockExists(cha)) {
				character* cc = new character;
				cc->set("fname", buildValue(cha));
				cc->set("lname", buildValue(""));
				cc->set("unknown", buildValue("Unknown"));
				cc->set("known", buildValue(false));
				if (isEnabled("omniscient")) {
					cc->seen = true;
				}
				else {
					cc->seen = false;
				}
				if (run(cha,&cc->values)) {
					checkCharacter(cc, "fname",datatypes::string);
					checkCharacter(cc, "lname", datatypes::string);
					checkCharacter(cc, "unknown", datatypes::string);
					checkCharacter(cc, "known", datatypes::boolean);
				}
				else {
					return nullptr;
				}
				characters.insert_or_assign(cha, cc);
				// Call Character event!
				handler->OnSpeakerCreated(this,cc);
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
		if (chunks[entry] == NULL) {
			push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",entry,"]") });
			return false;
		}
		return run("$INIT",&memory);
	}
	bool dms_state::run(std::string ent, std::unordered_map<std::string, value*>* mem) {
		codes::op code;
		cmd* c = nullptr;
		bool halt = false;
		size_t pos=0;
		size_t max = 0;
		std::vector<cmd*> cmds;
		init(chunks[ent],pos,max,cmds);

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
				case EXIT:
					if (c->args.args.size()) {
						exitcode = c->args.args[0]->n->getValue();
					}
					return true;
					break;
				case LIST:
					//We need to create an enviroment value then set that
					{
						dms_env* env = new dms_env;
						env->hpart.insert_or_assign("$size", c->args.args[1]);
						value* val = new value;
						val->set(env);
						assign(mem,c->args.args[0], val);
					}
					break;
				case INST:
					{
						value* list = (*mem)[c->args.args[0]->s->getValue()];
						list->e->pushValue(c->args.args[1]);
					}
					break;
				case HALT:
					//wait();
					sleep(700);
					std::cout << std::endl;
					break;
				case WAIT:
					sleep((int)(c->args.args[0]->n->getValue()*1000));
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
						if (!handler->handleSpeaker(this, speaker))
							return false;
					}
					else {
						push_error(errors::error{ errors::disp_unknown,concat("Unknown character '",c->args.args[0]->s->getValue(),"'!")});
						return false;
					}
					break;
				case APND:
					if (!handler->handleMessageAppend(this, c->args.args[0]->s->getValue(this)))
						return false;
					break;
				case DISP:
					if (!handler->handleMessageDisplay(this, c->args.args[0]->s->getValue(this)))
						return false;
					break;
				case ASGN:
					assign(mem,c->args.args[0], c->args.args[1]);
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
							args.push_back(c->args.args[i]->resolve(*mem)->s->getValue());
						size_t npos = handler->handleChoice(this, prompt, args);
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
					if (utils::valueassert(c->args, *mem, datatypes::string)) {
						std::string block = c->args.args[0]->resolve(*mem)->s->getValue();
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
						datatypes set = c->args.args[0]->resolve(*mem)->type;
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