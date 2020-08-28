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
		if (isBlock(bt_block) && stream->match(tokens::newline, tokens::string, tokens::newline)) {
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
		return false;
	}
	bool LineParser::match_process_choice(tokenstream* stream) {
		return false;
	}
	/// <summary>
	/// Recursively parse through function related tokens
	/// </summary>
	/// <param name="stream"></param>
	/// <param name="v"></param>
	/// <returns></returns>
	bool LineParser::match_process_function(tokenstream* stream, value* v) {
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
			print("FUNC ",n);
			c->args.push(buildValue(n)); // Set the func identifier as the first variable
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
			std::vector<token> t = stream->next(tokens::parao, tokens::parac);
			tempstream.init(&t);
			tokens::token tok;
			while (tempstream.peek().type != tokens::none) { // End of stream
				if (tempstream.match(tokens::string)) {
					// We have a string argument
				}
				else if (tempstream.match(tokens::number)) {
					// This is an interesting one This could be just a number, or an expression with functions n stuff
				}
				else if (match_process_expression(&tempstream)) {
					// We have an expression and its been handled Whoo!!!
				}
				// Final match this could be a function it might also be an expression
				else if (match_process_function(&tempstream, buildVariable(""))) {

				}
				else if (tempstream.match(tokens::seperator)) {
					// We have a seperator for function arguments
					tempstream.next(); // Consume it
				}
			}
			/*std::cout << "---Tokens---" << std::endl;
			for (size_t i = 0; i < t.size() - 1; i++) {
				std::cout << t[i] << std::endl;
			}*/
			current_chunk->addCmd(c); // We push this onto the chunk after all dependants if any have been handled
			wait();
		}
		return false;
	}
	bool LineParser::match_process_goto(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_jump(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_exit(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_label(tokenstream* stream) {
		return false;
	}
	bool LineParser::match_process_expression(tokenstream* stream) {
		// I will have to consume for this to work so we need to keep track of what was incase we return false!
		size_t start_pos = stream->pos;
		// It has to start with one of these 3 to even be considered an expression
		if (stream->match(tokens::number) || stream->match(tokens::name) || stream->match(tokens::parao)) {
			// What do we know, math expressions can only be on a single line. We know where to stop looking if we have to
			token t = stream->peek();
			if (t.type == tokens::parao) {
				tokenstream temp;
				temp.init(&(stream->next(tokens::parao, tokens::parac))); // Balanced match!
				return match_process_expression(&temp); // Return true is everything else checks out!
			}
			else if (t.type == tokens::number) {

			}
		}
		else {
			return stream->restore(start_pos); // Always return false and restores the position in stream!
		}
	}
	}
}