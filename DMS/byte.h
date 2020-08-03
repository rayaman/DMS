#pragma once
#include <cstdint>
#include <iostream>
#include <string>
namespace dms {
	class byte
	{
	private:
		uint8_t data;

	public:
		friend std::ostream& operator << (std::ostream& out, const byte& c) {
			out << c.data;
			return out;
		};
		byte(int value);
		byte(char value);
		byte();
		std::string g() const;
		int g(std::string type) const;
		byte& operator=(const int& rhs);
		byte& operator=(const char& rhs);
		bool operator==(const byte rhs) const;
	};
}
