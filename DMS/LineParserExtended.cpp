#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	bool LineParser::match_process_disp(tokenstream* stream) {
		/*
			DISP, "msg"
			DISP, "msg" speaker

			Compound DISP
		*/
		if (isBlock(bt_block) && stream->match(tokens::newline, tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			std::string msg = stream->next().name;
			print("DISP := ", msg);
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(buildValue(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			return true;
		}
		else if (isBlock(bt_block) && stream->match(tokens::newline, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			// We might have to handle scope here
			// Here we match 'Ryan: "This guy said this!"' Note the colon is needed!
			stream->next(); // Standard consumption
			std::string name = stream->next().name;
			stream->next(); // That colon
			std::string msg = stream->next().name;
			print("DISP := ", name, " says '", msg, "'");
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(buildValue(msg));
			c->args.push(buildValue(name));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			// We might have to consume a newline... Depends on what's next
			return true;
		}
		// emotion: "path"
		// looks like a simple disp command
		else if (isBlock(bt_character) && stream->match(tokens::tab, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			return true;
		}
		return false;
	}
	bool LineParser::match_process_debug(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_choice(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_function(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_goto(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_jump(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_exit(tokenstream* stream) {
		return false;
	}
}