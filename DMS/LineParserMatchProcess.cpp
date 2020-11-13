#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
// TODO: process if elseif else statements, for loops and while loops
namespace dms {
	bool LineParser::match_process_standard(tokenstream* stream, value& v) {
		if (match_process_expression(stream, v)) {
			return true;
		}
		else if (match_process_function(stream, v)) {
			return true;
		}
		else if (match_process_list(stream, v)) {
			return true;
		}
		else if (match_process_index(stream, v)) {
			return true;
		}
		else if (stream->match(tokens::True)) {
			v.set(buildBool(true));
			stream->next();
			return true;
		}
		else if (stream->match(tokens::False)) {
			v.set(buildBool(false));
			stream->next();
			return true;
		}
		else if (stream->match(tokens::number)) {
			v.set(std::stod(stream->next().name));
			return true;
		}
		else if (stream->match(tokens::string)) {
			v.set(buildString(stream->next().name));
			return true;
		}
		else if (stream->match(tokens::name)) {
			v.set(buildString(stream->next().name));
			v.type = datatypes::variable;
			return true;
		}
		else if (stream->match(tokens::nil)) {
			stream->next();
			v.set();
			return true;
		}
		else if (stream->match(tokens::bracketo, tokens::name, tokens::bracketc)) {
			// We are assigning a block as a variable
			stream->next();
			v.set(buildString(stream->next().name));
			v.type = datatypes::block;
			stream->next();
			return true;
		}
		else if (stream->match(tokens::newline)) {
			return false;
		}
		return false;
	}
	bool LineParser::match_process_list(tokenstream* stream, value& v) {
		if (stream->match(tokens::cbracketo)) {
			token start = stream->peek();
			token ancor = start;
			std::vector<token> t = stream->next(tokens::cbracketo, tokens::cbracketc);
			tokenstream tempstream;
			tempstream.init(&t);
			value ref = value(datatypes::variable);
			value length = value();
			value dict = value();
			size_t count = 0;
			cmd* c = new cmd;
			c->opcode = codes::LIST;
			c->args.push(v);
			c->args.push(length);
			current_chunk->addCmd(c);
			bool ready = true;
			while (tempstream.peek().type != tokens::none) {
				debugInvoker(stream);
				if (tempstream.match(tokens::cbracketc)) {
					ready = true;
					c = new cmd;
					c->opcode = codes::INST;
					c->args.push(v);
					c->args.push(ref);
					if (dict != nullptr) {
						c->args.push(dict);
						dict = nullptr;
					}
					current_chunk->addCmd(c);
					break;
				}
				// Match Dict
				else if (tempstream.match(tokens::name,tokens::colon)) {
					dict = value(tempstream.next().name,datatypes::variable);
					tempstream.next();
					if (!match_process_standard(&tempstream,ref)) {
						badSymbol();
					}
				}
				// This will modify the ref!!!
				else if (match_process_standard(&tempstream, ref)) {
					count++;
					if (ready) {
						ancor = tempstream.last();
						ready = false;
					}
					else
						state->push_error(errors::error{ errors::badtoken,concat("Unexpected symbol '",ancor.toString(),"' Expected '}' to close list (line: ",start.line_num,") Did you forget a comma?"),true,ancor.line_num,current_chunk });
				}
				else if (tempstream.match(tokens::newline)) {
					tempstream.next(); // this is fine
				}
				else if (tempstream.match(tokens::seperator)) {
					// Handle the next piece
					ready = true;
					c = new cmd;
					c->opcode = codes::INST;
					c->args.push(v);
					c->args.push(ref);
					if (dict != nullptr) {
						c->args.push(dict);
						dict = nullptr;
					}
					current_chunk->addCmd(c);
					// Build new value
					ref = value(datatypes::variable);
					tempstream.next();
				}
				else if (tempstream.match(tokens::cbracketo)) {
					tempstream.next();
				}
				else {
					badSymbol(&tempstream);
				}
			}
			length.set((double)count); // the second argument is the length of the list! This should be modified if lists are changed at runtime!
			c = new cmd;
			c->opcode = codes::INST;
			c->args.push(v);
			c->args.push(value());
			if (dict != nullptr) {
				c->args.push(dict);
				dict = nullptr;
			}
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_disp(tokenstream* stream) {
		if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline, tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(value(stream->next().name));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			current_chunk->addCmd(new cmd{ codes::HALT });
			return true;
		}
		else if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline,tokens::pipe ,tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			stream->next(); // pipe consumption
			if (current_chunk->cmds.size() && current_chunk->cmds.back()->opcode == codes::HALT) {
				current_chunk->cmds.pop_back();
			}
			std::string msg = stream->next().name;
			cmd* c = new cmd;
			c->opcode = codes::APND;
			c->args.push(value(concat(" ", msg)));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			current_chunk->addCmd(new cmd{ codes::HALT });
			return true;
		}
		else if (isBlock(bt_character) && stream->match(tokens::newline, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			std::string name = stream->next().name;
			stream->next(); // That colon
			std::string msg = stream->next().name;
			cmd* c = new cmd;
			c->opcode = codes::STAT;
			c->args.push(value(name,datatypes::variable));
			c->args.push(value(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
		}
		else if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			// We might have to handle scope here
			// Here we match 'Ryan: "This guy said this!"' Note the colon is needed!
			stream->next(); // Standard consumption
			std::string name = stream->next().name;
			stream->next(); // That colon
			std::string msg = stream->next().name;
			cmd* c = new cmd;
			c->opcode = codes::SSPK;
			c->args.push(value(name, datatypes::variable));
			current_chunk->addCmd(c);
			c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(value(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			current_chunk->addCmd(new cmd{ codes::HALT });
			return true;
		}
		else if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::name,tokens::colon,tokens::cbracketo)) {
			std::string name = stream->next().name;
			// Command to set the speaker
			cmd* c = new cmd;
			c->opcode = codes::SSPK;
			c->args.push(value(name,datatypes::variable));
			current_chunk->addCmd(c);
			stream->next();
			stream->next();
			while (stream->peek().type != tokens::cbracketc) {
				debugInvoker(stream);
				if (stream->match(tokens::name)) {
					std::string mode = stream->next().name;
					if (mode == "speed") {
						if (stream->match(tokens::number)) {
							buildSpeed(std::stod(stream->next().name));
						}
						else {
							badSymbol(stream);
							return false;
						}
					}
					else if (mode == "wait") {
						if (stream->match(tokens::number)) {
							buildWait(std::stod(stream->next().name));
						}
						else {
							badSymbol(errors::disp_unknown, stream);
						}
					}
					else {
						// Assume we have a dact
						if (stream->match(tokens::string) || stream->match(tokens::colon,tokens::string)) {
							if (stream->match(tokens::colon)) {
								stream->next();
							}
							cmd* c = new cmd;
							c->opcode = codes::DACT;
							c->args.push(value(mode));
							current_chunk->addCmd(c);
							// Now build the apnd msg cmd
							c = new cmd;
							c->opcode = codes::APND;
							c->args.push(value(stream->next().name));
							current_chunk->addCmd(c);
						}
						else {
							badSymbol(stream);
							return false;
						}
					}
				}
				else if (stream->match(tokens::string)) {
					cmd* c = new cmd;
					c->opcode = codes::APND;
					c->args.push(value(stream->next().name));
					current_chunk->addCmd(c);
				}
				else if (stream->match(tokens::newline)) {
					stream->next();
				}
				else {
					badSymbol(stream);
					return false;
				}
			}
			stream->next();
			current_chunk->addCmd(new cmd{ codes::HALT });
			return true;
		}
		return false;
	}
	bool LineParser::match_process_return(tokenstream* stream) {
		// Only handle this inside of a function block!
		if (current_chunk->type == blocktype::bt_method) {
			if (stream->match(tokens::ret)) {
				cmd* c = new cmd;
				c->opcode = codes::RETN;
				value ref = value(datatypes::variable);
				stream->next();
				if (match_process_standard(stream, ref)) {
					c->args.push(ref);
					current_chunk->addCmd(c);
					return true;
				}
				else {
					badSymbol();
					return false;
				}
			}
		}
		else if(stream->match(tokens::ret)) {
			stream->next();
			badSymbol();
		}
		return false;
	}
	bool LineParser::match_process_assignment(tokenstream* stream) {
		value v = value();
		if (match_process_index(stream, v, true)) {
			cmd* c = current_chunk->cmds.back();
			value ref = value(datatypes::variable);
			if (stream->peek().type == tokens::equal) {
				stream->next();
			}
			else {
				badSymbol(stream);
				return false;
			}
			if (match_process_standard(stream, ref)) {
				c->args.push(ref);
				return true;
			}
			else if (stream->match(tokens::newline)) {
				stream->next();
			}
		}
		else if (stream->match(tokens::name,tokens::equal)) {
			value var = value(stream->next().name,datatypes::variable); // The variable that we will be setting stuff to
			stream->next(); // Consume the equal
			cmd* c = new cmd;
			c->opcode = codes::ASGN;
			c->args.push(var);
			value ref = value(datatypes::variable);
			if (match_process_standard(stream,ref)) {
				c->args.push(ref);
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::newline)) {
				stream->next();
			}
		}
		return false;
	}
	bool LineParser::match_process_debug(tokenstream* stream) {
		if (stream->  match(tokens::newline, tokens::debug, tokens::string) || stream->match(tokens::newline, tokens::debug, tokens::name)){
			stream->next();
			stream->next();
			if (state->isEnabled("debugging")) {
				cmd* c = new cmd;
				c->opcode = codes::DEBG;
				if (stream->match(tokens::string)) {
					c->args.push(value(stream->next().name));
				}
				else {
					c->args.push(value(stream->next().name,datatypes::variable));
				}
				current_chunk->addCmd(c);
				return true;
			}
			else {
				stream->next(); // Consume the third element anyway
				return true; // This is a debugging match, but debugging is disabled. It's good!
			}
		}
		return false;
	}
	bool LineParser::match_process_choice(tokenstream* stream) {
		token temp = stream->peek();
		if (temp.raw == codes::CHOI && stream->match(tokens::control,tokens::string)) {
			// Let's parse choice blocks.
			stream->next();
			std::string prompt = stream->next().name;
			bool good = true;
			std::string option;
			cmd* c = new cmd;
			// Create a unique label name by using the line number
			int CHOI_ID = 0;
			std::string choi_str = concat("CHOI_", fn);
			std::string choicelabel = concat("$CHOI_END_", choi_str,"_", stream->peek().line_num);
			c->opcode = codes::CHOI;
			c->args.push(prompt);
			c->args.push(fn);
			current_chunk->addCmd(c); // We will keep a reference to this and add to it as we go through the list
			bool start = false;
			bool hasfunc = false;
			/*
				What's going on here might be tough to understand just by looking at the code
				The bytecode generated by this code might look something like this:

				off	op		opcodes
				0	CHOI	"Pick one!" CHOI_filename_index "Choice 1" "Choice 2" "Choice 3" "Choice 4"
				1	LABL	CHOI_filename_1
				2	FUNC	print "You picked 1!"
				3	GOTO	$CHOI_END_1
				4	LABL	CHOI_filename_2
				5	FUNC	print "You picked 2!"
				6	GOTO	$CHOI_END_1
				7	LABL	CHOI_filename_3
				8	JUMP	park
				9	LABL	CHOI_filename_4
				10	GOTO	mylabel
				11	LABL	$CHOI_END_1
			*/
			while (!stream->match(tokens::cbracketc)) {
				debugInvoker(stream);
				if (start && !stream->match(tokens::newline)) {
					buildLabel(concat(choi_str,"_", CHOI_ID++));
				} 
				if (stream->match(tokens::cbracketo) && !start) {
					start = true;
					stream->next();
				}
				else if (stream->match(tokens::cbracketo) && start) {
					badSymbol(stream);
				}
				else if (stream->match(tokens::string)) {
					std::string name = stream->next().name;
					c->args.push(value(name)); // We append the choice to the first part of the CHOI cmd

					value val = value();
					if (match_process_function(stream,val,false)) { // No returns and also no nesting of functions!
						// We cannot have a nested function here, but if we dont have that then we add our goto
						hasfunc = true;
						buildGoto(choicelabel);
					}
					else if (match_process_goto(stream)) {}
					else if (match_process_jump(stream)) {}
					else if (match_process_exit(stream)) {}
				}
				// Last Match
				else if (stream->match(tokens::newline)) {
					stream->next(); // Consume
				}
				else {
					badSymbol(stream);
				}
			}
			buildGoto(choicelabel);
			cmd* cc = current_chunk->cmds.back(); // Delete last element
			current_chunk->cmds.pop_back();
			delete cc;
			if (hasfunc)
				buildLabel(choicelabel);
			return true;
		}
		return false;
	}

	void cleanup(value* v) {
		v->nuke(); // Make sure we clean up the data
		delete v; // We didn't need it, lets clean it up!
	}
	bool LineParser::match_process_function(tokenstream* stream, value& v, bool nested) {
		/*
			Functions should be able to handle function calls as arguments, 
			HOWEVER functions cannot be passed as values since they aren't values like they are in other languages!

			The bytecode here is a little tricky, a bit of moving parts and work that the vm has to do.

			If a function has a return that is expected to be consumed, v will not be nullptr and will be pointing to something
			The type of v should be variable and if not throw an error!

			The bytecode should look something like this for a given method: 
				testfunc("Hello!")
			off	op		opcode
			0	FUNC	testfunc <nil> "Hello"

			The bytecode should look something like this for a given method: 
				val = testfunc2(2,2)
			off	op		opcode
			0	FUNC	testfunc2 val 2 2

			Notice how we have nil for the first function with no return, this tells the interperter that we do not need to store a value at all
			The second method has a varaible to store val so we store that in val

			Given a function: 
				val = test3(testfunc2(4,4),"Test function as an argument!")
			off	op		opcode
			0	FUNC	testfunc2 $A 4 4
			1	FUNC	test3 val $A "Test function as an argument!"

			Not too scary, in fact it's rathar stright forward
			Last example:
				test4("Testing",test3(1,testfunc2(1,2)),testfunc2(10,100))
			off	op		opcode
			0	FUNC	testfunc2 $A 1 2
			1	FUNC	test3 $B 1 $A
			2	FUNC	testfunc2 $C 10 100
			3	FUNC	test4 nil "Testing" $B $C

			A bit more involved, but I'm sure it wasn't too much to follow, especially if you looked at the examples in order

			That's all there is to functions within the bytecode side of things, the vm is where things are a little more involved
		*/
		if (stream->match(tokens::name, tokens::parao) || stream->match(tokens::gotoo, tokens::parao) || stream->match(tokens::name, tokens::jump)) {
			cmd* c = new cmd;
			c->opcode = codes::FUNC;
			token tokn = stream->peek();
			std::string n = stream->next().name;
			c->args.push(value(n,datatypes::variable)); // Set the func identifier as the first variable
			// Let's set the target
			if (!v.isNil()) {
				c->args.push(v); // Push the supplied variable
			}
			else {
				c->args.push(value()); // Creates a nil value 
			}
			// Already we have built: FUNC name val
			// Next we add arguments this is where things get interesting
			tokenstream tempstream;
			// This is a balanced consuming method (()(()))
			std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
			if (t.size() == 1) { // No arg function!
				current_chunk->addCmd(c);
				return true;
			}
			token end = t.back();
			t.pop_back();
			t.push_back(token{ tokens::seperator,codes::NOOP,"",t[0].line_num });
			t.push_back(token{ tokens::nil,codes::NOOP,"",t[0].line_num });
			t.push_back(end);
			tempstream.init(&t); // Turn tokens we consumed into a tokenstream
			if (tokn.type==tokens::gotoo) {
				if (tempstream.match(tokens::string) || tempstream.match(tokens::gotoo)) {
					buildGoto(tempstream.next().name);
					buildNoop();
					delete c;
					return true;
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
			}
			else if (tokn.type == tokens::jump) {
				if (tempstream.match(tokens::string) || tempstream.match(tokens::jump)) {
					buildJump(tempstream.next().name);
					buildNoop();
					delete c;
					return true;
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
			}
			value tempval;
			token tok;
			value ref = value(datatypes::variable);
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				debugInvoker(stream);
				tempval = value(datatypes::variable);
				tok = tempstream.peek();
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
				} 
				else if (match_process_standard(&tempstream, tempval)) {
					c->args.push(tempval);
				}
				else if (tempstream.match(tokens::newline)) {
					tempstream.next();
				}
				else if (tempstream.match(tokens::parac)) {
					tempstream.next();
					current_chunk->addCmd(c); // We push this onto the chunk after all dependants if any have been handled
					return true;
				}
				else {
					badSymbol(&tempstream);
				}
			}
		}
		else if (stream->match(tokens::name,tokens::dot,tokens::name,tokens::parao)) {
			cmd* c = new cmd;
			c->opcode = codes::OFUN;
			// OFUN obj fname target args
			c->args.push(value(stream->next().name,datatypes::variable)); // push the obj
			stream->next(); // consume the dot
			c->args.push(value(stream->next().name,datatypes::variable)); // push the fname
			// Let's set the target
			if (!v.isNil()) {
				c->args.push(v); // Push the supplied variable
			}
			else {
				c->args.push(value()); // Creates a nil value 
			}
			// Already we have built: FUNC name val
			// Next we add arguments this is where things get interesting
			tokenstream tempstream;
			// This is a balanced consuming method (()(()))
			std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
			if (t.size() == 1) { // No arg function!
				current_chunk->addCmd(c);
				return true;
			}
			token end = t.back();
			t.pop_back();
			t.push_back(token{ tokens::seperator,codes::NOOP,"",t[0].line_num });
			t.push_back(token{ tokens::nil,codes::NOOP,"",t[0].line_num });
			t.push_back(end);
			tempstream.init(&t); // Turn tokens we consumed into a tokenstream
			value tempval;
			token tok;
			value ref = value(datatypes::variable);
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				debugInvoker(stream);
				tempval = value(datatypes::variable);
				tok = tempstream.peek();
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
				}
				else if (match_process_standard(&tempstream, tempval)) {
					c->args.push(tempval);
				}
				else if (tempstream.match(tokens::newline)) {
					tempstream.next();
				}
				else if (tempstream.match(tokens::parac)) {
					tempstream.next();
					current_chunk->addCmd(c); // We push this onto the chunk after all dependants if any have been handled
					//lastCall.pop();
					return true;
				}
				else {
					badSymbol(&tempstream);
				}
			}
		}
		return false;
	}
	bool LineParser::match_process_goto(tokenstream* stream) {
		if (stream->match(tokens::gotoo,tokens::name) || stream->match(tokens::gotoo,tokens::string)) {
			stream->next(); // consume gotoo
			if (stream->match(tokens::name)) {
				buildGoto(stream->next().name,true);
			}
			else {
				buildGoto(stream->next().name);
			}
			return true;
		}
		return false;
	}
	bool LineParser::match_process_index(tokenstream* stream, value& v, bool leftside) {
		if (stream->match(tokens::name,tokens::bracketo)) {
			std::string name = stream->next().name;
			std::vector<token> tok = stream->next(tokens::bracketo, tokens::bracketc);
			tok.pop_back(); // Remove the last element since its a ']'
			tok.push_back(token{ tokens::newline,codes::NOOP,"",tok[0].line_num });
			tokenstream tempstream; // As usual the tokens are balanced match to [...] where the contents of tok = ...
			tempstream.init(&tok);
			value tempval = value(datatypes::variable);
			cmd* c = new cmd;
			if (leftside) {
				c->opcode = codes::ASID;
			}
			else {
				c->opcode = codes::INDX;
			}
			int nlcount = 0;
			while (tempstream.peek().type != tokens::none) { // Keep going until we hit the end
				if (match_process_standard(&tempstream, tempval)) {
					c->args.push(v);
					c->args.push(value(name,datatypes::block));
					c->args.push(tempval);
				}
				else if (nlcount) {
					state->push_error(errors::error{ errors::badtoken,concat("Unexpected symbol '",tempstream.last().toString(),"' Expected ']' to close list (line: ",tempstream.last().line_num,") Indexing must be done on one line?"),true,tempstream.last().line_num,current_chunk });
					return false;
				}
				else if (tempstream.match(tokens::newline)) {
					nlcount++;
					tempstream.next();
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
			}
			
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_jump(tokenstream* stream) {
		if (current_chunk->type == blocktype::bt_character || current_chunk->type == blocktype::bt_env)
			return false;
		if (stream->match(tokens::jump, tokens::name) || stream->match(tokens::jump, tokens::string)) {
			cmd* c = new cmd;
			c->opcode = codes::JUMP;
			stream->next(); // consume jump
			if (stream->match(tokens::name)) {
				c->args.push(value(stream->next().name,datatypes::variable));
			}
			else {
				c->args.push(value(stream->next().name));
			}
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_exit(tokenstream* stream) {
		if (stream->match(tokens::exit)) {
			stream->next();
			cmd* c = new cmd;
			c->opcode = codes::EXIT;
			if (stream->match(tokens::number) || stream->match(tokens::name)) {
				if(stream->match(tokens::number)){
					c->args.push(value(std::stod(stream->next().name)));
				}
				else {
					c->args.push(value(stream->next().name,datatypes::variable));
				}
			}
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_IFFF(tokenstream* stream) {
		return false; // TODO finish this
	}
	
	bool LineParser::match_process_wait(tokenstream* stream) {
		if (stream->match(tokens::name, tokens::number)) {
			if (stream->peek().name == "wait") {
				stream->next();
				buildWait(std::stod(stream->next().name));
			}
			else {
				return false;
			}
		}
		return false;
	}

	void reset(value& l, codes::op& op, value& r) {
		l = value();
		op = codes::NOOP;
		r = value();
	}

	bool LineParser::match_process_expression(tokenstream* stream, value& v) {
		// I will have to consume for this to work so we need to keep track of what was incase we return false!
		stream->store(current_chunk);
		cmd* lastcmd = nullptr;
		// It has to start with one of these 3 to even be considered an expression
		if ((stream->match(tokens::number) || stream->match(tokens::name) || stream->match(tokens::parao)) && stream->tokens.size()>=3) {
			// What do we know, math expressions can only be on a single line. We know where to stop looking if we have to
			cmd* c = new cmd;
			value wv;
			value left; // lefthand
			codes::op op; // opperator
			value right; // righthand
			reset(left, op, right);
			size_t loops = 0;
			bool hasOP = false;
			while (stream->peek().type != tokens::none) {
				debugInvoker(stream);
				if (stream->match(tokens::parao)) {
					tokenstream temp;
					temp.init(&(stream->next(tokens::parao, tokens::parac))); // Balanced match!
					value tmpvalue = value(datatypes::variable);
					if (match_process_expression(&temp, tmpvalue)) {
						if (left.isNil())
							left = tmpvalue;
						else if (right.isNil())
							right = tmpvalue;
						else
							badSymbol(stream);
					}
					else {
						badSymbol(stream);
					}
					// Take that temp value and set it to left or right TODO finish this
				}
				else if (stream->match(tokens::plus)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::ADD;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::minus)) {
					hasOP = true;
					if (left.isNil()) {
						left = value(0); // -5 is the same as 0-5 right? Also -(5+5) is the same as 0-(5+5) So we good
					}
					if (op == codes::NOOP)
						op = codes::SUB;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::multiply)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::MUL;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::divide)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::DIV;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::percent)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::MOD;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::caret)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::POW;
					else
						badSymbol(stream);
					stream->next();
				}
				else if (stream->match(tokens::name,tokens::parao)) {
					//Function template ^^^ If we encounter an issue the method should push an error, incase it misses something we will!
					value tmpvalue = value(datatypes::variable);
					if (match_process_function(stream,tmpvalue)) {
						if (left.isNil())
							left = tmpvalue;
						else if (right.isNil())
							right = tmpvalue;
						else
							badSymbol(stream);
					}
					else {
						badSymbol(stream);
					}
				}
				else if (stream->match(tokens::number)) {
					double num = std::stod(stream->next().name);
					if (left.isNil())
						left = value(num);
					else if (right.isNil())
						right = value(num);
					else
						badSymbol(stream);
				}
				else if (stream->match(tokens::name)) {
					// We tested functions already! So if that fails and we have a name then... we have a variable lets handle this!
					if (left.isNil())
						left = value(stream->next().name,datatypes::variable);
					else if (right.isNil())
						right = value(stream->next().name,datatypes::variable);
					else
						badSymbol(stream);
				}
				else if (stream->match(tokens::newline) || stream->match(tokens::parac) || stream->match(tokens::seperator)) {
					if (wv.isNil())
						return stream->restore(lastcmd, current_chunk); // Always return false and restores the position in stream!
					cmd* cc = new cmd;
					cc->opcode = codes::ASGN;
					cc->args.push(v);
					cc->args.push(wv);
					current_chunk->addCmd(cc);
					if(stream->match(tokens::parac))
						stream->next();
					// We done!
					int t=0;
					return true;
				}
				else if (stream->match(tokens::newline)) {
					stream->next();
				}
				else {
					return stream->restore(lastcmd, current_chunk);
				}
				if (!left.isNil() && !right.isNil() && op != codes::NOOP) {
					cmd* c = new cmd;
					c->opcode = op;
					if (wv.isNil()) {
						value temp = value(datatypes::variable);
						c->args.push(temp);
						wv = temp;
					}
					else {
						wv = value(datatypes::variable);
						c->args.push(wv);
					}
					c->args.push(left);
					c->args.push(right);
					current_chunk->addCmd(c);
					reset(left, op, right);
					left = wv;
				}
			}
		}
		else {
			return stream->restore(lastcmd, current_chunk); // Always return false and restores the position in stream!
		}
	}
}