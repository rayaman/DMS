#include "LineParser.h"
namespace dms {
	void LineParser::buildGoto(std::string g, bool v) {
		cmd* c = new cmd;
		c->opcode = codes::GOTO;
		if (v) {
			c->args.push(buildVariable(g));
		}
		else {
			c->args.push(buildValue(g));
		}
		current_chunk->addCmd(c);
	}
	void LineParser::buildNoop() {
		cmd* c = new cmd;
		c->opcode = codes::NOOP;
		current_chunk->addCmd(c);
	}
	void LineParser::buildLabel(std::string l) {
		cmd* c = new cmd;
		c->opcode = codes::LABL;
		c->args.push(buildValue(l));
		current_chunk->addCmd(c);
		current_chunk->addLabel(l);
	}
}