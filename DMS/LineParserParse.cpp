#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
bool is_file_exist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}
namespace dms {
	dms_state* dms::LineParser::Parse() {
		if (fn == "") {
			std::cout << "ERROR: You did not provide the constructor with a file path!" << std::endl;
			return nullptr;
		}
		return Parse(fn);
	}
	dms_state* dms::LineParser::Parse(std::string file) {
		std::remove("dump.txt");
		dms_state* state = new dms_state();
		return Parse(state, fn);
	}
	dms_state* dms::LineParser::Parse(dms_state* state, std::string file) {
		bool isFirst = false;
		if (!state->hasFirst) {
			isFirst = true;
			state->hasFirst = true;
		}
		std::vector<token> t_vec;
		std::string li;
		std::ifstream myfile(file);
		std::stringstream rawdata;
		// Read whole file into a string
		if (myfile.is_open())
		{
			std::string line;
			rawdata << ";;"; // For things to work I added 2 newlines. Using ';' doesn't change the actual line numbers
			// This way you are allowed to start a block at the top of the screen!
			while (std::getline(myfile, line)) {
				trim(line);
				rawdata << line << ";\n";
			}
			myfile.close();
			fn = file;
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
				std::string str = stream.processBuffer(buffer);
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line });
				if (isNum && str.size() != 0) {
					trim(str);
					if (str.size() != 0) {
						t_vec.push_back(token{ tokens::number,codes::NOOP,str,line });
						buffer.clear();
						isNum = false;
					}
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
				t_vec.push_back(token{ tokens::label,codes::NOOP,stream.processBuffer(buffer),line });
				buffer.clear();
				stream.next();
				labelStart = false;
			}
			else if (data == '\\' && stream.peek() == '"' && isStr) {
				buffer.push_back('"');
				stream.next();
			}
			else if (data == '\\' && stream.peek() == 'n' && isStr) {
				buffer.push_back('\n');
				stream.next();
			}
			else if (data == '"' && isStr) {
				isStr = false;
				t_vec.push_back(token{ tokens::string,codes::NOOP,stream.processBuffer(buffer),line });
				buffer.clear();
			}
			else if (isStr) {
				buffer.push_back(data);
			}
			else if (data == '_') { // Can't believe I missed that haha
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
				t_vec.push_back(token{ tokens::number,codes::ERRO,"Malformed number!",line });
			}
			else if (data == '[') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketo,codes::NOOP,"[",line });
			}
			else if (data == ']') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::bracketc,codes::NOOP,"]",line });
			}
			else if (data == '(') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parao,codes::NOOP,"(",line });
			}
			else if (data == ')') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::parac,codes::NOOP,")",line });
			}
			else if (data == ',') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::seperator,codes::NOOP,",",line });
			}
			else if (data == '.') {
				// If a number starts with a .## then we need to ensure that we create the value properly
				if (std::isdigit(stream.peek()) && buffer.size()==0) {
					// If the buffer has data then something isn't right
					isNum = true;
					buffer.push_back('0');
					buffer.push_back('.');
				}
				else {
					doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
					t_vec.push_back(token{ tokens::dot,codes::NOOP,".",line });
				}
			}
			else if (data == '{') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketo,codes::NOOP,"{",line });
			}
			else if (data == '}') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::cbracketc,codes::NOOP,"}",line });
			}
			else if (data == '+') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::plus,codes::NOOP,"+",line });
			}
			else if (data == '-') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::minus,codes::NOOP,"-",line });
			}
			else if (data == '*') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::multiply,codes::NOOP,"*",line });
			}
			else if (data == '/') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::divide,codes::NOOP,"/",line });
			}
			else if (data == '^') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::caret,codes::NOOP,"^",line });
			}
			else if (data == '%') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::percent,codes::NOOP,"%",line });
			}
			else if (data == '=') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::equal,codes::NOOP,"=",line });
			}
			else if (data == ':') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::colon,codes::NOOP,":",line });
			}
			else if (data == ';') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::newline,codes::NOOP,"",line });
			}
			else if (data == '!') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::exclamation,codes::NOOP,"!",line });
			}
			else if (data == '~') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::tilde,codes::NOOP,"~",line });
			}
			else if (data == '`') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::backtick,codes::NOOP,"`",line });
			}
			else if (data == '|') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::pipe,codes::NOOP,"|",line });
			}
			else if (data == '@') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::at,codes::NOOP,"@",line });
			}
			else if (data == '#') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::pound,codes::NOOP,"#",line });
			}
			else if (data == '$') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::dollar,codes::NOOP,"$",line });
			}
			else if (data == '&') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::ampersand,codes::NOOP,"&",line });
			}
			else if (data == '>') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::anglebracketC,codes::NOOP,">",line });
			}
			else if (data == '<') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::anglebracketO,codes::NOOP,"<",line });
			}
			else if (data == '\t') {
				doCheck(&stream, &t_vec, line, isNum, hasDec, &buffer);
				t_vec.push_back(token{ tokens::tab,codes::NOOP,"\t",line });
			}

			if ((data == ' ' || data == '(') && !isStr) { // tokens end with a space
				std::string str = stream.processBuffer(buffer);
				tolower(str);
				if (str == "enable") {
					t_vec.push_back(token{ tokens::flag,codes::ENAB,"",line });
				}
				else if (str == "entry") {
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
				else if (str == "true") {
					t_vec.push_back(token{ tokens::True,codes::NOOP,"",line });
				}
				else if (str == "false") {
					t_vec.push_back(token{ tokens::False,codes::NOOP,"",line });
				}
				else if (str == "and") {
					t_vec.push_back(token{ tokens::And,codes::NOOP,"",line });
				}
				else if (str == "or") {
					t_vec.push_back(token{ tokens::Or,codes::NOOP,"",line });
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
				else if (str == "exit") {
					t_vec.push_back(token{ tokens::exit,codes::NOOP,"",line });
				}
				else if (str == "debug") {
					t_vec.push_back(token{ tokens::debug,codes::NOOP,"",line });
				}
				else if (utils::isNum(str) && str.size()!=0) {
					trim(str);
					if(str!=""){
						t_vec.push_back(token{ tokens::number,codes::NOOP,stream.processBuffer(buffer),line });
						isNum = false;
					}
				}
				else if (utils::isalphanum(str) && str.size() > 0) {
					t_vec.push_back(token{ tokens::name,codes::NOOP,stream.processBuffer(buffer),line });
				}
				buffer.clear();
			}
			data = stream.next();
		}
		t_vec.push_back(token{ tokens::eof,codes::NOOP,"",line - 1 });
		tokenDump(&t_vec);
		// Tokens build let's parse
		tokenizer(state, t_vec);
		if (stop) {
			state->stop = true;
		}
		if (isFirst) {
			state->init();
		}
		return state;
	}
	void LineParser::tokenDump(std::vector<token>* v) {
		if (is_file_exist("dump.txt")) {
			std::ofstream outputFile;
			outputFile.open("dump.txt", std::ios_base::app); // append instead of overwrite
			for (size_t i = 0; i < v->size(); i++) {
				outputFile << (*v)[i] << std::endl;
			}
			outputFile.close();
		}
		else {
			std::ofstream outputFile("dump.txt");
			outputFile << "Token Dump:" << std::endl;
			for (size_t i = 0; i < v->size(); i++) {
				outputFile << (*v)[i] << std::endl;
			}
			outputFile.close();
		}
	}
	bool LineParser::ParseLoop(tokenstream* stream, size_t count) {
		if (stop) return false;
		size_t current_count=0;
		token current = token{tokentype::newline,codes::NOOP};
		cmd* flagcmd = new cmd;
		value nil;
		size_t last_pos = SIZE_MAX;
		while (stream->peek().type != tokens::none) {
			if (stop) return false;
			debugInvoker(stream);
			//utils::debug(current);
			//utils::print("[flags]");
			if (stream->match(flag)) {
				current = stream->next();
				temp = stream->next(tokens::newline);
				stream->prev(); // Unconsume the newline piece
				if (temp.size() != 2) {
					std::cout << "Error";
				}
				codes::op code = current.raw;
				tokens::tokentype tok = temp[0].type;
				if (code == codes::ENAB && tok == tokens::name) {
					tolower(temp[0].name);
					state->enable(temp[0].name);
					flagcmd->opcode = code;
					flagcmd->args.push(value(temp[0].name));
					current_chunk->addCmd(flagcmd);
					flagcmd = new cmd;
				}
				else if (code == codes::ENTR && tok == tokens::name) {
					if (state->entry != "$undefined") {
						state->push_error(errors::error{ errors::unknown ,utils::concat("Entrypoint already defined as '",state->entry,"'. Trying to redefine as '",temp[0].name,"' is not allowed!"), true,stream->last().line_num,current_chunk });
						return false;
					}
					state->entry = temp[0].name;
					flagcmd->opcode = code;
					flagcmd->args.push(value(temp[0].name));
					current_chunk->addCmd(flagcmd);
					flagcmd = new cmd;
				}
				else if (code == codes::DISA && tok == tokens::name) {
					tolower(temp[0].name);
					state->disable(temp[0].name);
					flagcmd->opcode = code;
					flagcmd->args.push(value(temp[0].name));
					current_chunk->addCmd(flagcmd);
					flagcmd = new cmd;
				}
				else if (code == codes::VERN && tok == tokens::number) {
					state->version = std::stod(temp[0].name);
					flagcmd->opcode = code;
					flagcmd->args.push(value(std::stod(temp[0].name)));
					current_chunk->addCmd(flagcmd);
					flagcmd = new cmd;
				}
				else if (code == codes::USIN && tok == tokens::name) {
					// TODO add usings, kinda useless atm since everything will be packed in. Perhaps extensions?
				}
				else if (code == codes::LOAD && tok == tokens::string) {
					/*flagcmd->opcode = code;
					flagcmd->args.push(buildValue(temp[0].name));
					current_chunk->addCmd(flagcmd);
					flagcmd = new cmd;*/
					LineParser parser = LineParser();
					parser.Parse(state, temp[0].name);// Load another file
				}
				else {
					state->push_error(errors::error{ errors::badtoken,concat("Expected <FLAG IDENTIFIER> got: ", current, temp[0]),true,line,current_chunk });
					return false;
				}
			}
			// Default block
			if (stream->match(tokens::bracketo, tokens::name, tokens::bracketc, tokens::newline)) {
				stream->next();
				std::string name = stream->next().name;
				createBlock(name, bt_block);
				line = stream->next().line_num; // Consume
			}
			// This handles a few block types since they all follow a similar format
			else if (stream->match(tokens::bracketo, tokens::name, tokens::colon, tokens::name, tokens::bracketc)) {
				stream->next();
				std::string name = stream->next().name;
				line = stream->next().line_num;
				std::string temp = stream->next().name;
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
				stream->next();
			}
			// Function block type
			else if (stream->match(tokens::bracketo, tokens::name, tokens::colon, tokens::name, tokens::parao)) {
				std::stringstream str;
				stream->next();
				std::string name = stream->next().name;
				line = stream->next().line_num; // The color, not needed after the inital match, but we still need to consume it
				std::string b = stream->next().name;
				if (b == "function") {
					createBlock(name, bt_method); // We have a method let's set the block type to that, but we aren't done yet
					// We need to set the params if any so the method can be supplied with arguments
					stream->next(); // parao
					std::vector<token> tokens = stream->next(tokens::parac); // Consume until we see parac
					dms_args args;
					for (size_t i = 0; i < tokens.size() - 1; i++) {//The lase symbol is parac since that was the consume condition
						if (tokens[i].type == tokens::name) {
							// We got a name which is refering to a variable so lets build one
							value v;
							v.type = datatypes::variable; // Special type, it writes data to the string portion, but is interperted as a lookup
							v.s = tokens[i].name;
							args.push(v);
						}
						else if (tokens[i].type == tokens::seperator) {
							// We just ignore this
						}
						else {
							std::stringstream str;
							str << "Unexpected symbol: " << tokens[i];
							state->push_error(errors::error{ errors::badtoken,str.str(),true,line,current_chunk });

							return false;
						}
					}
					// If all went well the 'args' now has all of tha params for the method we will be working with
					current_chunk->params = args;
					// Thats should be all we need to do
					if (stream->peek().type != tokens::bracketc) {
						state->push_error(errors::error{ errors::badtoken, "Incomplete function block declaration! Expected ']' to close the block!",true,line,current_chunk });
						return false;
					}
					else {
						stream->next();
					}
				}
				else {
					str << "'function' keyword expected got " << b;
					state->push_error(errors::error{ errors::badtoken, str.str(),true,line,current_chunk });

					return false;
				}
			}
			// Control Handle all controls here
			manageCount(match_process_choice(stream), count, current_count);
			manageCount(match_process_while(stream), count, current_count);
			manageCount(match_process_for(stream), count, current_count);
			manageCount(match_process_IFFF(stream), count, current_count);
			// Let's handle function stuff!
			//utils::print("[return]");
			manageCount(match_process_return(stream), count, current_count);
			//utils::print("[disp]");
			manageCount(match_process_disp(stream), count, current_count); // Match and process dialogue
			//utils::print("[label]");
			if (stream->match(tokens::newline, tokens::label)) { // Match and process labels
				stream->next();// We don't add this to the count, since a label is just a position to jump to!
				buildLabel(stream->next().name);
			}
			//utils::print("[func]");
			manageCount(match_process_function(stream, nil), count, current_count); // Naked Function
			//utils::print("[assn]");
			manageCount(match_process_assignment(stream), count, current_count);
			//utils::print("[debug]");
			manageCount(match_process_debug(stream), count, current_count);
			//utils::print("[goto]");
			manageCount(match_process_goto(stream), count, current_count);
			//utils::print("[exit]");
			manageCount(match_process_exit(stream), count, current_count);
			//utils::print("[wait]");
			manageCount(match_process_wait(stream), count, current_count);
			//utils::print("[jump]");
			manageCount(match_process_jump(stream), count, current_count);
			manageCount(match_process_1afunc(stream, nil),count,current_count);
			manageCount(match_process_asm(stream), count, current_count);
			if (count != 0 && current_count == count) {
				return true; // We got what we came for, we exit and consume no more!
			}
			if (last_pos == stream->pos) {
				badSymbol(stream);
			}
			last_pos = stream->pos;
			if (stream->match(newline) || stream->match(eof)) {
				current = stream->next();
			}
			
		}
	}
	void LineParser::_Parse(tokenstream* stream) {
		if (stop) return;
		createBlock("$INIT", bt_block);
		ParseLoop(stream);
		if (stop) return;
		createBlock("$END", bt_block);// Runs code that ensures that last user block is processed into the chunks array. Yes, I could have simply added in the lines of code at the end, but I didn't want to rewrite code again!
	}
}