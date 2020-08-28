#pragma once
#include <string>
#include "codes.h"
namespace dms::tokens {
	enum tokentype {
		none,
		noop,
		flag,
		name,
		number,
		string,
		parao,
		parac,
		bracketo,
		bracketc,
		seperator,
		dot,
		cbracketo,
		cbracketc,
		plus,
		minus,
		multiply,
		divide,
		pow,
		mod,
		equal,
		colon,
		control,
		True,
		False,
		Or,
		And,
		Not,
		For,
		label,
		newline,
		tab,
		eof,
		ret,
		gotoo,
		jump,
		exit,
		nil
	};//stream, t_vec, line, isNum, buffer
	struct token {
		tokentype type = noop;
		codes::op raw = codes::NOOP;
		std::string name="";
		size_t line_num=0;
		void build(tokentype tt, codes::op o) {
			type = tt;
			raw = o;
		}
		void build(tokentype tt, std::string s) {
			type = tt;
			name = s;
		}
		void build(tokentype tt, codes::op o, std::string s) {
			type = tt;
			raw = o;
			name = s;
		}
		friend std::ostream& operator << (std::ostream& out, const token& c) {
			const std::string temp1[] = { 
				"none",
				"noop",
				"flag",
				"name",
				"number",
				"string",
				"parao",
				"parac",
				"bracketo",
				"bracketc",
				"seperator",
				"dot",
				"cbracketo",
				"cbracketc",
				"plus",
				"minus",
				"multiply",
				"divide",
				"pow",
				"mod",
				"equal",
				"colon",
				"control",
				"true",
				"false",
				"or",
				"and",
				"not",
				"for",
				"label",
				"newline",
				"tab",
				"eof",
				"ret",
				"gotoo",
				"jump",
				"exit",
				"nil"
			};
			out << "Line <" << c.line_num << ">" << codes::list[c.raw] << " " << temp1[c.type]  << " \t\t " << c.name;
			return out;
		}
	};
}