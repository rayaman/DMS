#include "LineParser.h"
namespace dms {
	void LineParser::buildGoto(std::string g, bool v) {
		cmd* c = new cmd;
		c->opcode = codes::GOTO;
		if (v) {
			c->args.push(value(g,datatypes::variable));
		}
		else {
			c->args.push(value(g));
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
		c->args.push(value(l));
		current_chunk->addCmd(c);
		current_chunk->addLabel(l);
	}
	void LineParser::buildSpeed(double s) {
		cmd* c = new cmd;
		c->opcode = codes::DSPD;
		c->args.push(value(s));
		current_chunk->addCmd(c);
	}
	void LineParser::buildWait(double w) {
		cmd* c = new cmd;
		c->opcode = codes::WAIT;
		c->args.push(value(w));
		current_chunk->addCmd(c);
	}
}