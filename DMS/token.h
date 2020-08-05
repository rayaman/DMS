#pragma once
#include <string>
#include "codes.h"
namespace dms::tokens {
	enum tokentype {
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
		bracket,
		control,
		True,
		False,
		Or,
		And,
		Not,
		For,
		label
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
		friend std::ostream& operator << (std::ostream& out, const token& c) {
			const std::string temp1[] = { 
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
				"bracket",
				"control",
				"true",
				"false",
				"or",
				"and",
				"not",
				"for",
				"label"
			};
			out << "Line <" << c.line_num << ">" << codes::list[c.raw] << " " << temp1[c.type]  << " \t\t " << c.name;
			return out;
		}
	};
}