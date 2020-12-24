#include "pch.h"
#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
// TODO: process if elseif else statements, for loops and while loops
namespace dms {
	bool LineParser::match_process_standard(tokenstream* stream, value& v) {
		stream->chomp(newline);
		//utils::debug(stream->peek());
		if (stream->peek().type == tokens::none) {
			return false;
		}
		//!something
		if (stream->match(exclamation)) {
			stream->next(); // Consume the token
			// Within the standard we always get passed a valid value, We need to pass a new value to a recursive call to standard then pass through
			if (stream->match(name)) {
				v = value(stream->next().name, variable);
				cmd* c = new cmd;
				c->opcode = codes::KNOT;
				c->args.push(v);
				current_chunk->addCmd(c);
				return true;
			}
			else if (stream->match(parao)) {
				size_t last_line = stream->peek().line_num;
				std::vector<tokens::token> toks = stream->next(tokens::parao, tokens::parac);
				if (notBalanced(toks, last_line, stream, "(", ")"))
					return false;
				toks.pop_back(); // Remove the ')'
				tokenstream tempstream(&toks);
				value var(variable);
				if (match_process_standard(&tempstream,var)) {
					value assn = value(datatypes::variable); // The variable that we will be setting stuff to
					stream->next(); // Consume the equal
					cmd* c = new cmd;
					c->opcode = codes::ASGN;
					c->args.push(assn);
					c->args.push(var);
					current_chunk->addCmd(c);

					c = new cmd;
					c->opcode = codes::KNOT;
					c->args.push(assn);
					current_chunk->addCmd(c);
					v.set(assn.s);
					v.type = variable;
					return true;
				}
			}
			else {
				stop = true;
				state->push_error(errors::error{ errors::unknown,concat("Unexpected symbol '",stream->next().toString(), "' near '!' \"Not\" can only be used on a varaible or a group!"),true,stream->peek().line_num,current_chunk });
				return false;
			}
		}
		if (stream->match(tokens::parao)) {
			size_t last_line = stream->peek().line_num;
			std::vector<tokens::token> toks = stream->next(tokens::parao, tokens::parac);
			if (notBalanced(toks, last_line, stream, "(", ")"))
				return false;
			toks.pop_back(); // Remove the ')'
			toks.push_back(tokens::token{tokens::newline,codes::NOOP,"",stream->peek().line_num});
			tokenstream tempstream(&toks);
			value var(datatypes::variable);
			if (match_process_standard(&tempstream, var)) {
				v.set(var.s);
				v.type = datatypes::variable;
				return true;
			}
			else {
				badSymbol(stream);
				return false;
			}
			return true;
		}
		if (match_process_ooxoo(stream,v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (match_process_1afunc(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (match_process_expression(stream, v)) {
			match_process_condition(stream,v);
			return true;
		}
		else if (match_process_function(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (match_process_list(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (match_process_index(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::True)) {
			v.set(true);
			stream->next();
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::False)) {
			v.set(false);
			stream->next();
			match_process_condition(stream, v);
			return true;
		}
		else if (match_process_number(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::string)) {
			v.set(stream->next().name);
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::name)) {
			v.set(stream->next().name);
			v.type = datatypes::variable;
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::nil)) {
			stream->next();
			v.set();
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::bracketo, tokens::name, tokens::bracketc)) {
			// We are assigning a block as a variable
			stream->next();
			v.set(stream->next().name);
			v.type = datatypes::block;
			stream->next();
			match_process_condition(stream, v);
			return true;
		}
		else if (stream->match(tokens::newline)) {
			stream->next();
			return match_process_standard(stream,v);
		} else if (match_process_andor(stream, v)) {
			match_process_condition(stream, v);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_ooxoo(tokenstream* stream, value& v) {
		if (stream->match(name, plus, plus)) {

		}
		else if (stream->match(name, minus, minus)) {

		}
		else if (stream->match(plus, plus, name)) {

		}
		else if (stream->match(minus, minus, name)) {

		}
		return false;
	}
	bool LineParser::match_process_andor(tokenstream* stream, value& v) {
		codes::op code = codes::MUL;
		if (stream->match(tokens::Or)) {
			code = codes::ADD;
		}
		else if (!stream->match(tokens::And)){
			return false;
		}
		stream->next();
		value right = value(datatypes::variable);
		value left = v;
		value var = value(datatypes::variable);
		// We have some work to do here
		if (match_process_standard(stream, right)) {
			v.set(var.s);
			v.type = datatypes::variable;
			cmd* c = new cmd;
			c->opcode = code;
			c->args.push(v);
			c->args.push(left);
			c->args.push(right);
			current_chunk->addCmd(c);
			return true;
		}
		else {
			badSymbol(stream);
			return false;
		}
	}
	bool LineParser::match_process_condition(tokenstream* stream, value& v) {
		// This has taken way too long, but there exists only a few places where this needs to be interjected
		// The reference to v needs some work if we have a comparison!
		// First we need to get a copy of v store it somewhere do the comparision and convert v into a variable that points to the output of the comparison
		comp cmp;
		// We only need to see if one of these conditions are true
		//==
		if (stream->match(tokens::equal,tokens::equal)) {
			cmp = comp::eq;
			stream->next();
			stream->next();
		}
		//<=
		else if (stream->match(tokens::anglebracketO, tokens::equal)) {
			cmp = comp::lteq;
			stream->next();
			stream->next();
		}
		//>=
		else if (stream->match(tokens::anglebracketC, tokens::equal)) {
			cmp = comp::gteq;
			stream->next();
			stream->next();
		}
		//!=
		else if (stream->match(tokens::exclamation, tokens::equal)) {
			cmp = comp::nteq;
			stream->next();
			stream->next();
		}
		//<
		else if (stream->match(tokens::anglebracketO)) {
			cmp = comp::lt;
			stream->next();
		}
		//>
		else if (stream->match(tokens::anglebracketC)) {
			cmp = comp::gt;
			stream->next();
		}
		else {
			return false;
		}
		// So if all is good we continue here
		value right = value(datatypes::variable);
		value left = v;
		value var = value(datatypes::variable);
		// COMP cmp out v1 v2
		if (match_process_standard(stream,right)) {
			v.set(var.s);
			v.type = datatypes::variable;
			cmd* c = new cmd;
			c->opcode = codes::COMP;
			c->args.push(value((int)cmp));
			c->args.push(var);
			c->args.push(left);
			c->args.push(right);
			current_chunk->addCmd(c);
			if (match_process_andor(stream, v)) {
				match_process_standard(stream, v);
			}
			return true;
		}
		else {
			badSymbol(stream);
			return false;
		}
	}
	//Covers if, elseif, else
	bool LineParser::match_process_list(tokenstream* stream, value& v) {
		if (stream->match(tokens::cbracketo)) {
			token start = stream->peek();
			token ancor = start;
			size_t last_line = stream->last().line_num;
			std::vector<token> t = stream->next(tokens::cbracketo, tokens::cbracketc);
			if (notBalanced(t, last_line, stream, "{", "}"))
				return false;
			tokenstream tempstream(&t);
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
	bool LineParser::match_process_1afunc(tokenstream* stream, value& v ) {
		if (stream->match(tokens::name,tokens::string) || stream->match(tokens::name,tokens::number) || stream->match(tokens::name,tokens::minus)) {
			cmd* c = new cmd;
			c->opcode = codes::FUNC;
			c->args.push(value(stream->next().name,variable));
			c->args.push(v);
			value num;
			if (stream->match(tokens::string)) {
				c->args.push(value(stream->next().name));
			} else if (match_process_number(stream,num)) {
				c->args.push(num);
			}
			current_chunk->addCmd(c);
			return true;
		}
		return false;
	}
	bool LineParser::match_process_disp(tokenstream* stream) {
		if (stream->match(tokens::name,tokens::colon)) {
			std::string name = stream->next().name;
			stream->next();
			cmd* c = new cmd;
			if (isBlock(bt_character)) {
				c->opcode = codes::STAT;
				c->args.push(value(name, datatypes::variable));
			}
			else {
				c->opcode = codes::SSPK;
				c->args.push(value(name, datatypes::variable));
			}
			current_chunk->addCmd(c);
			value msg(variable);
			if (stream->match(tokens::cbracketo)) {
				if (match_process_scope(stream)) {
					return true;
				}
				else {
					badSymbol(stream);
					return false;
				}
			} else if (match_process_standard(stream, msg)) {
				if (isBlock(bt_character)) {
					c->args.push(value(msg));
				}
				else {
					c = new cmd;
					c->opcode = codes::DISP;
					c->args.push(msg);
					current_chunk->addCmd(c); // Add the cmd to the current chunk
					current_chunk->addCmd(new cmd{ codes::HALT });
				}
			}
			else {
				badSymbol(stream);
				return false;
			}
			return true;
		}
		else if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline, tokens::string)) {
			stream->next(); // Standard consumption
			value temp(variable);
			if (match_process_standard(stream, temp)) {
				cmd* c = new cmd;
				c->opcode = codes::DISP;
				c->args.push(temp);
				current_chunk->addCmd(c); // Add the cmd to the current chunk
				current_chunk->addCmd(new cmd{ codes::HALT });
				return true;
			}
			else {
				badSymbol(stream);
				return false;
			}
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
		return false;
	}
	bool LineParser::match_process_return(tokenstream* stream) {
		// Only handle this inside of a function block!
		if (current_chunk->type == bt_method) {
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
					if (match_process_goto(stream)) {}
					else if (match_process_jump(stream)) {}
					else if (match_process_exit(stream)) {}
					else if (match_process_scope(stream)) {
						buildGoto(choicelabel);
					}
					else {
						badSymbol(stream);
					}
					//value val = value();
					//if (match_process_function(stream,val,false)) { // No returns and also no nesting of functions!
					//	// We cannot have a nested function here, but if we dont have that then we add our goto
					//	hasfunc = true;
					//	
					//}
					
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
			buildLabel(choicelabel);
			stream->next(); // Remove the last bracket!
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
			// This is a balanced consuming method (()(()))
			size_t last_line = stream->last().line_num;
			std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
			if (notBalanced(t, last_line, stream, "(", ")"))
				return false;
			//t.pop_back();
			tokenstream tempstream(&t);
			if (t.size() == 1) { // No arg function!
				current_chunk->addCmd(c);
				return true;
			}
			token end = t.back();
			t.pop_back();
			t.push_back(token{ tokens::seperator,codes::NOOP,"",t[0].line_num });
			t.push_back(token{ tokens::escape,codes::NOOP,"",t[0].line_num });
			t.push_back(end);
			tempstream.init(&t); // Turn tokens we consumed into a tokenstream
			if (tokn.type==tokens::gotoo) {
				if (tempstream.match(tokens::string)) {
					buildGoto(tempstream.next().name);
					delete c;
					return true;
				}
				else if (tempstream.match(tokens::name)) {
					buildGoto(tempstream.next().name,true);
					delete c;
					return true;
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
			}
			else if (tokn.type == tokens::jump) {
				if (tempstream.match(tokens::string)) {
					buildJump(tempstream.next().name);
					delete c;
					return true;
				}
				else if (tempstream.match(tokens::name)) {
					buildJump(tempstream.next().name, true);
					delete c;
					return true;
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
			}
			value tempval;
			value ref = value(datatypes::variable);
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				debugInvoker(stream);
				//utils::debug(stream->peek());
				tempval = value(datatypes::variable);
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
				}
				else if (tempstream.match(tokens::escape)) {
					c->args.push(value(datatypes::escape));
					tempstream.next();
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
					//utils::debug(tempstream.peek());
					badSymbol(&tempstream);
					return false;
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
			// This is a balanced consuming method (()(()))
			size_t last_line = stream->last().line_num;
			std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
			if (notBalanced(t, last_line, stream, "(", ")"))
				return false;
			if (t.size() == 1) { // No arg function!
				current_chunk->addCmd(c);
				return true;
			}
			token end = t.back();
			t.pop_back();
			t.push_back(token{ tokens::seperator,codes::NOOP,"",t[0].line_num });
			t.push_back(token{ tokens::escape,codes::NOOP,"",t[0].line_num });
			t.push_back(end);
			tokenstream tempstream(&t);
			value tempval;
			token tok;
			value ref = value(datatypes::variable);
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				debugInvoker(stream);
				//utils::debug(stream->peek());
				tempval = value(datatypes::variable);
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
				}
				else if (tempstream.match(tokens::escape)) {
					c->args.push(value(datatypes::escape));
					tempstream.next();
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
					//utils::debug(tempstream.peek());
					badSymbol(&tempstream);
					return false;
				}
			}
			return true;
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
		if (stream->match(tokens::name,tokens::dot,tokens::name)) {
			std::string name = stream->next().name;
			stream->next();
			std::string key = stream->next().name;
			cmd* c = new cmd;
			if (leftside) {
				c->opcode = codes::ASID;
			}
			else {
				c->opcode = codes::INDX;
			}
			c->args.push(v);
			c->args.push(value(name, datatypes::block));
			c->args.push(value(key));
			current_chunk->addCmd(c);
			return true;
		}
		else if (stream->match(tokens::name,tokens::bracketo)) {
			std::string name = stream->next().name;
			std::vector<token> toks = stream->next(tokens::bracketo, tokens::bracketc);
			toks.pop_back(); // Remove the last element since its a ']'
			toks.push_back(token{ tokens::newline,codes::NOOP,"",toks[0].line_num });
			tokenstream tempstream(&toks); // As usual the tokens are balanced match to [...] where the contents of tok = ...
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
					current_chunk->addCmd(c);
					return true;
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
					c->args.push(stov(stream->next().name));
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

	bool LineParser::match_process_scope(tokenstream* stream) {
		if (stream->match(cbracketo)) {
			size_t last_line = stream->last().line_num;
			std::vector<token> t = stream->next(cbracketo, cbracketc); // Consume and get tokens
			t.pop_back();
			tokenstream tempstream(&t);
			if (notBalanced(t, last_line, stream, "{", "}"))
				return false;
			// We got the balanced match, time to simple do it
			ParseLoop(&tempstream); // Done
			return true;
		}
		else if (ParseLoop(stream, 1)) {
			return true; // Let's try and match one command
		}
		return false;
	}
	bool LineParser::match_process_while(tokenstream* stream)
	{
		if (stream->match(tokens::name) && stream->peek().name == "while") {
			stream->next();
			stream->chomp(tokens::newline);
			value ref(datatypes::variable);
			std::string wstart = std::string("WHS_") + random_string(4);
			std::string wend = std::string("WHE_") + random_string(4);
			buildLabel(wstart);
			
			if (stream->match(parao)) {
				size_t last_line = stream->last().line_num;
				std::vector<token> t = stream->next(parao, parac); // Consume and get tokens
				t.pop_back();
				tokenstream tempstream(&t);
				if (notBalanced(t, last_line, stream, "(", ")"))
					return false;
				if (match_process_standard(&tempstream, ref)) {
					cmd* c = new cmd;
					c->opcode = codes::IFFF;
					c->args.push(ref);
					c->args.push(value(wend));
					current_chunk->addCmd(c);
					if (match_process_scope(stream)) {
						buildGoto(wstart);
						buildLabel(wend);
						return true;
					}
					else {
						badSymbol(stream);
						return false;
					}
				}
				else {
					badSymbol(stream);
					return false;
				}
			}
		}
		return false;
	}
	bool LineParser::match_process_for(tokenstream* stream)
	{
		if (stream->match(tokens::name) && stream->peek().name == "for") {
			stream->next();
			stream->chomp(tokens::newline);
			value ref(datatypes::variable);
			std::string fstart = std::string("FOS_") + random_string(4);
			std::string fend = std::string("FOE_") + random_string(4);
			if (stream->match(parao)) {
				size_t last_line = stream->last().line_num;
				std::vector<token> t = stream->next(tokens::parao, tokens::parac); // Consume and get tokens
				if (notBalanced(t, last_line, stream, "(", ")"))
					return false;
				t.pop_back();
				tokenstream tempstream(&t);
				cmd* c = new cmd;
				int count = 0;
				value iter;
				value start;
				value end;
				value inc = value(1);
				if (tempstream.match(tokens::name,tokens::equal)) {
					std::vector<token> assn = tempstream.next(tokens::seperator);
					assn.pop_back(); // That comma gets consumed
					tokenstream tstream(&assn);
					if (ParseLoop(&tstream, 1)) {
						// Grab the values from the parseloop
						cmd* tempc = current_chunk->cmds.back();
						iter = tempc->args.args[0];
						start = tempc->args.args[1];
						buildLabel(fstart); // Now that we assigned we can  build the lael
						if (match_process_standard(&tempstream, end)) {
							// Set end to the value it needs to be
							if (tempstream.match(tokens::seperator)) {
								tempstream.next();
								if (match_process_standard(&tempstream, inc)) {
									// If this works then we got all the needed values
									goto for_ack_done; // Skip all the checks from the nested if statements
								}
								else {
									badSymbol(&tempstream);
								}
							}
						}
						else {
							badSymbol(&tempstream);
							return false;
						}
					}
					else {
						badSymbol(&tstream);
						return false;
					}
				}
				else {
					badSymbol(&tempstream);
					return false;
				}
				// Now we build the rest of the bytecode
			for_ack_done:
				value co = value(variable);
				int com;

				if (start > end) // counting up
					com = comp::gteq;
				else // counting down
					com = comp::lteq;

				buildCmd(codes::COMP, { value(com), co, iter, end });

				buildCmd(codes::IFFF, { co, value(fend) });

				if (match_process_scope(stream)) {
					buildCmd(codes::ADD, {iter,inc,iter});
					buildGoto(fstart);
					buildLabel(fend);
					return true;
				}
				else {
					badSymbol(stream);
					return false;
				}
			}
		}
		return false;
	}
	bool LineParser::match_process_number(tokenstream* stream, value& v)
	{
		if (stream->match(tokens::number)) {
			v.set(stov(stream->next().name));
			return true;
		}
		else if (stream->match(tokens::minus, tokens::number)) {
			stream->next();
			v.set(stov(stream->next().name));
			v.i = -v.i;
			v.n = -v.n;
			return true;
		}
		return false;
	}
	bool LineParser::match_process_asm(tokenstream* stream)
	{
		if (stream->match(tokens::name) && stream->peek().name == "asm") {
			stream->next();
			stream->chomp(tokens::newline);
			if (stream->match(tokens::cbracketo)) {
				size_t last_line = stream->last().line_num;
				std::vector<token> t = stream->next(tokens::cbracketo, tokens::cbracketc); // Consume and get tokens
				if (notBalanced(t, last_line, stream, "{", "}"))
					return false;
				t.pop_back();
				tokenstream ts(&t);
				std::vector<value> temp;
				codes::op o;
				while (ts.can()) {
					//utils::debug(ts.peek());
					if (ts.match(tokens::string)) {
						temp.push_back(value(ts.next().name));
					}
					else if (ts.match(tokens::number)) {
						temp.push_back(stov(ts.next().name));
					}
					else if (ts.match(tokens::minus,tokens::number)) {
						ts.next();
						temp.push_back(stov(ts.next().name));
					}
					else if (ts.match(tokens::True)) {
						temp.push_back(value(true));
					}
					else if (ts.match(tokens::False)) {
						temp.push_back(value(false));
					}
					else if (ts.match(tokens::bracketo, tokens::name, tokens::bracketc)) {
						ts.next();
						temp.push_back(value(ts.next().name,block));
						ts.next();
					}
					else if (ts.match(tokens::percent, tokens::name)) {
						ts.next();
						temp.push_back(value(ts.next().name,variable));
					}
					else if (ts.match(tokens::newline)) {
						ts.next();
					}
					else if (ts.match(tokens::nil)) {
						temp.push_back(value());
						ts.next();
					}
					else if (ts.match(tokens::name)) {
						if (temp.size() >= 1) {
							buildCmd(o, temp);
						}
						temp.clear();
						std::string cmd = ts.next().name;
						tolower(cmd);
						if (cmd == "noop")
							o = codes::NOOP;
						else if (cmd == "entr")
							o = codes::ENTR;
						else if (cmd == "enab")
							o = codes::ENAB;
						else if (cmd == "disa")
							o = codes::DISA;
						else if (cmd == "load")
							o = codes::LOAD;
						else if (cmd == "vern")
							o = codes::VERN;
						else if (cmd == "usin")
							o = codes::USIN;
						else if (cmd == "stat")
							o = codes::STAT;
						else if (cmd == "disp")
							o = codes::DISP;
						else if (cmd == "asgn")
							o = codes::ASGN;
						else if (cmd == "labl")
							o = codes::LABL;
						else if (cmd == "choi")
							o = codes::CHOI;
						else if (cmd == "blck")
							o = codes::BLCK;
						else if (cmd == "whle")
							o = codes::WHLE;
						else if (cmd == "func")
							o = codes::FUNC;
						else if (cmd == "ifff")
							o = codes::IFFF;
						else if (cmd == "knot")
							o = codes::KNOT;
						else if (cmd == "else")
							o = codes::ELSE;
						else if (cmd == "defn")
							o = codes::DEFN;
						else if (cmd == "skip")
							o = codes::SKIP;
						else if (cmd == "comp")
							o = codes::COMP;
						else if (cmd == "indx")
							o = codes::INDX;
						else if (cmd == "inst")
							o = codes::INST;
						else if (cmd == "erro")
							o = codes::ERRO;
						else if (cmd == "goto")
							o = codes::GOTO;
						else if (cmd == "jump")
							o = codes::JUMP;
						else if (cmd == "retn")
							o = codes::RETN;
						else if (cmd == "exit")
							o = codes::EXIT;
						else if (cmd == "debg")
							o = codes::DEBG;
						else if (cmd == "dspd")
							o = codes::DSPD;
						else if (cmd == "dact")
							o = codes::DACT;
						else if (cmd == "wait")
							o = codes::WAIT;
						else if (cmd == "apnd")
							o = codes::APND;
						else if (cmd == "sspk")
							o = codes::SSPK;
						else if (cmd == "add")
							o = codes::ADD;
						else if (cmd == "sub")
							o = codes::SUB;
						else if (cmd == "mul")
							o = codes::MUL;
						else if (cmd == "div")
							o = codes::DIV;
						else if (cmd == "pow")
							o = codes::POW;
						else if (cmd == "mod")
							o = codes::MOD;
						else if (cmd == "list")
							o = codes::LIST;
						else if (cmd == "line")
							o = codes::LINE;
						else if (cmd == "halt")
							o = codes::HALT;
						else if (cmd == "file")
							o = codes::FILE;
						else if (cmd == "gc")
							o = codes::GC;
						else if (cmd == "asid")
							o = codes::ASID;
						else if (cmd == "ofun")
							o = codes::OFUN;
					}
					else {
						badSymbol(&ts);
						return false;
					}
				}
				if (temp.size() >= 1) {
					if(o == codes::FUNC || o == codes::OFUN)
						temp.push_back(escape);
					buildCmd(o, temp);
				}
				return true;
			}
		}
		return false;
	}
	/*
	stream->match(tokens::bracketo, tokens::name, tokens::bracketc)) {
			// We are assigning a block as a variable
			stream->next();
			v.set(stream->next().name);
			v.type = datatypes::block;
			stream->next();
			match_process_condition(stream, v);
			return true;
	*/
	bool LineParser::match_process_IFFF(tokenstream* stream) {
		/*if(this) {
		*	then()
		* }
		* else if(that){
		*	doThis()
		* } else {
		*	this()
		* }
		*/

		// We match a condition, or anything that is non nil/false
		// God controls are from a time past... I could refactor, but I'm lazy and would have to change a lot of old code... So we will deal with controls
		if (stream->match(tokens::name,tokens::parao) && stream->peek().name == "if") {
			stream->next();
			size_t last_line = stream->last().line_num;
			std::vector<token> ts = stream->next(tokens::parao, tokens::parac);
			if (notBalanced(ts, last_line, stream, "(", ")"))
				return false;
			ts.pop_back();
			tokenstream tmpstream(&ts);
			value cmp(datatypes::variable);
			value nil;
			stream->chomp(newline);
			if (match_process_standard(&tmpstream,cmp)) {
				std::string ifend = std::string("IFE_") + random_string(4);
				std::string next = std::string("IFF_") + random_string(4);
				cmd* c = new cmd;
				c->opcode = codes::IFFF;
				c->args.push(cmp);
				c->args.push(value(next));
				current_chunk->addCmd(c);
				if (match_process_scope(stream)) {
					/*size_t last_line = stream->last().line_num;
					std::vector<token> toks = stream->next(tokens::cbracketo, tokens::cbracketc);
					if (notBalanced(toks, last_line, stream, "{", "}"))
						return false;
					toks.pop_back();
					tokenstream tempstream(&toks);
					ParseLoop(&tempstream);*/
					buildGoto(ifend);
					buildLabel(next);
					stream->chomp(newline);
					if (match_process_ELIF(stream, ifend) || match_process_ELSE(stream, ifend)) {
					
					}
					else if (stream->match(tokens::pipe)) {
						stream->next();
						stream->chomp(newline);
						if (!match_process_scope(stream)) {
							state->push_error(errors::error{ errors::unknown,"Missing else function or scope",true,stream->peek().line_num,current_chunk });
							return false;
						}
					}
					buildLabel(ifend);
					// We keep trying to match else if/else until nothing is left
					return true;
				}
				else if (stream->match(tokens::name,tokens::parao)) {
					if (match_process_function(stream, nil)) {
						stream->chomp(newline);
						if (stream->match(tokens::pipe)) {
							stream->next();
							stream->chomp(newline);
							buildGoto(ifend);
							buildLabel(next);
							if (!match_process_function(stream, nil) && !match_process_scope(stream)) {
								state->push_error(errors::error{ errors::unknown,"Missing else function or scope",true,stream->peek().line_num,current_chunk });
								return false;
							}
						}
						else {
							state->push_error(errors::error{ errors::unknown,"Expected '|'",true,stream->peek().line_num,current_chunk });
							return false;
						}
					}
					else {
						state->push_error(errors::error{ errors::unknown,"Expected '{' or function pair!",true,stream->peek().line_num,current_chunk });
						return false;
					}
					buildLabel(ifend);
				}
				else {
					badSymbol(stream);
					return false;
				}
			}
			else {
				badSymbol(stream);
				return false;
			}
		}
		return false; // TODO finish this
	}
	bool LineParser::match_process_ELSE(tokenstream* stream, std::string ifend) {
		if (stream->match(tokens::name) && stream->peek().name == "else") {
			stream->next();
			if (match_process_scope(stream)) {
				return true;
			}
		}
		return false;
	}
	bool LineParser::match_process_ELIF(tokenstream* stream, std::string ifend) {
		if (stream->match(tokens::name, tokens::parao) && stream->peek().name == "elseif") {
			stream->next();
			size_t last_line = stream->last().line_num;
			std::vector<token> ts = stream->next(tokens::parao, tokens::parac);
			if (notBalanced(ts, last_line, stream, "(", ")"))
				return false;
			ts.pop_back();
			tokenstream tmpstream(&ts);
			value cmp(datatypes::variable);
			//buildLabel(iff);
			if (match_process_standard(&tmpstream, cmp)) {
				std::string next = std::string("IFF_") + random_string(4);
				if (match_process_scope(stream)) {
					/*size_t last_line = stream->last().line_num;
					std::vector<token> toks = stream->next(tokens::cbracketo, tokens::cbracketc);
					if (notBalanced(ts, last_line, stream, "{", "}"))
						return false;
					toks.pop_back();
					tokenstream tempstream(&toks);*/
					cmd* c = new cmd;
					c->opcode = codes::IFFF;
					c->args.push(cmp);
					c->args.push(value(next));
					current_chunk->addCmd(c);
					//ParseLoop(&tempstream);
					buildGoto(ifend);
					buildLabel(next);
					stream->chomp(newline);
					if (match_process_ELIF(stream, ifend) || match_process_ELSE(stream, ifend)) {}
					else if (stream->match(tokens::pipe)) {
						stream->next();
						stream->chomp(newline);
						if (!match_process_scope(stream)) {
							state->push_error(errors::error{ errors::unknown,"Missing else function or scope",true,stream->peek().line_num,current_chunk });
							return false;
						}
					}
					// We keep trying to match else if/else until nothing is left
					return true;
				}
			}
			else {
				badSymbol(stream);
				return false;
			}
		}
		return false;
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
		if ((stream->match(tokens::minus,tokens::number) || stream->match(tokens::number) || stream->match(tokens::string) || stream->match(tokens::name) || stream->match(tokens::parao)) && stream->tokens.size()>=3) {
			// What do we know, math expressions can only be on a single line. We know where to stop looking if we have to
			cmd* c = new cmd;
			value wv;
			value left; // left hand
			codes::op op; // opperator
			value right; // right hand
			reset(left, op, right);
			size_t loops = 0;
			bool hasOP = false;
			while (stream->peek().type != tokens::none) {
				debugInvoker(stream);
				if (stream->match(tokens::parao)) {
					size_t last_line = stream->last().line_num;
					auto ts = stream->next(tokens::parao, tokens::parac);
					if (notBalanced(ts, last_line, stream, "(", ")"))
						return false;
					tokenstream temp(&ts);
					value tmpvalue = value(datatypes::variable);
					if (match_process_expression(&temp, tmpvalue)) {
						if (left.isNil())
							left = tmpvalue;
						else if (right.isNil())
							right = tmpvalue;
						else {
							badSymbol(stream);
							return false;
						}
					}
					else {
						badSymbol(stream);
						return false;
					}
					// Take that temp value and set it to left or right TODO finish this
				}
				else if (stream->match(tokens::plus)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::ADD;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::minus)) {
					hasOP = true;
					if (left.isNil()) {
						left = value(0); // -5 is the same as 0-5 right? Also -(5+5) is the same as 0-(5+5) So we good
					}
					if (op == codes::NOOP)
						op = codes::SUB;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::multiply)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::MUL;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::divide)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::DIV;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::percent)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::MOD;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::caret)) {
					hasOP = true;
					if (op == codes::NOOP)
						op = codes::POW;
					else {
						badSymbol(stream);
						return false;
					}
					stream->next();
				}
				else if (stream->match(tokens::name,tokens::parao)) {
					//Function template ^^^ If we encounter an issue the method should push an error, incase it misses something we will!
					value tmpvalue = value(datatypes::variable);
					if (match_process_function(stream,tmpvalue)) {
						hasOP = false;
						if (left.isNil())
							left = tmpvalue;
						else if (right.isNil())
							right = tmpvalue;
						else {
							badSymbol(stream);
							return false;
						}
					}
					else {
						badSymbol(stream);
						return false;
					}
				}
				else if (stream->match(tokens::number)) {
					hasOP = false;
					if (left.isNil())
						left = stov(stream->next().name);
					else if (right.isNil())
						right = stov(stream->next().name);
					else {
						badSymbol(stream);
						return false;
					}
				}
				else if (stream->match(tokens::string)) {
					hasOP = false;
					if (left.isNil())
						left = value(stream->next().name,string);
					else if (right.isNil())
						right = value(stream->next().name, string);
					else {
						badSymbol(stream);
						return false;
					}
				}
				else if (stream->match(tokens::name, tokens::bracketo)) {
				value tmpvalue = value(datatypes::variable);
				if (match_process_index(stream, tmpvalue)) {
					hasOP = false;
					if (left.isNil())
						left = tmpvalue;
					else if (right.isNil())
						right = tmpvalue;
					else {
						badSymbol(stream);
						return false;
					}
				}
				}
				else if (stream->match(tokens::name)) {
					// We tested functions already! So if that fails and we have a name then... we have a variable lets handle this!
					hasOP = false;
					if (left.isNil())
						left = value(stream->next().name,datatypes::variable);
					else if (right.isNil())
						right = value(stream->next().name,datatypes::variable);
					else {
						badSymbol(stream);
						return false;
					}
				}
				else if ((stream->match(tokens::newline) && !hasOP) || stream->match(tokens::parac) || stream->match(tokens::seperator)) {
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
			return stream->restore(lastcmd, current_chunk); // Always return false and restores the position in stream!
		}
		else {
			return stream->restore(lastcmd, current_chunk); // Always return false and restores the position in stream!
		}
	}
}