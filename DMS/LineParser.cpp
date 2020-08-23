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
	token tokenstream::peek() {
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
	void doCheck(passer* stream,std::vector<token>* t_vec, size_t line, bool &isNum, bool &hasDec, std::vector<uint8_t>* buffer) {
		std::string str = stream->processBuffer(*buffer);
		if (utils::isNum(str) && isNum) {
			t_vec->push_back(token{ tokens::number,codes::NOOP,stream->processBuffer(*buffer),line });
			buffer->clear();
			isNum = false;
			hasDec = false;
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
	tokentype* LineParser::expr() {
		return new tokentype[9]{ tokens::name,tokens::number,tokens::divide,tokens::minus,tokens::mod,tokens::multiply,tokens::plus,tokens::pow ,tokens::none };
		// tokens::none tells us we are at the end of the array.
	}
	tokentype* LineParser::variable() {
		return new tokentype[7]{tokens::name,tokens::number,tokens::True,tokens::False,tokens::nil,tokens::string,tokens::none};
		// tokens::none tells us we are at the end of the array.
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
	
	// Handling functions are important. This method is a helper function that allows you to
	bool LineParser::processFunc(tokenstream stream, chunk c) {
		//
		return false;
	}
	bool processFunc(tokenstream stream, chunk c, std::string gotoo) {
		return true;
	}
	bool LineParser::processExpr(tokenstream stream, chunk c) {
		//
		return false;
	}
	bool LineParser::processLogic(tokenstream stream, chunk c) {
		//
		return false;
	}
	std::map<std::string, chunk*> LineParser::tokenizer(dms_state* state,std::vector<token> &toks) {
		std::map<std::string,chunk*> chunks;
		chunk* current_chunk = nullptr;
		std::string chunk_name;
		blocktype chunk_type = bt_block;
		std::stack<scope> scopes;
		size_t line=1;
		tokenstream stream;
		stream.init(&toks);
		token current = stream.next();
		std::vector<token> temp;
		size_t tabs = 0;
		while (stream.peek().type != tokens::eof) {
			print(current);
			if (current.type == tokens::tab)
				tabs++;
			if (current.type == tokens::flag) {
				temp = stream.next(tokens::newline);
				if (temp.size() != 2) {
					std::cout << "Error";
				}
				codes::op code = current.raw;
				tokens::tokentype tok = temp[0].type;
				if (code == codes::ENAB && tok == tokens::name) {
					tolower(temp[0].name);
					state->enables.insert_or_assign(temp[0].name,true);
				}
				else if (code == codes::ENTR && tok == tokens::name) {
					state->entry = temp[0].name;
				}
				else if (code == codes::DISA && tok == tokens::name) {
					tolower(temp[0].name);
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
					str << "Expected <FLAG IDENTIFIER> " << " got: " << current << temp[0];
					state->push_error(errors::error{errors::badtoken,str.str(),true,line});
				}
			}
			if (stream.match(tokens::newline,tokens::bracketo,tokens::name,tokens::bracketc)) {
				stream.next();
				if (current_chunk != nullptr) {
					if (!chunks.count(current_chunk->name))
						chunks.insert_or_assign(current_chunk->name, current_chunk);
					else
					{
						std::stringstream str;
						str << "Block <" << current_chunk->name << "> already defined!";
						state->push_error(errors::error{ errors::block_already_defined,str.str(),true,line });
					}
				}
				current_chunk = new chunk;
				chunk_type = bt_block;
				line = stream.next().line_num; // Consume
				current_chunk->name = stream.next().name;
				stream.next(); // Consume
			}
			// This handles a few block types since they all follow a similar format
			else if (stream.match(tokens::newline, tokens::bracketo, tokens::name,tokens::colon,tokens::name, tokens::bracketc)) {
				stream.next();
				stream.next();
				if (current_chunk != nullptr) {
					if (!chunks.count(current_chunk->name))
						chunks.insert_or_assign(current_chunk->name, current_chunk);
					else
					{
						std::stringstream str;
						str << "Block <" << current_chunk->name << "> already defined!";
						state->push_error(errors::error{ errors::block_already_defined,str.str(),true,line });
					}
				}

				current_chunk = new chunk;
				current_chunk->name = stream.next().name;
				line = stream.next().line_num;
				std::string temp = stream.next().name;
				// Characters are a feature I want to have intergrated into the language
				if (temp == "char") {
					current_chunk->type = bt_character;
					chunk_type = bt_character;
				}
				// Enviroments are sortof like objects, they can be uses as an object. They are a cleaner way to build a hash map like object
				else if (temp == "env") {
					current_chunk->type = bt_env;
					chunk_type = bt_env;
				}
				// Menus are what they say on the tin. They provide the framework for having menus within your game
				else if (temp == "menu") {
					current_chunk->type = bt_menu;
					chunk_type = bt_menu;
				}
				stream.next();
			}
			else if (stream.match(tokens::newline,tokens::bracketo,tokens::name,tokens::colon,tokens::name,tokens::parao)) {
				std::stringstream str;
				stream.next();
				stream.next();
				if (current_chunk != nullptr) {
					if (!chunks.count(current_chunk->name))
						chunks.insert_or_assign(current_chunk->name, current_chunk);
					else
					{
						std::stringstream str;
						str << "Block <" << current_chunk->name << "> already defined!";
						state->push_error(errors::error{ errors::block_already_defined,str.str(),true,line });
					}
				}

				current_chunk = new chunk;
				current_chunk->name = stream.next().name;
				line = stream.next().line_num; // The color, not needed after the inital match, but we still need to consume it
				std::string b = stream.next().name;
				if (b == "function") {
					current_chunk->type = bt_method; // We have a method let's set the block type to that, but we aren't done yet
					// We need to set the params if any so the method can be supplied with arguments
					stream.next(); // parao
					std::vector<token> tokens = stream.next(tokens::parac); // Consume until we see parac
					dms_args args;
					for (size_t i = 0; i < tokens.size()-1; i++) {//The lase symbol is parac since that was the consume condition
						if (tokens[i].type == tokens::name) {
							// We got a name which is refering to a variable so lets build one
							value* v = new value{};
							v->type = datatypes::variable; // Special type, it writes data to the string portion, but is interperted as a lookup
							v->s = buildString(tokens[i].name);
							args.push(v);
						}
						else if (tokens[i].type == tokens::seperator) {
							// We just ignore this
						}
						else {
							std::stringstream str;
							str << "Unexpected symbol: " << tokens[i];
							state->push_error(errors::error{errors::badtoken,str.str(),true,line });
						}
					}
					// If all went well the 'args' now has all of tha params for the method we will be working with
					current_chunk->params = args;
					// Thats should be all we need to do
				}
				else {
					str << "'function' keyword expected got " << b;
					state->push_error(errors::error{errors::badtoken, str.str(),true,line });
				}
			}
			// Control Handle all controls here
			if (stream.match(tokens::tab,tokens::control)) {
				stream.next(); // Standard consumption
				token control = stream.next();
				if (control.raw == codes::CHOI && stream.peek().type==tokens::string) {
					// Let's parse choice blocks.
					std::string prompt = stream.next().name;
					print("Prompt: ",prompt);
					bool good = true;
					size_t c = 0;
					while (good) {
						// We need to template the matches
						if (stream.match(tokens::tab,tokens::string,tokens::name,tokens::parao)) {
							stream.next();
							std::string prompt = stream.next().name;

							// To handle the function stuff we need to consume and process that data
							// We have a method to process function data since this will be used a lot in many different places
							// We just grabbed the prompt, we don't yet know how many choices we have. So we have to figure out how we can
							// Process and write the bytecode for this.
							std::string func = stream.next().name;
							print("Choice: <",c,"> ",prompt," Funcname: ",func);
							std::vector funcstuff = stream.next(tokens::newline);

							//We need to process the function data and finish creating 

							c++;
						}
						else if (stream.match(tokens::tab) || stream.match(tokens::newline)) {
							stream.next(); // Allow tabs and newlines to pass like normal
						}
						else {
							good = false;
							print("Choice handled!");
							wait();
						}
					}
				}
				else if (control.raw == codes::IFFF) {
					// This will probably be the toughest one of them all
				}
			}
			// Displays both with a target and without
			if (stream.match(tokens::tab, tokens::string, tokens::newline)) {
				// ToDo Implement the command for this
				stream.next(); // Standard consumption
				print("DISP := ", stream.next().name);
			}
			else if (stream.match(tokens::tab, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
				// We might have to handle scope here
				// Here we mathc Name "This guy said this!"
				stream.next(); // Standard consumption
				std::string name = stream.next().name;
				stream.next(); // That colon
				std::string msg = stream.next().name;
				print("DISP := ", name , " says '",msg,"'");
				// We might have to consume a newline... Depends on what's next
				if (stream.hasScope(tabs)) {
					// If true we might have a group of displaying stuff.
					// A proper scope will have the next line contain one more tab than the last

				}
			}
			// function stuff
			/*if (match(stream, tokens::name, tokens::parao)) {
				std::string n = stream.next().name;
				std::vector<token> t = stream.next(tokens::parao, tokens::parac);
				std::cout << "---Tokens---" << std::endl;
				for (size_t i = 0; i < t.size()-1; i++) {
					std::cout << t[i] << std::endl;
				}
				wait();
			}*/
			if (current.type != tokens::tab)
				tabs=0;
			current = stream.next();
		}
		chunks.insert_or_assign(current_chunk->name, current_chunk);
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
				//line++;
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
				print("> ",str);
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
				else if (str == "false") {
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
				else if (str == "return") {
					t_vec.push_back(token{ tokens::ret,codes::RETN,"",line });
				}
				else if (str == "nil") {
					t_vec.push_back(token{ tokens::nil,codes::NOOP,"",line });
				}
				else if (str == "goto") {
					t_vec.push_back(token{ tokens::gotoo,codes::NOOP,"",line });
				}
				else if (str == "jump") {
					t_vec.push_back(token{ tokens::jump,codes::NOOP,"",line });
				}
				else if (utils::isalphanum(str) && str.size()>0) {
					t_vec.push_back(token{ tokens::name,codes::NOOP,stream.processBuffer(buffer),line });
				}
				else {
					// Unknown command!
					/*tok.build(tokens::noop, codes::UNWN);
					tok.name = str;
					tok.line_num = line;*/
				}
				buffer.clear();
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
		std::map<std::string,chunk*> test = tokenizer(state, t_vec);
		print(test.size());
		return state;
	}
}