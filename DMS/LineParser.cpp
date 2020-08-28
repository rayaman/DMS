#include "LineParser.h"
#include "errors.h"
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
	void tokenstream::prev() {
		pos--;
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
	bool LineParser::isBlock() {
		return isBlock(bt_block); // Default block type
	}
	bool LineParser::isBlock(blocktype bk_type) {
		if (current_chunk == nullptr) {
			return false; // If a chunk wasn't defined then code was probably defined outside of a block
		}
		return current_chunk->type == bk_type;
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
	bool LineParser::createBlock(std::string bk_name, blocktype bk_type) {
		if (current_chunk != nullptr) {
			if (!state->chunks.count(current_chunk->name))
				state->chunks.insert_or_assign(current_chunk->name, current_chunk);
			else
			{
				std::stringstream str;
				str << "Block <" << current_chunk->name << "> already defined!";
				state->push_error(errors::error{ errors::block_already_defined,str.str(),true,line });
				return false;
			}
		}
		current_chunk = new chunk;
		current_chunk->name = bk_name;
		chunk_type = bk_type;
		current_chunk->type = bk_type;
		print("Created Block: ",bk_name," <",bk_type,">");
		return true;
	}
	void LineParser::_Parse(tokenstream stream) {
		token current = stream.next();
		while (stream.peek().type != tokens::eof) {
			print(current);
			if (current.type == tokens::flag) {
				temp = stream.next(tokens::newline);
				stream.prev(); // Unconsume the newline piece
				if (temp.size() != 2) {
					std::cout << "Error";
				}
				codes::op code = current.raw;
				tokens::tokentype tok = temp[0].type;
				if (code == codes::ENAB && tok == tokens::name) {
					tolower(temp[0].name);
					state->enables.insert_or_assign(temp[0].name, true);
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
					// TODO add usings, kinda useless atm since everything will be packed in. Perhaps extensions?
				}
				else if (code == codes::LOAD && tok == tokens::string) {
					print("Loading File: ",temp[0].name);
					LineParser parser = LineParser();
					parser.Parse(state, temp[0].name);// Load another file
				}
				else {
					std::stringstream str;
					str << "Expected <FLAG IDENTIFIER> " << " got: " << current << temp[0];
					state->push_error(errors::error{ errors::badtoken,str.str(),true,line });
				}
			}
			// Default block
			if (stream.match(tokens::newline, tokens::bracketo, tokens::name, tokens::bracketc)) {
				stream.next();
				std::string name = stream.next().name;
				createBlock(name,bt_block);
				line = stream.next().line_num; // Consume
			}
			// This handles a few block types since they all follow a similar format
			else if (stream.match(tokens::newline, tokens::bracketo, tokens::name, tokens::colon, tokens::name, tokens::bracketc)) {
				stream.next();
				stream.next();
				std::string name = stream.next().name;
				line = stream.next().line_num;
				std::string temp = stream.next().name;
				// Characters are a feature I want to have intergrated into the language
				if (temp == "char") {
					createBlock(name, bt_character);
				}
				// Enviroments are sortof like objects, they can be uses as an object. They are a cleaner way to build a hash map like object
				else if (temp == "env") {
					createBlock(name, bt_env);
				}
				// Menus are what they say on the tin. They provide the framework for having menus within your game
				else if (temp == "menu") {
					createBlock(name, bt_menu);
				}
			}
			// Function block type
			else if (stream.match(tokens::newline, tokens::bracketo, tokens::name, tokens::colon, tokens::name, tokens::parao)) {
				std::stringstream str;
				stream.next();
				stream.next();
				std::string name = stream.next().name;
				line = stream.next().line_num; // The color, not needed after the inital match, but we still need to consume it
				std::string b = stream.next().name;
				if (b == "function") {
					createBlock(name, bt_method); // We have a method let's set the block type to that, but we aren't done yet
					// We need to set the params if any so the method can be supplied with arguments
					stream.next(); // parao
					std::vector<token> tokens = stream.next(tokens::parac); // Consume until we see parac
					dms_args args;
					for (size_t i = 0; i < tokens.size() - 1; i++) {//The lase symbol is parac since that was the consume condition
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
							state->push_error(errors::error{ errors::badtoken,str.str(),true,line });
						}
					}
					// If all went well the 'args' now has all of tha params for the method we will be working with
					current_chunk->params = args;
					// Thats should be all we need to do
				}
				else {
					str << "'function' keyword expected got " << b;
					state->push_error(errors::error{ errors::badtoken, str.str(),true,line });
				}
			}
			// Control Handle all controls here
			if (stream.match(tokens::control)) {
				token control = stream.next();
				if (control.raw == codes::CHOI && stream.peek().type == tokens::string) {
					// Let's parse choice blocks.
					std::string prompt = stream.next().name;
					print("Prompt: ", prompt);
					bool good = true;
					std::string option;
					cmd* c = new cmd;
					// Create a unique label name by using the line number
					std::string choicelabel = concat("$CHOI_END_", stream.peek().line_num);
					wait();
					c->opcode = codes::CHOI;
					c->args.push(buildValue(prompt));
					current_chunk->addCmd(c); // We will keep a reference to this and add to it as we go through the list

					/*
						What's going on here might be tough to understand just by looking at the code
						The bytecode generated by this code might look something like this:

						off	op		opcodes
						0	CHOI	"Pick one!" "Choice 1" "Choice 2" "Choice 3" "Choice 4"
						1	FUNC	print "You picked 1!"
						2	GOTO	$CHOI_END_1
						3	FUNC	print "You picked 2!"
						4	GOTO	$CHOI_END_1
						5	JUMP	park
						6	NOOP
						7	GOTO	mylabel
						8	LABL	$CHOI_END_1

						The CHOI code tells the vm that we need to process user input. The input we get in a number 0-3
						I know we have 4 choices
						If the user provides us with a 0 then we need to move to off 1
						If the user provides us with a 1 then we need to move to off 3
						If the user provides us with a 2 then we need to move to off 5
						If the user provides us with a 3 then we need to move to off 7
						I'm sure you see the pattern here. 1 (+2) 3 (+2) 5... We only need to jump once then let the vm continue like normal.
						The math for this is: [current_pos] + (n*2+1)
						n*2+1 (n = 0) = 1
						n*2+1 (n = 1) = 3
						n*2+1 (n = 2) = 5
						n*2+1 (n = 3) = 7
						Which is why you see NOOP for the JUMP code. If GOTO wasn't the last choice possible to make there would be a NOOP after that as well.
						The NOOP ensures the pattern stays.
						If we are provided with a number greater than 3 then we can push an execption.
					*/
					while (!stream.match(tokens::cbracketc)) {
						// We need to match the possible options for a choice block
						/*
							"option" function()
							"option" goto ""
							"option" goto var
							"option" jump ""
							"option" jump var
							"option" exit [0]
							
							Exit takes an optional int
						*/
						if (stream.match(tokens::string)) {
							std::string name = stream.next().name;
							c->args.push(buildValue(name)); // We append the choice to the first part of the CHOI cmd

							// We consumed the option now lets do some matching, note that all of these are one liners in the bytecode!
							if (match_process_function(&stream)) {

							}
							else if (match_process_goto(&stream)) {

							}
							else if (match_process_jump(&stream)) {

							}
							else if (match_process_exit(&stream)) {

							}
						}
						// Last Match
						else if (stream.match(tokens::newline)) {
							stream.next(); // Consume
						}
						else if (!stream.match(tokens::cbracketc)) {
							state->push_error(errors::error{errors::choice_unknown,"Unexpected symbol!"});
						}
					}
				}
				else if (control.raw == codes::IFFF) {
					// This will probably be the toughest one of them all
				}
			}
			// Displays both with a target and without
			match_process_disp(&stream);

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
				tabs = 0;
			current = stream.next();
		}
		state->chunks.insert_or_assign(current_chunk->name, current_chunk);
	}
	void LineParser::tokenizer(dms_state* state,std::vector<token> &toks) {
		tokenstream stream;
		stream.init(&toks);
		this->state = state; // Grab the pointer to the state and store it within the parser object
		_Parse(stream);
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
			rawdata << "\n\n"; // For things to work I added 2 newlines. The issue is with how I decided to parse things.
			// This way you are allowed to start a block at the top of the screen!
			while (std::getline(myfile, line)) {
				trim(line);
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
			else if (data == ';') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line });
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
		tokenizer(state, t_vec);
		return state;
	}
}