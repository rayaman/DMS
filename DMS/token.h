#pragma once
#include <string>
namespace dms::tokens {
	enum tokentype {
		flag,
		name,
		number,
		boolean,
		string,
		parao,
		parac,
	};
	struct token {
		tokentype type;
		std::string raw;
	};
}