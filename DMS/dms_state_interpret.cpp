#include "pch.h"
#include "dms_state.h"
#include "utils.h"
#include "Handlers.h"
using namespace dms::utils;
using namespace dms::exceptions;
using namespace dms::codes;
namespace dms {
	// This is a pointer to a choicehandler! dms_state and utils have codepedencies, so I have to do this.
	void dms_state::setHandler(Handler* hand) {
		this->handler = hand;
	}
	enviroment* dms_state::getEnvironment(std::string env) {
		if (environments.count(env)) {
			return environments[env];
		}
		else {
			if (blockExists(env)) {
				enviroment* e = new enviroment;
				if (run(env, &e->values)) {
					std::cout << "Ran :D" << std::endl;
				}
				environments.insert_or_assign(env, e);
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
	void checkCharacter(character* cha, std::string index, datatypes type) {
		auto val = cha->values[index];
		// If a type mismatch is present, overwrite them with the defaults
		if (val.type != type) {
			if (type == datatypes::string)
				cha->values[index] = value("");
			else if (type == datatypes::boolean) {
				cha->values[index] = value(false);
			}
			else if (type == datatypes::number) {
				cha->values[index] = value(0);
			}
		}
	}
	character* dms_state::getCharacter(std::string cha) {
		if (characters.count(cha)) {
			characters[cha]->seen = true;
			return characters[cha];
		}
		else {
			if (blockExists(cha)) {
				pushMem();
				character* cc = new character;
				cc->fullname = isEnabled("fullname");
				/*cc->set("fname", cha);
				cc->set("lname", "");
				cc->set("unknown", "Unknown");
				cc->set("known", false);*/
				if (isEnabled("omniscient")) {
					cc->seen = true;
				}
				else {
					cc->seen = false;
				}
				if (run(cha,&cc->values)) {
					cc->values = *getMem();
					checkCharacter(cc, "fname",datatypes::string);
					if (cc->get("fname") == value("")) {
						cc->set("fname",value(cha));
					}
					checkCharacter(cc, "unknown", datatypes::string);
					checkCharacter(cc, "known", datatypes::boolean);
					if (cc->get("known").b) {
						cc->seen = true;
					}
				}
				else {
					popMem();
					return nullptr;
				}
				characters.insert_or_assign(cha, cc);
				// Call Character event!
				handler->OnSpeakerCreated(this,cc);
				popMem();
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
			if (cmds[i]->opcode == code && cmds[i]->args.args[0].getPrintable() == label)
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
	bool dms_state::error(std::string err)
	{
		push_error(errors::error{ errors::unknown ,err });
		return false;
	}
	bool dms_state::run() {
		if (stop)
			return false;
		if (chunks[entry] == NULL) {
			push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",entry,"]") });
			return false;
		}
		return run("$INIT",getMem());
	}
	bool dms_state::run(std::string ent, memory* mem) {
		if (stop) {
			exitcode = 1;
			return false;
		}
		if (chunks[ent] == NULL) {
			push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",ent,"]") });
			return false;
		}

		pushStateData();
		pushMem(mem);
		init(chunks[ent], n_pos, n_max, n_cmds);
		bool working = true;
		while (working) {
			working = next(mem);
		}
		popMem();
		popStateData();
		return true;
	}
	bool dms_state::next(memory* mem) {
		if (n_halt || stop)
			return false;
		if (!n_cmds.size())
			init(chunks["$INIT"], n_pos, n_max, n_cmds);
		n_c = n_cmds[n_pos++];
		n_code = n_c->opcode;
		//debug("(",n_pos,")> ",*n_c);
		//wait();
		switch (n_code)
		{
			// Handle flags here
		case ENTR:
			// When reading code from a file the flags are compiled and we need to load them
			entry = n_c->args.args[0].getPrintable();
			break;
		case ENAB:
			enable(n_c->args.args[0].getPrintable());
			break;
		case DISA:
			disable(n_c->args.args[0].getPrintable());
			break;
		case codes::FILE:
			cur_file = n_c->args.args[0].getPrintable();
		case LOAD:
			// Nothing needs to be done here
			break;
		case VERN:
			Sversion = n_c->args.args[0].n;
			// Version X.xx
			// X: Will not guarantee compatibality. Code breaking changes should be expected especially on lower versions
			// xx: Will not break compatibality. They may add features as it goes up the ranks, but those changes do not break older code. If they do they will increase the X part and not the xx part
			if (Sversion > Iversion) {
				push_error(errors::error{ errors::incompatible_version ,"This script was made for a later version of this interperter!" });
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
			if (n_c->args.args.size()) {
				exitcode = (int)n_c->args.args[0].n;
			}
			return true;
			break;
		case KNOT:
		{
			value cmp = n_c->args.args[0];
			if (cmp.resolve(this).type == datatypes::boolean || cmp.resolve(this).isNil()) {
				if (!cmp.resolve(this).b || cmp.resolve(this).isNil()) {
					if (!assign(cmp, value(true))) {
						return false;
					}
				}
				else {
					if (!assign(cmp, value(false))) {
						return false;
					}
				}
			}
			else {
				if (!assign(cmp, value(false))) {
					return false;
				}
			}
			break;
		}
		case IFFF:
		{
			value cmp = n_c->args.args[0].resolve(this);
			value gt = n_c->args.args[1].resolve(this);
			if (cmp.type == datatypes::boolean || cmp.isNil()) {
				if (!cmp.b || cmp.isNil()) {
					n_pos = seek(gt.getPrintable(), n_cmds, LABL, n_pos);
				}
			}
			break;
		}
		case GOTO:
		{
			value labl = n_c->args.args[0];
			if (isEnabled("forwardlabels")) {
				size_t nnpos = seek(labl.resolve(this).getPrintable(), n_cmds, LABL, n_pos); // Seek from next pos (pos++) to end of cmds
				if (!nnpos) {
					// This could be a method, it could, but it isn't. We jump to searching from the beginning of the block
					goto seek_from_1;
				}
				else {
					n_pos = nnpos;
				}
				break;
			}
		seek_from_1:
			size_t nnpos = seek(labl.resolve(this).getPrintable(), n_cmds, LABL, 0);
			if (!nnpos) {
				push_error(errors::error{ errors::choice_unknown ,utils::concat("Unknown label '",labl.resolve(this).getPrintable(),"'!") });
				return false;
			}
			else {
				n_pos = nnpos;
			}
		}
		break;
		case RETN:
		{
			value ret = n_c->args.args[0].resolve(this);
			//c->args.args[0]->nuke(); // Lets clean this up
			return_stack.push(ret);
			return true; // We did it, let's return this
		}
		break;
		case OFUN:
		{
			if (n_c->args.args[0].resolve(this).type == datatypes::custom) {
				auto cust = n_c->args.args[0].resolve(this);
				auto fname = n_c->args.args[1].getString();
				auto assn = n_c->args.args[2];
				dms_args args;

				for (int i = 3; i < n_c->args.args.size(); i++) {
					args.push(n_c->args.args[i]);
				}
				if (inv_map.count(cust.ctype)) {
					Invoker* inv = inv_map[cust.ctype];
					auto ret = inv->Invoke(fname, cust.c, this, &args);
					if (assn.type != datatypes::nil) {
						if (!assign(assn, ret)) {
							return false;
						}
					}
				}
				else {
					error("Custom object not assoiated with any functions!");
				}
			}
			else {
				std::string obj = n_c->args.args[0].getPrintable();
				if (obj == "nil") {
					obj = n_c->args.args[0].getPrintable();
				}
				std::string funcname = n_c->args.args[1].getPrintable();
				value assn = n_c->args.args[2];
				dms_args args;

				for (int i = 3; i < n_c->args.args.size(); i++) {
					args.push(n_c->args.args[i]);
				}

				value ret;
				if (characterExists(obj)) {
					ret = getCharacter(obj)->Invoke(funcname, this, &args);
				}
				else if (environmentExists(obj)) {
					ret = getEnvironment(obj)->Invoke(funcname, this, &args);
				}
				if (ret.type == datatypes::error)
					return error(ret.getString());
				if (assn.type != datatypes::nil) {
					if (!assign(assn, ret)) {
						return false;
					}
				}
			}
		}
		break;
		case FUNC:
		{
			std::string funcname = n_c->args.args[0].getPrintable();
			value assn = n_c->args.args[1];
			dms_args args;
			value ret;
			for (int i = 2; i < n_c->args.args.size(); i++) {
				args.push(n_c->args.args[i]);
			}
			// If we have a block made function we don't invoke like normal
			if (functionExists(funcname)) {
				call_stack.push(funcname);
				ret = invoker.Invoke("$BlockInvoke$", this, &args);
			}
			else {
				ret = invoker.Invoke(funcname, this, &args);
			}
			if (ret.type == datatypes::error) {
				return error(ret.getString());
			}
			if (assn.type != datatypes::nil) {
				if (!assign(assn, ret)) {
					return false;
				}
			}
		}
		break;
		case ASID:
		{
			value env = n_c->args.args[1];
			value indx = n_c->args.args[2].resolve(this);
			value assn = n_c->args.args[3].resolve(this);
			if (env.type == datatypes::block && blockExists(env.getString())) { // If this is a block let's handle this 
				enviroment* e = nullptr;
				if (environments.count(env.getString())) {
					e = environments[env.getString()];
				}
				else if (characters.count(env.getString())) {
					e = characters[env.getString()];
				}
				e->values[indx.getString()] = assn;
			}
			else if (env.type == datatypes::env) {
				if (indx.type == datatypes::number) {
					env.e->pushValue(assn);
				}
				else {
					push_error(errors::error{ errors::invalid_type ,concat("Expected a number value got ",datatype[indx.type]) });
					return false;
				}
			}
			else if (env.type == datatypes::custom) {
				//env.c->NewIndex(indx, assn);
				//if(!assign( assn, env->c->Index(indx));
				// Call the method within the custom data
			}
			break;
		}
		case ADD:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = o1.resolve(this) + o2.resolve(this);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case SUB:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = o1.resolve(this) - o2.resolve(this);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case MUL:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = o1.resolve(this) * o2.resolve(this);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case DIV:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = o1.resolve(this) / o2.resolve(this);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case POW:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = pow(o1.resolve(this).n, o2.resolve(this).n);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case MOD:
		{
			value assn = n_c->args.args[0];
			value o1 = n_c->args.args[1];
			value o2 = n_c->args.args[2];
			value ret = std::fmod(o1.resolve(this).n, o2.resolve(this).n);
			if (!assign(assn, ret)) {
				return false;
			}
		}
		break;
		case INDX:
		{
			value assn = n_c->args.args[0];
			value env = n_c->args.args[1];
			value indx = n_c->args.args[2].resolve(this);
			if (env.type == datatypes::block && blockExists(env.getString())) { // If this is a block let's handle this 
				enviroment* e = nullptr;
				if (environments.count(env.getString())) {
					e = environments[env.getString()];
				}
				else if (characters.count(env.getString())) {
					e = characters[env.getString()];
				}
				if (!assign(assn, e->values[indx.getPrintable()])) {
					return false;
				}
			}
			else if (env.type == datatypes::env) {
				if (indx.type == datatypes::number) {
					if (!assign(assn, env.e->getValue(indx))) {
						return false;
					}
				}
				else {
					push_error(errors::error{ errors::invalid_type ,concat("Expected a number value got ",datatype[indx.type]) });
					return false;
				}
			}
			else if (env.type == datatypes::custom) {
				//if(!assign( assn, env.c->Index(indx))) {
				//	return false;
				//}
				// Call the method within the custom data
			}
		}
		break;
		case LIST:
			//We need to create an enviroment value then set that
		{
			dms_list* env = new dms_list;
			// Inject the size of the list
			env->hpart["$size"] = n_c->args.args[1];
			value val = new value;
			val.set(env);
			if (!assign(n_c->args.args[0], val)) {
				return false;
			}
		}
		break;
		case INST:
		{
			value list = (*mem)[n_c->args.args[0].getPrintable()];
			list.e->pushValue(n_c->args.args[1]);
		}
		break;
		case COMP:
		{
			comp cmp = (comp)n_c->args.args[0].i;
			value assn = n_c->args.args[1];
			value left = n_c->args.args[2].resolve(this);
			value right = n_c->args.args[3].resolve(this);
			switch (cmp) {
			case comp::eq: {
				if (!assign(assn, left == right)) {
					return false;
				}
				break;
			}
			case comp::gt: {
				if (left.isNil() || right.isNil()) { push_error(errors::error{ errors::unknown ,"Attempt to compare a nil value!" }); return false; }
				if (!assign(assn, left > right)) {
					return false;
				}
				break;
			}
			case comp::gteq: {
				if (left.isNil() || right.isNil()) { push_error(errors::error{ errors::unknown ,"Attempt to compare a nil value!" }); return false; }
				if (!assign(assn, left >= right)) {
					return false;
				}
				break;
			}
			case comp::lt: {
				if (left.isNil() || right.isNil()) { push_error(errors::error{ errors::unknown ,"Attempt to compare a nil value!" }); return false; }
				if (!assign(assn, left < right)) {
					return false;
				}
				break;
			}
			case comp::lteq: {
				if (left.isNil() || right.isNil()) { push_error(errors::error{ errors::unknown ,"Attempt to compare a nil value!" }); return false; }
				if (!assign(assn, left <= right)) {
					return false;
				}
				break;
			}
			case comp::nteq: {
				if (!assign(assn, left != right)) {
					return false;
				}
				break;
			}
			}
		}
		break;
		case HALT:
			//wait();
			//sleep(700);
			std::cout << std::endl;
			//HandleHalt.fire(this); // We need a way to keep things running while also stopping the state from running. Shouldn't be too bad
			break;
		case WAIT:
			sleep((int)(n_c->args.args[0].n * 1000));
			break;
		case DSPD:
			if (speaker == nullptr) {
				push_error(errors::error{ errors::unknown ,utils::concat("A call to set speaker speed, but a speaker has not been defined!") });
				return false;
			}
			else {
				speaker->spd = n_c->args.args[0].n;
			}
			break;
		case SSPK:
			//Because we are using void* we must cast our pointers
			if (characterExists(n_c->args.args[0].getString())) {
				speaker = getCharacter(n_c->args.args[0].getString());
				if (!handler->handleSpeaker(this, speaker))
					return false;
			}
			else {
				push_error(errors::error{ errors::disp_unknown,concat("Unknown character '",n_c->args.args[0].getString(),"'!") });
				return false;
			}
			break;
		case APND:
			OnAppendText.fire(message{
				speaker,
				n_c->args.args[0].resolve(this).getPrintable()
			});
			//if (!handler->handleMessageAppend(this, c->args.args[0].resolve(this).getPrintable()))
			//	return false;
			break;
		case CHAR:
		{
			std::string cha = n_c->args.args[0].getString();
			getCharacter(cha);
			break;
		}
		case DISP:
		{
			OnText.fire(message{
				speaker,
				n_c->args.args[0].resolve(this).getPrintable()
			});
			/*if (!handler->handleMessageDisplay(this, c->args.args[0].resolve(this).getPrintable()))
				return false;*/
		}
		break;
		case ASGN:
			if (!assign(n_c->args.args[0], n_c->args.args[1])) {
				return false;
			}
			break;
		case LINE:
			cur_line = (size_t)n_c->args.args[0].i;
			break;
		case NOOP:
			break;
		case CHOI:
			//Because we are using void* we must cast our pointers
		{
			std::vector<std::string> args;
			std::string prompt = n_c->args.args[0].getString();
			std::string fn = n_c->args.args[1].getString();
			for (size_t i = 2; i < n_c->args.args.size(); i++)
				args.push_back(n_c->args.args[i].resolve(this).getString());
			size_t npos = handler->handleChoice(this, prompt, args);
			size_t nnpos = seek(concat("CHOI_", fn, "_", npos), n_cmds, LABL, n_pos);
			if (!nnpos) {
				push_error(errors::error{ errors::choice_unknown ,utils::concat("Unknown choice!") });
				return false;
			}
			else {
				n_pos = nnpos;
			}
		}
		break;
		case JUMP:
			// Value assert resolves the data so a variable must eventually equal a string
			if (utils::valueassert(n_c->args, this, datatypes::string)) {
				std::string block = n_c->args.args[0].resolve(this).getString();
				if (chunks[block] == NULL) {
					push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non existing block [",block,"]") });
					return false;
				}
				else if (chunks[block]->type != bt_block) {
					push_error(errors::error{ errors::non_existing_block ,utils::concat("Attempted to Jump to a non standard block [",block,"] type: ",getBlockType(chunks[block]->type)) });
					return false;
				}
				else {
					init(chunks[block], n_pos, n_max, n_cmds);
				}
			}
			else {
				//utils::print(c->args.args[0].type);
				datatypes set = n_c->args.args[0].resolve(this).type;
				//utils::print("> ",set);
				push_error(errors::error{ errors::invalid_arguments, utils::concat("String expected got ",datatype[set]), true, n_ln });
				return false;
			}
			break;
		default:
			break;
		}
		if (n_pos == n_max) {
			// How did we get here? The end of a block?
			return false;
		}
		return true;
	}
}