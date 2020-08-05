#include "LineParser.h"

using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	void doCheck(passer* stream,std::vector<token>* t_vec, size_t line, bool &isNum, bool &hasDec, std::vector<uint8_t>* buffer) {
		if (isNum) {
			t_vec->push_back(token{ tokens::number,codes::NOOP,stream->processBuffer(*buffer),line });
			buffer->clear();
			isNum = false;
			hasDec = false;
		}
		else if (buffer->size() > 0) {
			std::string str = stream->processBuffer(*buffer);
			if (utils::isNum(str) && str.size() > 0) {
				t_vec->push_back(token{ tokens::number,codes::NOOP,stream->processBuffer(*buffer),line });
				buffer->clear();
				isNum = false;
				hasDec = false;
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
	cmd* tokenizer(std::vector<token>* tok) {
		cmd* c = new cmd{};
		// turn toke data into
		return c;
	}
	LineParser::LineParser(std::string f) {
		fn = f;
	}
	LineParser::LineParser() {}
	dms_state* dms::LineParser::Parse() {
		if (fn == "") {
			std::cout << "ERROR: You did not provide the constructor with a file path!" << std::endl;
			return nullptr;
		}
		return Parse(fn);
	}
	dms_state* dms::LineParser::Parse(std::string file) {
		dms_state* state = new dms_state();
		return Parse(state, fn);
	}
	dms_state* dms::LineParser::Parse(dms_state* state, std::string file) {
		std::map<std::string, chunk> chunks;
		std::vector<token> t_vec;
		std::string li;
		std::ifstream myfile(file);
		std::stringstream rawdata;
		// Read whole file into a string
		if (myfile.is_open())
		{
			std::string line;
			while (std::getline(myfile, line)) {
				rawdata << line << "\n";
			}
			myfile.close();
			//std::cout << rawdata.str() << std::endl;
		}
		else {
			std::cout << "Unable to open file";
			delete[] state; // Cleanup
			return nullptr;
		}
		passer stream = passer();
		stream.stream = rawdata.str();
		uint8_t data = stream.next();
		std::vector<uint8_t> buffer;
		bool isStr = false;
		bool isNum = false;
		bool hasDec = false;
		bool labelStart = false;
		size_t line = 0;
		while (data != NULL) {			
			if (data == '/' && stream.peek()=='/') {
				line++;
				stream.next('\n'); // Seek until you find a newline
			} 
			else if (data == '\n') {
				if (isNum) {
					t_vec.push_back(token{ tokens::number,codes::NOOP,stream.processBuffer(buffer),line });
					buffer.clear();
					isNum = false;
				}
				line++;
				data = ' ';
			}
			else if (data == '"' && !isStr) {
				isStr = true;
			} 
			else if (data == ':' && stream.peek()==':' && !labelStart) {
				labelStart = true;
				stream.next();
			}
			else if (data == ':' && stream.peek() == ':' && labelStart) {
				t_vec.push_back(token{ tokens::label,codes::NOOP,stream.processBuffer(buffer),line });
				buffer.clear();
				stream.next();
				labelStart = false;
			}
			else if (data == '\\' && stream.peek() == '"' && isStr) {
				buffer.push_back('"');
				stream.next();
			}
			else if (data == '"' && isStr) {
				isStr = false;
				t_vec.push_back(token{ tokens::string,codes::NOOP,stream.processBuffer(buffer),line });
				buffer.clear();
			}
			else if (isdigit(data) ) {
				isNum = true;
				buffer.push_back(data);
			}
			else if (isalnum(data) || isStr) {
				buffer.push_back(data);
			}
			else if (data == '.' && isNum && !hasDec) {
				hasDec = true;
				buffer.push_back(data);
			}
			else if (data == '.' && isNum && hasDec) {
				t_vec.push_back(token{ tokens::number,codes::ERRO,"Malformed number!",line });
			}
			else if (data == '[') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketo,codes::NOOP,"",line });
			}
			else if (data == ']') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketc,codes::NOOP,"",line });
			}
			else if (data == '(') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parao,codes::NOOP,"",line });
			}
			else if (data == ')') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parac,codes::NOOP,"",line });
			}
			else if (data == ',') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::seperator,codes::NOOP,"",line });
			}
			else if (data == '.') {
				//doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::dot,codes::NOOP,"",line });
			}
			else if (data == '{') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketo,codes::NOOP,"",line });
			}
			else if (data == '}') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketc,codes::NOOP,"",line });
			}
			else if (data == '+') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::plus,codes::NOOP,"",line });
			}
			else if (data == '-') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::minus,codes::NOOP,"",line });
			}
			else if (data == '*') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::multiply,codes::NOOP,"",line });
			}
			else if (data == '/') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::divide,codes::NOOP,"",line });
			}
			else if (data == '^') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::pow,codes::NOOP,"",line });
			}
			else if (data == '%') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::mod,codes::NOOP,"",line });
			}
			else if (data == '=') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::equal,codes::NOOP,"",line });
			}
			else if (data == ':') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracket,codes::NOOP,"",line });
			}
			else if (data == '!') {
			doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
			t_vec.push_back(token{ tokens::Not,codes::NOOP,"",line });
			}

			if (data == ' ' && !isStr) { // tokens end with a space
				token tok;
				std::string str = stream.processBuffer(buffer);
				buffer.clear();
				if (str == "enable") {
					tok.build(tokens::flag, codes::ENAB);
				} else if (str == "entry") {
					tok.build(tokens::flag, codes::ENTR);
				}
				else if (str == "loadfile") {
					tok.build(tokens::flag, codes::LOAD);
				}
				else if (str == "version") {
					tok.build(tokens::flag,codes::VERN);
				}
				else if (str == "using") {
					tok.build(tokens::flag, codes::USIN);
				}
				else if (str == "if") {
					tok.build(tokens::control, codes::IFFF);
				}
				else if (str == "elseif") {
					tok.build(tokens::control, codes::ELIF);
				}
				else if (str == "while") {
					tok.build(tokens::control, codes::WHLE);
				}
				else if (str == "true") {
					tok.build(tokens::True, codes::NOOP);
				}
				else if (str == "False") {
					tok.build(tokens::False, codes::NOOP);
				}
				else if (str == "else") {
					tok.build(tokens::control, codes::ELSE);
				}
				else if (str == "and") {
					tok.build(tokens::And, codes::NOOP);
				}
				else if (str == "or") {
					tok.build(tokens::Or, codes::NOOP);
				}
				else if (str == "for") {
					tok.build(tokens::For, codes::NOOP);
				}
				else if (utils::isalphanum(str) && str.size()>0) {
					tok.build(tokens::name, str);
				}
				else {
					// Unknown command!
					tok.build(tokens::noop, codes::UNWN);
					tok.name = str;
				}
				if (tok.raw!=codes::UNWN && tok.type != tokens::noop) {
					tok.line_num = line;
					t_vec.push_back(tok);
				}
			}
			data = stream.next();
		} 
		std::cout << "Done!" << std::endl;
		for (size_t i = 0; i < t_vec.size(); i++) {
			std::cout << t_vec[i] << std::endl;
		}
		// Data is good now let's parse

		return state;
	}
}