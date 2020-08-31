#include "LineParser.h"
using namespace dms::tokens;
using namespace dms::utils;
namespace dms {
	bool LineParser::match_process_disp(tokenstream* stream) {
		/*
			DISP, "msg"
			DISP, "msg" speaker

			Compound DISP
		*/
		if ((isBlock(bt_block) || isBlock(bt_method)) && stream->match(tokens::newline, tokens::string, tokens::newline)) {
			stream->next(); // Standard consumption
			std::string msg = stream->next().name;
			print("DISP := ", msg);
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(buildValue(msg));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			return true;
		}
		else if (isBlock(bt_block) && stream->match(tokens::newline, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			// We might have to handle scope here
			// Here we match 'Ryan: "This guy said this!"' Note the colon is needed!
			stream->next(); // Standard consumption
			std::string name = stream->next().name;
			stream->next(); // That colon
			std::string msg = stream->next().name;
			print("DISP := ", name, " says '", msg, "'");
			cmd* c = new cmd;
			c->opcode = codes::DISP;
			c->args.push(buildValue(msg));
			c->args.push(buildValue(name));
			current_chunk->addCmd(c); // Add the cmd to the current chunk
			// We might have to consume a newline... Depends on what's next
			return true;
		}
		// emotion: "path"
		// looks like a simple disp command
		else if (isBlock(bt_character) && stream->match(tokens::tab, tokens::name, tokens::colon, tokens::string, tokens::newline)) {
			return true;
		}

		// TODO: We still have to implement the compound disp

		return false;
	}
	bool LineParser::match_process_debug(tokenstream* stream) {
		if (stream->match(tokens::newline, tokens::debug, tokens::string) || stream->match(tokens::newline, tokens::debug, tokens::name)){
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
		token temp = stream->peek();
		if (temp.raw == codes::CHOI && stream->match(tokens::control,tokens::string)) {
			// Let's parse choice blocks.
			print(stream->peek());
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
					state->push_error(errors::error{ errors::choice_unknown,concat("Unexpected symbol ",stream->next()),true,stream->peek().line_num,current_chunk });
				}
				else if (stream->match(tokens::string)) {
					std::string name = stream->next().name;
					c->args.push(buildValue(name)); // We append the choice to the first part of the CHOI cmd

					// We consumed the option now lets do some matching, note that all of these are one liners in the bytecode!
					if (match_process_function(stream,nullptr,false)) { // No returns and also no nesting of functions!
						// We cannot have a nested function here, but if we dont have that then we add our goto
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
					state->push_error(errors::error{ errors::choice_unknown,concat("Unexpected symbol ",stream->next()),true,stream->peek().line_num,current_chunk });
				}
			}
			buildLabel(str);
			return true;
		}
		return false;
	}

	void cleanup(value* v) {
		v->nuke(); // Make sure we clean up the data
		delete[] v; // We didn't need it, lets clean it up!
	}
	bool LineParser::match_process_function(tokenstream* stream, value* v, bool nested) {
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
			tempstream.init(&t); // Turn tokens we consumed into a tokenstream
			value* tempval;
			token tok;
			// This part we add values to the opcodes for the bytecode FUNC val a1 a2 a3 ... an
			while (tempstream.peek().type != tokens::none) { // End of stream
				tempval = buildVariable();
				tok = tempstream.peek();
				if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
					cleanup(tempval); // We don't need it
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
				}
				else if (match_process_expression(&tempstream, tempval)) {
					// We have an expression and its been handled Whoo!!!
				}
				// Final match this could be a function it might also be an expression
				else if (match_process_function(&tempstream, tempval)) {
					if (!nested) {
						print("No nested!");
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
					return true;
				}
				else {
					cleanup(tempval); // Cleanup
					state->push_error(errors::error{ errors::badtoken,concat("Invalid symbol: ",tempstream.peek()),true, tempstream.peek().line_num,current_chunk });
				}
			}
		}
		return false;
	}
	bool LineParser::match_process_goto(tokenstream* stream) {
		if (stream->match(tokens::gotoo,tokens::name) || tokens::gotoo,tokens::string) {
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
	bool LineParser::match_process_expression(tokenstream* stream, value* v) {
		return false; // Method isn't done yet, we will get an error without this!
		// I will have to consume for this to work so we need to keep track of what was incase we return false!
		size_t start_pos = stream->pos;
		// It has to start with one of these 3 to even be considered an expression
		if (stream->match(tokens::number) || stream->match(tokens::name) || stream->match(tokens::parao)) {
			// What do we know, math expressions can only be on a single line. We know where to stop looking if we have to
			token t = stream->peek();
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

			/* lua code which was actually really compact and works!
			if not expr:match("[/%^%+%-%*%%]") then --[[print("No math to do!")]] return expr end
			-- handle pharanses
			expr=expr:gsub("([%W])(%-%d)",function(b,a)
				return b.."(0-"..a:match("%d+")..")"
			end)
			-- Work on functions
			expr = expr:gsub("([_%w]+)(%b())",function(func,args)
				local v = gen("$")
				local fnwr = {self.current_line[1],self.current_line[2],FWNR,self.current_line[4],v.." = "..func..args}
				self:parseFNWR(fnwr)
				return v
			end)
			expr = expr:gsub("(%b())",function(a)
				return self:parseExpr(a:sub(2,-2))
			end)
			return self:chop(expr)
			*/
			token left; // lefthand
			token op; // opperator
			token right; // righthand
			while (stream->peek().type != tokens::none) {
				if (t.type == tokens::parao) {
					tokenstream temp;
					temp.init(&(stream->next(tokens::parao, tokens::parac))); // Balanced match!
					return match_process_expression(&temp,buildVariable("Work On This")); // Return true is everything else checks out!
				}
				// We have a number
				else if (t.type == tokens::number) {

				}
			}
		}
		else {
			return stream->restore(start_pos); // Always return false and restores the position in stream!
		}
	}
}