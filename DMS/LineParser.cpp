#include "LineParser.h"

using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	void tokenstream::init(std::vector<token>* ptr) {
		this->tokens = *ptr;
	}
	token tokenstream::next() {
		if (pos > this->tokens.size())
			return token{ tokentype::noop,codes::NOOP,"EOF",0 };
		return this->tokens[pos++];
	}
	token tokenstream::peek() {
		return this->tokens[pos];
	}
	std::vector<token> tokenstream::next(tokens::tokentype tk, size_t n) {
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
	void wait() {
		std::cin.ignore();
	}
	bool LineParser::match(tokenstream stream, tokens::tokentype t1, tokens::tokentype t2, tokens::tokentype t3, tokens::tokentype t4, tokens::tokentype t5, tokens::tokentype t6, tokens::tokentype t7, tokens::tokentype t8, tokens::tokentype t9, tokens::tokentype t10, tokens::tokentype t11, tokens::tokentype t12) {
		tokens::tokentype types[12] = { t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12 };
		for (size_t i = 0; i < 12; i++) {
			if (types[i] == tokens::none)
				return true;
			if (stream.tokens[stream.pos+i].type != types[i])
				return false;
			print(stream.tokens[stream.pos + i].type, " | ", types[i]);
		}
		return true;
	}
	std::map<std::string, chunk> LineParser::tokenizer(dms_state* state,std::vector<token> &toks) {
		std::map<std::string,chunk> chunks;
		chunk* current_chunk = nullptr;
		std::string chunk_name;
		blocktype chunk_type;

		tokenstream stream;
		stream.init(&toks);
		token current = stream.next();
		std::vector<token> temp;
		while (stream.peek().type != tokens::eof) {
			print(current);
			if (current.type == tokens::flag) {
				temp = stream.next(tokens::newline);
				if (temp.size() != 2) {
					std::cout << "";
				}
				codes::op code = current.raw;
				tokens::tokentype tok = temp[0].type;
				if (code == codes::ENAB && tok == tokens::name) {
					state->enables.insert_or_assign(temp[0].name,true);
				}
				else if (code == codes::ENTR && tok == tokens::name) {
					state->entry = temp[0].name;
				}
				else if (code == codes::DISA && tok == tokens::name) {
					state->enables.insert_or_assign(temp[0].name, false);
				}
				else if (code == codes::VERN && tok == tokens::number) {
					state->version = std::stod(temp[0].name);
				}
				else if (code == codes::USIN && tok == tokens::name) {
					// TODO add usings, kinda useless since everything will be packed in. Perhaps extensions might work
				}
				else if (code == codes::LOAD && tok == tokens::string) {
					Parse(state, temp[0].name); // Load another file
				}
				else {
					std::stringstream str;
					str << "Expected <FLAG IDENTIFIER> " << " got: " << current << " ";
					state->push_error(errors::error{errors::badtoken,str.str(),true,current.line_num});
				}
				std::cout << temp.size() << std::endl;
				std::cout << temp[0] << std::endl;
			}
			// To implement function we need to match stuff

			//Todo Finish the chunk data stuff
			if (match(stream,tokens::newline,tokens::bracketo,tokens::name,tokens::bracketc)) {
				stream.next();
				if (current_chunk != nullptr)
					chunks.insert_or_assign(current_chunk->name, *current_chunk);

				current_chunk = new chunk;
				stream.next(); // Consume
				current_chunk->name = stream.next().name;
				stream.next(); // Consume
			}
			else if (match(stream, tokens::newline, tokens::bracketo, tokens::name,tokens::colon,tokens::name, tokens::bracketc)) {
				stream.next();
				stream.next();
				if (current_chunk != nullptr)
					chunks.insert_or_assign(current_chunk->name, *current_chunk);

				current_chunk = new chunk;
				current_chunk->name = stream.next().name;
				stream.next();
				std::string temp = stream.next().name;
				if (temp == "char") {
					current_chunk->type = bt_character;
				}
				else if (temp == "env") {
					current_chunk->type = bt_env;
				}
				else if (temp == "menu") {
					current_chunk->type = bt_menu;
				}
				stream.next();
			}
			wait();
			current = stream.next();
		}
		return chunks;
	}
	void LineParser::tolower(std::string &s1) {
		std::transform(s1.begin(), s1.end(), s1.begin(), std::tolower);
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
		size_t line = 1;
		while (data != NULL) {			
			if (data == '/' && stream.peek()=='/') {
				line++;
				stream.next('\n'); // Seek until you find a newline
			} 
			else if (data == '\n') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line });
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
				t_vec.push_back(token{ tokens::colon,codes::NOOP,"",line });
			}
			else if (data == '!') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::Not,codes::NOOP,"",line });
			}
			else if (data == '\t') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::tab,codes::NOOP,"",line });
			}

			if (data == ' ' && !isStr) { // tokens end with a space
				std::string str = stream.processBuffer(buffer);
				tolower(str);
				buffer.clear();
				if (str == "enable") {
					t_vec.push_back(token{ tokens::flag,codes::ENAB,"",line });
				} else if (str == "entry") {
					t_vec.push_back(token{ tokens::flag,codes::ENTR,"",line });
				}
				else if (str == "loadfile") {
					t_vec.push_back(token{ tokens::flag,codes::LOAD,"",line });
				}
				else if (str == "version") {
					t_vec.push_back(token{ tokens::flag,codes::VERN,"",line });
				}
				else if (str == "using") {
					t_vec.push_back(token{ tokens::flag,codes::USIN,"",line });
				}
				else if (str == "disable") {
					t_vec.push_back(token{ tokens::flag,codes::DISA,"",line });
				}
				else if (str == "if") {
					t_vec.push_back(token{ tokens::control,codes::IFFF,"",line });
				}
				else if (str == "elseif") {
					t_vec.push_back(token{ tokens::control,codes::ELIF,"",line });
				}
				else if (str == "while") {
					t_vec.push_back(token{ tokens::control,codes::WHLE,"",line });
				}
				else if (str == "true") {
					t_vec.push_back(token{ tokens::True,codes::NOOP,"",line });
				}
				else if (str == "False") {
					t_vec.push_back(token{ tokens::False,codes::NOOP,"",line });
				}
				else if (str == "else") {
					t_vec.push_back(token{ tokens::control,codes::ELSE,"",line });
				}
				else if (str == "and") {
					t_vec.push_back(token{ tokens::And,codes::NOOP,"",line });
				}
				else if (str == "or") {
					t_vec.push_back(token{ tokens::Or,codes::NOOP,"",line });
				}
				else if (str == "for") {
					t_vec.push_back(token{ tokens::For,codes::NOOP,"",line });
				}
				else if (str == "choice") {
					t_vec.push_back(token{ tokens::control,codes::CHOI,"",line });
				}
				else if (utils::isalphanum(str) && str.size()>0) {
					t_vec.push_back(token{ tokens::name,codes::NOOP,str,line });
				}
				else {
					// Unknown command!
					/*tok.build(tokens::noop, codes::UNWN);
					tok.name = str;
					tok.line_num = line;*/
				}
			}
			data = stream.next();
		} 
		t_vec.push_back(token{ tokens::eof,codes::NOOP,"",line+1 });
		std::ofstream outputFile("dump.txt");
		outputFile << "Token Dump:" << std::endl;
		for (size_t i = 0; i < t_vec.size(); i++) {
			outputFile << t_vec[i] << std::endl;
		}
		outputFile.close();
		print("Running tokenizer");
		// Tokens build let's parse
		tokenizer(state, t_vec);
		return state;
	}
}