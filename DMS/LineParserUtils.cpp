#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	void tokenstream::init(std::vector<token>* ptr) {
		this->tokens = *ptr;
	}
	token tokenstream::last() {
		return this->tokens[pos-1];
	}
	token tokenstream::next() {
		if (pos > this->tokens.size()-1)
			return token{ tokentype::none,codes::NOOP,"EOS",0 };
		return this->tokens[pos++];
	}
	void tokenstream::prev() {
		pos--;
	}
	void tokenstream::store(chunk* c) {
		stack.push(c->cmds.size());
		spos.push(pos);
	}
	bool tokenstream::restore(cmd* c, chunk* cnk) {
		pos = spos.top();
		spos.pop();
		size_t temp = stack.top(); 
		stack.pop();
		if (temp == cnk->cmds.size())
			return false;

		cnk->cmds.erase(cnk->cmds.begin()+temp, cnk->cmds.end());
		return false;
	}
	std::vector<token> tokenstream::next(tokentype to, tokentype tc) {
		std::vector<token> tok;
		size_t open = 0;
		if (peek().type == to) {
			open++;
			next(); // Consume
			while (open != 0) {
				if (peek().type == to)
					open++;
				else if (peek().type == tc)
					open--;
				tok.push_back(next());
			}
		}
		return tok;
	}
	std::vector<token> tokenstream::next(tokentype to, tokentype tc, bool nonewline) {
		std::vector<token> tok;
		size_t open = 0;
		if (peek().type == to) {
			open++;
			next(); // Consume
			while (open != 0) {
				if (peek().type == to)
					open++;
				else if (peek().type == tc)
					open--;
				else if (peek().type == tokens::newline)
					return std::vector<token>();
				tok.push_back(next());
			}
		}
		return tok;
	}
	token tokenstream::peek() {
		if (pos > this->tokens.size()-1)
			return token{ tokentype::none,codes::NOOP,"EOS",0 };
		return this->tokens[pos];
	}
	bool tokenstream::hasScope(size_t tabs) {
		return false;
	}
	std::vector<token> tokenstream::next(tokentype tk) {
		std::vector<token> temp;
		while (peek().type!=tk) {
			temp.push_back(next());
		}
		temp.push_back(next());
		return temp;
	}
	uint8_t passer::next() {
		if (stream.size() == pos) {
			return NULL;
		}
		else {
			return stream[pos++];
		}
	}
	void passer::next(uint8_t c) {
		next();
		while (peek() != c) {
			next();
		}
	}
	uint8_t passer::prev() {
		if (0 == pos) {
			return NULL;
		}
		return stream[--pos];
	}
	uint8_t passer::peek() {
		if (stream.size() == pos) {
			return NULL;
		}
		return stream[pos];
	}
	std::string passer::processBuffer(std::vector<uint8_t> buf) {
		return std::string(buf.begin(), buf.end());
	}

	bool LineParser::isBlock() {
		return isBlock(bt_block); // Default block type
	}
	bool LineParser::isBlock(blocktype bk_type) {
		if (current_chunk == nullptr) {
			return false; // If a chunk wasn't defined then code was probably defined outside of a block
		}
		return current_chunk->type == bk_type;
	}
	bool tokenstream::match(tokens::tokentype t1, tokens::tokentype t2, tokens::tokentype t3, tokens::tokentype t4, tokens::tokentype t5, tokens::tokentype t6, tokens::tokentype t7, tokens::tokentype t8, tokens::tokentype t9, tokens::tokentype t10, tokens::tokentype t11, tokens::tokentype t12) {
		tokens::tokentype types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		for (size_t i = 0; i < 12; i++) {
			if (types[i] == tokens::none)
				return true;
			if (this->tokens[pos+i].type != types[i])
				return false;
		}
		return true;
	}
	bool inList(tokens::tokentype t,tokens::tokentype* list) {
		size_t c = 0;
		while (list[c] != tokens::none) {
			if (list[c] == t)
				return true;
		}
		return false;
	}
	bool tokenstream::match(tokens::tokentype* t1, tokens::tokentype* t2, tokens::tokentype* t3, tokens::tokentype* t4, tokens::tokentype* t5, tokens::tokentype* t6, tokens::tokentype* t7, tokens::tokentype* t8, tokens::tokentype* t9, tokens::tokentype* t10, tokens::tokentype* t11, tokens::tokentype* t12) {
		tokens::tokentype* types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		for (size_t i = 0; i < 12; i++) {
			if (types[i] == nullptr)
				return true;
			if (!inList(this->tokens[pos + i].type, types[i]))
				return false;
		}
		return true;
	}
	void LineParser::doCheck(passer* stream, std::vector<token>* t_vec, size_t line, bool& isNum, bool& hasDec, std::vector<uint8_t>* buffer) {
		std::string str = stream->processBuffer(*buffer);
		if (utils::isNum(str) && isNum) {
			trim(str);
			if (str.size() != 0) {
				t_vec->push_back(token{ tokens::number,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				isNum = false;
				hasDec = false;
			}
		}
		else if (buffer->size() > 0) {
			if (str == "nil") {
				t_vec->push_back(token{ tokens::nil,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				hasDec = false;
			}
			else if (str == "true") {
				t_vec->push_back(token{ tokens::True,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				hasDec = false;
			}
			else if (str == "false") {
				t_vec->push_back(token{ tokens::False,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				hasDec = false;
			}
			else if (utils::isNum(str) && str.size() > 0) {
				trim(str);
				if (str.size() != 0) {
					t_vec->push_back(token{ tokens::number,codes::NOOP,stream->processBuffer(*buffer),line });
					buffer->clear();
					isNum = false;
					hasDec = false;
				}
			}
			else if (utils::isalphanum(str) && str.size() > 0) {
				t_vec->push_back(token{ tokens::name,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				hasDec = false;
			}
			else {
				t_vec->push_back(token{ tokens::name,codes::ERRO,"Invalid variable name!",line });
			}
		}
	}
	void LineParser::badSymbol(errors::errortype err,tokenstream* stream) {
		state->push_error(errors::error{ err,concat("Unexpected symbol '",stream->next().toString(),"'"),true,stream->peek().line_num,current_chunk });
	}
	void LineParser::badSymbol(tokenstream* stream) {
		state->push_error(errors::error{ errors::unknown,concat("Unexpected symbol '",stream->peek().toString(),"'"),true,stream->next().line_num,current_chunk });
	}
	void LineParser::badSymbol() {
		state->push_error(errors::error{ errors::unknown,concat("Unexpected symbol '",_stream->peek().toString(),"'"),true,_stream->next().line_num,current_chunk });
	}
	void LineParser::debugInvoker(tokenstream* stream) {
		if (state->isEnabled("debugging") && stream->peek().type != tokens::newline) {
			// A very nasty if statement, I won't hide it, this could be made much more readable
			// This checks if the last cmd is a LINE cmd and if its the same line number as the current one we simply skip it
			if (current_chunk->cmds.size() >= 2 && current_chunk->cmds[current_chunk->cmds.size() - 1]!=nullptr && current_chunk->cmds[current_chunk->cmds.size() - 1]->opcode==codes::LINE && (size_t)current_chunk->cmds[current_chunk->cmds.size()-1]->args.args[0].n== (size_t)stream->peek().line_num) {
				return;
			}
			int current_line = stream->peek().line_num;
			cmd* ln = new cmd;
			ln->opcode = codes::LINE;
			ln->args.push(value(current_line));
			current_chunk->addCmd(ln);
		}
	}
	bool LineParser::createBlock(std::string bk_name, blocktype bk_type) {
		if (current_chunk != nullptr || (bk_name == "$INIT" && state->chunks.count("$INIT"))) {
			if (state->chunks.count(bk_name)==0 && bk_name!="$END")
				state->push_chunk(current_chunk->name, current_chunk);
			else
			{
				if (bk_name == "$INIT") {
					current_chunk = state->chunks["$INIT"];
					return true;
				}
				else if (bk_name == "$END") {
					/*cmd* c = new cmd;
					c->opcode = codes::JUMP;
					c->args.push(buildValue("$END"));
					current_chunk->addCmd(c);
					state->push_chunk(current_chunk->name, current_chunk);
					current_chunk = state->chunks["$END"];*/
					return true;
				}
				else {
					std::stringstream str;
					str << "Block <" << current_chunk->name << "> already defined!";
					state->push_error(errors::error{ errors::block_already_defined,str.str(),true,line,current_chunk });
					return false;
				}
			}
		}
		if (state->isEnabled("leaking") && (current_chunk != nullptr && current_chunk->name != "$INIT")) {
			cmd* c = new cmd;
			c->opcode = codes::JUMP;
			c->args.push(value(bk_name));
			current_chunk->addCmd(c);
		}
		if (current_chunk != nullptr && current_chunk->name == "$END") {
			cmd* c = new cmd;
			c->opcode = codes::EXIT;
			if (state->entry != "$undefined")
				c->args.push(value(0));
			else
				c->args.push(value(bk_name));
			current_chunk->addCmd(c);
		}
		current_chunk = new chunk;
		current_chunk->name = bk_name;
		chunk_type = bk_type;
		current_chunk->type = bk_type;
		state->push_chunk(bk_name, current_chunk);
		return true;
	}
	void LineParser::tokenizer(dms_state* state,std::vector<token> &toks) {
		tokenstream stream;
		stream.init(&toks);
		_stream = &stream;
		this->state = state; // Grab the pointer to the state and store it within the parser object
		_Parse(&stream);
	}
	void LineParser::tolower(std::string &s1) {
		std::transform(s1.begin(), s1.end(), s1.begin(), std::tolower);
	}
	LineParser::LineParser(std::string f) {
		fn = f;
	}
	LineParser::LineParser() {}
}