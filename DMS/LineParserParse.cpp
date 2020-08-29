#include "LineParser.h"
#include "errors.h"
using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
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
		std::unordered_map<std::string, chunk> chunks;
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
			if (data == '/' && stream.peek() == '/') {
				//line++;
				stream.next('\n'); // Seek until you find a newline
			}
			else if (data == '\n') {
				doCheck(&stream, &t_vec, line-2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line - 2 });
				if (isNum) {
					t_vec.push_back(token{ tokens::number,codes::NOOP,stream.processBuffer(buffer),line - 2 });
					buffer.clear();
					isNum = false;
				}
				line++;
				data = ' ';
			}
			else if (data == '"' && !isStr) {
				isStr = true;
			}
			else if (data == ':' && stream.peek() == ':' && !labelStart) {
				labelStart = true;
				stream.next();
			}
			else if (data == ':' && stream.peek() == ':' && labelStart) {
				t_vec.push_back(token{ tokens::label,codes::NOOP,stream.processBuffer(buffer),line - 2 });
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
				t_vec.push_back(token{ tokens::string,codes::NOOP,stream.processBuffer(buffer),line - 2 });
				buffer.clear();
			}
			else if (isStr) {
				buffer.push_back(data);
			}
			else if (isdigit(data)) {
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
				t_vec.push_back(token{ tokens::number,codes::ERRO,"Malformed number!",line - 2 });
			}
			else if (data == '[') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketo,codes::NOOP,"",line - 2 });
			}
			else if (data == ']') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketc,codes::NOOP,"",line - 2 });
			}
			else if (data == '(') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parao,codes::NOOP,"",line - 2 });
			}
			else if (data == ')') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parac,codes::NOOP,"",line - 2 });
			}
			else if (data == ',') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::seperator,codes::NOOP,"",line - 2 });
			}
			else if (data == '.') {
				//doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::dot,codes::NOOP,"",line - 2 });
			}
			else if (data == '{') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketo,codes::NOOP,"",line - 2 });
			}
			else if (data == '}') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketc,codes::NOOP,"",line - 2 });
			}
			else if (data == '+') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::plus,codes::NOOP,"",line - 2 });
			}
			else if (data == '-') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::minus,codes::NOOP,"",line - 2 });
			}
			else if (data == '*') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::multiply,codes::NOOP,"",line - 2 });
			}
			else if (data == '/') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::divide,codes::NOOP,"",line - 2 });
			}
			else if (data == '^') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::pow,codes::NOOP,"",line - 2 });
			}
			else if (data == '%') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::mod,codes::NOOP,"",line - 2 });
			}
			else if (data == '=') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::equal,codes::NOOP,"",line - 2 });
			}
			else if (data == ':') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::colon,codes::NOOP,"",line - 2 });
			}
			else if (data == ';') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line - 2 });
			}
			else if (data == '!') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::Not,codes::NOOP,"",line - 2 });
			}
			else if (data == '\t') {
				doCheck(&stream, &t_vec, line - 2, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::tab,codes::NOOP,"",line - 2 });
			}

			if (data == ' ' && !isStr) { // tokens end with a space
				std::string str = stream.processBuffer(buffer);
				tolower(str);
				if (str == "enable") {
					t_vec.push_back(token{ tokens::flag,codes::ENAB,"",line - 2 });
				}
				else if (str == "entry") {
					t_vec.push_back(token{ tokens::flag,codes::ENTR,"",line - 2 });
				}
				else if (str == "loadfile") {
					t_vec.push_back(token{ tokens::flag,codes::LOAD,"",line - 2 });
				}
				else if (str == "version") {
					t_vec.push_back(token{ tokens::flag,codes::VERN,"",line - 2 });
				}
				else if (str == "using") {
					t_vec.push_back(token{ tokens::flag,codes::USIN,"",line - 2 });
				}
				else if (str == "disable") {
					t_vec.push_back(token{ tokens::flag,codes::DISA,"",line - 2 });
				}
				else if (str == "if") {
					t_vec.push_back(token{ tokens::control,codes::IFFF,"",line - 2 });
				}
				else if (str == "elseif") {
					t_vec.push_back(token{ tokens::control,codes::ELIF,"",line - 2 });
				}
				else if (str == "while") {
					t_vec.push_back(token{ tokens::control,codes::WHLE,"",line - 2 });
				}
				else if (str == "true") {
					t_vec.push_back(token{ tokens::True,codes::NOOP,"",line - 2 });
				}
				else if (str == "false") {
					t_vec.push_back(token{ tokens::False,codes::NOOP,"",line - 2 });
				}
				else if (str == "else") {
					t_vec.push_back(token{ tokens::control,codes::ELSE,"",line - 2 });
				}
				else if (str == "and") {
					t_vec.push_back(token{ tokens::And,codes::NOOP,"",line - 2 });
				}
				else if (str == "or") {
					t_vec.push_back(token{ tokens::Or,codes::NOOP,"",line - 2 });
				}
				else if (str == "for") {
					t_vec.push_back(token{ tokens::For,codes::NOOP,"",line - 2 });
				}
				else if (str == "choice") {
					t_vec.push_back(token{ tokens::control,codes::CHOI,"",line - 2 });
				}
				else if (str == "return") {
					t_vec.push_back(token{ tokens::ret,codes::RETN,"",line - 2 });
				}
				else if (str == "nil") {
					t_vec.push_back(token{ tokens::nil,codes::NOOP,"",line - 2 });
				}
				else if (str == "goto") {
					t_vec.push_back(token{ tokens::gotoo,codes::NOOP,"",line - 2 });
				}
				else if (str == "jump") {
					t_vec.push_back(token{ tokens::jump,codes::NOOP,"",line - 2 });
				}
				else if (str == "exit") {
					t_vec.push_back(token{ tokens::exit,codes::NOOP,"",line - 2 });
				}
				else if (utils::isNum(str) && str.size()!=0) {
					t_vec.push_back(token{ tokens::number,codes::NOOP,stream.processBuffer(buffer),line - 2 });
					isNum = false;
				}
				else if (utils::isalphanum(str) && str.size() > 0) {
					t_vec.push_back(token{ tokens::name,codes::NOOP,stream.processBuffer(buffer),line - 2 });
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
		t_vec.push_back(token{ tokens::eof,codes::NOOP,"",line - 1 });
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
					print("Loading File: ", temp[0].name);
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
				createBlock(name, bt_block);
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
				//token control = stream.next();
				if (match_process_choice(&stream)) {
					// Handle choice stuff
				}
				else if (match_process_IFFF(&stream)) {
					// This will probably be the toughest one of them all
				}
			}
			// Displays both with a target and without
			match_process_disp(&stream);

			if (current.type != tokens::tab)
				tabs = 0;
			current = stream.next();
		}
		state->chunks.insert_or_assign(current_chunk->name, current_chunk);
	}
}