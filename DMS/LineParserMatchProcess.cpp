#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	bool LineParser::match_process_standard(tokenstream* stream, value* v) {
		if (match_process_expression(stream,v)) {
			return true; // Nothing todo
		}
		else if (match_process_function(stream, v)) {
			return true; // Nothing todo
		}
		else if (match_process_list(stream, v)) {
			return true;
		}
		else if (stream->match(tokens::True)) {
			v->set(buildBool(true));
			stream->next();
			return true;
		}
		else if (stream->match(tokens::False)) {
			v->set(buildBool(false));
			stream->next();
			return true;
		}
		else if (stream->match(tokens::number)) {
			v->set(buildNumber(std::stod(stream->next().name)));
			return true;
		}
		else if (stream->match(tokens::string)) {
			v->set(buildString(stream->next().name));
			return true;
		}
		else if (stream->match(tokens::name)) {
			v->set(buildString(stream->next().name));
			v->type = datatypes::variable;
			return true;
		}
		else if (stream->match(tokens::nil)) {
			stream->next();
			v->set();
			return true;
		}
		else if (stream->match(tokens::bracketo, tokens::name, tokens::bracketc)) {
			// We are assigning a block as a variable
			stream->next();
			v->set(buildString(stream->next().name));
			v->type = datatypes::block;
			stream->next();
			return true;
		}
		else if (stream->match(tokens::newline)) {
			return false;
		}
		return false;
	}
	bool LineParser::match_process_list(tokenstream* stream, value* v) {
		if (stream->match(tokens::cbracketo)) {
			token start = stream->peek();
			std::vector<token> t = stream->next(tokens::cbracketo, tokens::cbracketc);
			tokenstream tempstream;
			tempstream.init(&t);
			value* ref = buildVariable();
			value* length = new value;
			size_t count = 0;
			cmd* c = new cmd;
			c->opcode = codes::LIST;
			c->args.push(v);
			c->args.push(length);
			current_chunk->addCmd(c);
			while (tempstream.peek().type != tokens::none) {
				print(tempstream.peek());
				if (tempstream.match(tokens::cbracketc)) {
					c = new cmd;
					c->opcode = codes::INST;
					c->args.push(v);
					c->args.push(ref);
					current_chunk->addCmd(c);
					break;
				}
				// This will modify the ref!!!
				else if (match_process_standard(&tempstream, ref)) {
					count++;
					print(">>",tempstream.peek());

					// TODO: make sure each statement is properly seperated by a comma n such

					/*if (tempstream.match(tokens::seperator) || tempstream.match(tokens::newline,tokens::seperator)) {
						print("Good!");
					}*/
					/*if (!(tempstream.match(tokens::seperator) || tempstream.match(tokens::cbracketc) || tempstream.match(tokens::newline))) {
						state->push_error(errors::error{ errors::badtoken,concat("Unexpected symbol '",tempstream.next().toString(),"' Expected '}' to close list at ",start.line_num),true,tempstream.peek().line_num,current_chunk });
						return false;
					}*/
				}
				else if (tempstream.match(tokens::newline)) {
					tempstream.next(); // this is fine
				}
				else if (tempstream.match(tokens::seperator)) {
					// Handle the next piece
					c = new cmd;
					c->opcode = codes::INST;
					c->args.push(v);
					c->args.push(ref);
					current_chunk->addCmd(c);
					// Build new value
					ref = buildVariable();
					tempstream.next();
				}
				else if (tempstream.match(tokens::cbracketo)) {
					tempstream.next();
				}
				else {
					badSymbol(&tempstream);
				}
			}
			length->set(buildNumber(count)); // the second argument is the length of the list! This should be modified if lists are changed at runtime!

			return true;
		}
		return false;
	}
	bool LineParser::match_process_disp(tokenstream* stream) {
		/*
			DISP, "msg"
			DISP, "msg" speaker

			Compound DISP
		*/
		//lastCall.push("MP_disp");
		if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline, tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			std::string msg = stream->next().name;
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			//c->args.push(buildValue());
			c->args.push(buildValue(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			//lastCall.pop();
			return true;
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
			c->args.push(buildVariable(name));
			current_chunk->addCmd(c);
			c = new cmd;
			c->opcode = codes::DISP;
			//c->args.push(buildValue());
			c->args.push(buildValue(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			// We might have to consume a newline... Depends on what's next
			//lastCall.pop();
			return true;
		}
		else if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::name,tokens::colon,tokens::cbracketo)) {
			std::string name = stream->next().name;
			cmd* c = new cmd;
			c->opcode = codes::SSPK;
			c->args.push(buildVariable(name));
			current_chunk->addCmd(c);
			// Reset the display for the new speaker. Append can be used!
			//c = new cmd;
			//c->opcode = codes::DISP;
			//c->args.push(buildValue());
			//c->args.push(buildValue(std::string("")));
			//current_chunk->addCmd(c);
			// Command to set the speaker
			stream->next();
			stream->next();
			while (stream->peek().type != tokens::cbracketc) {
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
							c->args.push(buildValue(mode));
							current_chunk->addCmd(c);
							// Now build the apnd msg cmd
							c = new cmd;
							c->opcode = codes::APND;
							c->args.push(buildValue(stream->next().name));
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
					c->args.push(buildValue(stream->next().name));
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
			return true;
		}
		// emotion: "path"
		// looks like a simple disp command
		else if (isBlock(bt_character) && stream->match(tokens::tab, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			//lastCall.pop();
			return true;
		}

		// TODO: We still have to implement the compound disp
		//lastCall.pop();
		return false;
	}
	bool LineParser::match_process_assignment(tokenstream* stream) {
		// something equals something else lets go
		//lastCall.push("MP_assignment");
		if (stream->match(tokens::name,tokens::equal)) {
			value* var = buildVariable(stream->next().name); // The variable that we will be setting stuff to
			stream->next(); // Consume the equal
			cmd* c = new cmd;
			c->opcode = codes::ASGN;
			c->args.push(var);
			if (match_process_list(stream, var)) {
				return true;
			}
			else if (match_process_expression(stream, var)) {
				// Expressions can internally set variables
				// We actually need to clean up our cmds
				return true;
			}
			else if (match_process_function(stream, var)) {
				// Functions can internally set variables
				// We actually need to clean up our cmds
				return true;
			}
			else if (stream->match(tokens::True)) {
				stream->next();
				c->args.push(buildValue(true));
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::False)) {
				stream->next();
				c->args.push(buildValue(false));
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::string)) {
				c->args.push(buildValue(stream->next().name));
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::nil)) {
				stream->next();
				c->args.push(buildValue());
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::number)) {
				c->args.push(buildValue(std::stod(stream->next().name)));
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(tokens::bracketo, tokens::name, tokens::bracketc)) {
				// We are assigning a block as a variable
				stream->next();
				c->args.push(buildBlock(stream->next().name));
				current_chunk->addCmd(c);
				stream->next();
				return true;
			}
			else if (stream->match(tokens::name)) {
				c->args.push(buildVariable(stream->next().name));
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
					c->args.push(buildValue(stream->next().name));
				}
				else {
					c->args.push(buildVariable(stream->next().name));
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
		//lastCall.push("MP_choice");
		token temp = stream->peek();
		if (temp.raw == codes::CHOI && stream->match(tokens::control,tokens::string)) {
			// Let's parse choice blocks.
			stream->next();
			std::string prompt = stream->next().name;
			bool good = true;
			std::string option;
			cmd* c = new cmd;
			// Create a unique label name by using the line number
			std::string choicelabel = concat("$CHOI_END_", stream->peek().line_num);
			c->opcode = codes::CHOI;
			c->args.push(buildValue(prompt));
			current_chunk->addCmd(c); // We will keep a reference to this and add to it as we go through the list
			bool start = false;
			bool hasfunc = false;
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
			std::string str = concat("$",stream->peek().line_num);
			while (!stream->match(tokens::cbracketc)) {
				if (stream->match(tokens::cbracketo) && !start) {
					start = true;
					stream->next();
				}
				else if (stream->match(tokens::cbracketo) && start) {
					badSymbol(stream);
				}
				else if (stream->match(tokens::string)) {
					std::string name = stream->next().name;
					c->args.push(buildValue(name)); // We append the choice to the first part of the CHOI cmd

					// We consumed the option now lets do some matching, note that all of these are one liners in the bytecode!
					if (match_process_function(stream,nullptr,false)) { // No returns and also no nesting of functions!
						// We cannot have a nested function here, but if we dont have that then we add our goto
						hasfunc = true;
						buildGoto(str);
					}
					else if (match_process_goto(stream)) {
						buildNoop(); // Add noop to post-goto command
					}
					else if (match_process_jump(stream)) {
						buildNoop(); // Add noop to post-jump command
					}
					else if (match_process_exit(stream)) {
						buildNoop(); // Add noop to post-exit command
					}
				}
				// Last Match
				else if (stream->match(tokens::newline)) {
					stream->next(); // Consume
				}
				else {
					badSymbol(stream);
				}
			}
			cmd* cc = current_chunk->cmds.back(); // Delete last element
			current_chunk->cmds.pop_back();
			delete cc;
			if (hasfunc)
				buildLabel(str);
			//lastCall.pop();
			return true;
		}
		//lastCall.pop();
		return false;
	}

	void cleanup(value* v) {
		v->nuke(); // Make sure we clean up the data
		delete[] v; // We didn't need it, lets clean it up!
	}
	bool LineParser::match_process_function(tokenstream* stream, value* v, bool nested) {
		//lastCall.push("MP_function");
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
		if (stream->match(tokens::name, tokens::parao)) {
			cmd* c = new cmd;
			c->opcode = codes::FUNC;
			std::string n = stream->next().name;
			c->args.push(buildVariable(n)); // Set the func identifier as the first variable
			// Let's set the target
			if (v != nullptr) {
				c->args.push(v); // Push the supplied variable
			}
			else {
				c->args.push(buildValue()); // Creates a nil value 
			}
			// Already we have built: FUNC name val
			// Next we add arguments this is where things get interesting
			tokenstream tempstream;
			// This is a balanced consuming method (()(()))
			std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
			token end = t.back();
			t.pop_back();
			t.push_back(token{ tokens::seperator,codes::NOOP,"",t[0].line_num });
			t.push_back(token{ tokens::nil,codes::NOOP,"",t[0].line_num });
			t.push_back(end);
			tempstream.init(&t); // Turn tokens we consumed into a tokenstream
			value* tempval;
			token tok;
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				print("> ", tempstream.peek());
				tempval = buildVariable();
				tok = tempstream.peek();
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
					cleanup(tempval); // We don't need it
				} 
				else if (match_process_list(&tempstream, tempval)) {
					c->args.push(tempval);
				}
				else if (match_process_expression(&tempstream, tempval)) {
					c->args.push(tempval);
				}
				else if (tempstream.match(tokens::string)) {
					cleanup(tempval);
					tempval = buildValue(tempstream.next().name); // Get the string
					c->args.push(tempval); // add this argument to the function opcodes
				}
				else if (tempstream.match(tokens::number)) {
					cleanup(tempval);
					std::string str = tempstream.next().name;
					tempval = buildValue(std::stod(str)); // Get the number and convert it to a double then build a value
					c->args.push(tempval); // add this argument to the function opcodes
				}
				else if (tempstream.match(tokens::True)) {
					cleanup(tempval);
					tempval = buildValue(true);
					c->args.push(tempval);
					tempstream.next();
				}
				else if (tempstream.match(tokens::False)) {
					cleanup(tempval);
					tempval = buildValue(false);
					c->args.push(tempval);
					tempstream.next();
				} else if (tempstream.match(tokens::nil)){
					cleanup(tempval);
					tempval = buildValue();
					c->args.push(tempval);
					tempstream.next();
				}
				// Final match this could be a function it might also be an expression
				else if (match_process_function(&tempstream, tempval)) {
					if (!nested) {
						state->push_error(errors::error{ errors::nested_function,"Nested functions are not allowed in this context!",true, tempstream.peek().line_num });
					}
					c->args.push(tempval);
				}
				else if (tempstream.match(tokens::name)) {
					cleanup(tempval);
					tempval = buildVariable(tempstream.next().name);
					c->args.push(tempval);
				}
				else if (tempstream.match(tokens::newline)) {
					tempstream.next();
				}
				else if (tempstream.match(tokens::parac)) {
					cleanup(tempval);
					tempstream.next();
					current_chunk->addCmd(c); // We push this onto the chunk after all dependants if any have been handled
					//lastCall.pop();
					return true;
				}
				else {
					cleanup(tempval); // Cleanup
					badSymbol(&tempstream);
				}
			}
		}
		//lastCall.pop();
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
	bool LineParser::match_process_jump(tokenstream* stream) {
		if (stream->match(tokens::jump, tokens::name) || tokens::jump, tokens::string) {
			cmd* c = new cmd;
			c->opcode = codes::JUMP;
			stream->next(); // consume jump
			if (stream->match(tokens::name)) {
				c->args.push(buildVariable(stream->next().name));
			}
			else {
				c->args.push(buildValue(stream->next().name));
			}
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_exit(tokenstream* stream) {
		if (stream->match(tokens::exit)) {
			cmd* c = new cmd;
			c->opcode = codes::EXIT;
			if (stream->match(tokens::number) || stream->match(tokens::name)) {
				if(stream->match(tokens::number)){
					c->args.push(buildValue(std::stod(stream->next().name)));
				}
				else {
					c->args.push(buildVariable(stream->next().name));
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
	void reset(value*& l,codes::op& op, value*& r) {
		l = nullptr;
		op = codes::NOOP;
		r = nullptr;
	}
	bool LineParser::match_process_expression(tokenstream* stream, value* v) {
		// I will have to consume for this to work so we need to keep track of what was incase we return false!
		stream->store(current_chunk);
		cmd* lastcmd = nullptr;
		/*if(!current_chunk->cmds.empty())
			lastcmd = current_chunk->cmds.back();*/
		// It has to start with one of these 3 to even be considered an expression
		if (stream->match(tokens::number) || stream->match(tokens::name) || stream->match(tokens::parao)) {
			// What do we know, math expressions can only be on a single line. We know where to stop looking if we have to
			cmd* c = new cmd;
			/*
			* We have a few built-in methods we will have to handle
			* ADD val v1 v2
			* SUB val v1 v2
			* MUL val v1 v2
			* DIV val v1 v2
			* POW val v1 v2
			* MOD val v1 v2
			*/
			value* wv = nullptr;
			value* left; // lefthand
			codes::op op; // opperator
			value* right; // righthand
			reset(left, op, right);
			size_t loops = 0;
			bool hasOP = false;
			while (stream->peek().type != tokens::none) {
				if (stream->match(tokens::parao)) {
					tokenstream temp;
					temp.init(&(stream->next(tokens::parao, tokens::parac))); // Balanced match!
					value* tmpvalue = buildVariable();
					if (match_process_expression(&temp, tmpvalue)) {
						if (left == nullptr)
							left = tmpvalue;
						else if (right == nullptr)
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
					if (left == nullptr) {
						left = buildValue(0); // -5 is the same as 0-5 right? Also -(5+5) is the same as 0-(5+5) So we good
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
					value* tmpvalue = buildVariable();
					if (match_process_function(stream,tmpvalue)) {
						if (left == nullptr)
							left = tmpvalue;
						else if (right == nullptr)
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
					if (left == nullptr)
						left = buildValue(num);
					else if (right == nullptr)
						right = buildValue(num);
					else
						badSymbol(stream);
				}
				else if (stream->match(tokens::name)) {
					// We tested functions already! So if that fails and we have a name then... we have a variable lets handle this!
					if (left == nullptr)
						left = buildVariable(stream->next().name);
					else if (right == nullptr)
						right = buildVariable(stream->next().name);
					else
						badSymbol(stream);
				}
				else if (/*stream->match(tokens::newline) || */stream->match(tokens::parac) || stream->match(tokens::seperator)) {
					if (wv == nullptr)
						return stream->restore(lastcmd, current_chunk); // Always return false and restores the position in stream!
					cmd* cc = new cmd;
					cc->opcode = codes::ASGN;
					cc->args.push(v);
					cc->args.push(wv);
					current_chunk->addCmd(cc);
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
				if (left != nullptr && right != nullptr && op != codes::NOOP) {
					cmd* c = new cmd;
					c->opcode = op;
					if (wv == nullptr) {
						value* temp = buildVariable();
						c->args.push(temp);
						wv = temp;
					}
					else {
						wv = buildVariable();
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