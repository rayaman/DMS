#include "dms_state.h"
#include "utils.h"
#include "Handlers.h"
using namespace dms::utils;
using namespace dms::exceptions;
using namespace dms::codes;
namespace dms {
	// This is a pointer to a choicehandler! dms_state and utils have codepedencies, so I have to do this.
	void dms_state::setChoiceHandler(void* choi) {
		this->choi = choi;
	}
	void dms_state::init(chunk* chunk, size_t& pos, size_t& max, std::vector<cmd*>& cmds) {
		pos = 0;
		max = chunk->cmds.size();
		cmds = chunk->cmds;
	}
	// Instance, multiple instances can allow different code to work side by side
	bool dms_state::run(std::string instance) {
		codes::op code;
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
			push_error(errors::error{errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block <",entry,">")});
			return false;
		}
		init(chunks["$INIT"],pos,max,cmds);

		//TODO: parse the cmds and do stuff
		// If this is running in a thread then stop will force this loop to stop
		size_t ln = 0;
		while (!stop || !halt) {
			c = cmds[pos++];
			code = c->opcode;
			utils::print("> ",*c);
			//utils::wait();
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
				case LINE:
					ln = c->args.args[0]->n->getValue();
					break;
				case NOOP:
					break;
				case CHOI:
					if (utils::valueassertall(c->args, memory, datatypes::string)) {
						//Because we are using void* we must cast our pointers
						//The implementation of this however should mean that you only ever have to deal with one "ChoiceHandler. One annoying void*"
						pos += 2*(*(choiceHandler*)choi).manageChoice(this, c->args);
					}
					break;
				case JUMP:
					value* v1;
					// Value assert resolves the data so a variable must eventually equal a string
					if (utils::valueassert(c->args, memory, datatypes::string)) {
						std::string block = c->args.args[0]->resolve(memory)->s->getValue();
						if (chunks[block] == NULL) {
							push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block <",block,">") });
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
	END:

		return true;
	}
}