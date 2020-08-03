#include "byte.h"
dms::byte::byte() {
	data = 0;
}
dms::byte::byte(char value) {
	data = value;
}
dms::byte::byte(int value) {
	data = value;
}

dms::byte & dms::byte::operator=(const int &rhs) {
	byte{ rhs };
	return *this;
}
dms::byte & dms::byte::operator=(const char &rhs) {
	byte{ rhs };
	return *this;
}
std::string dms::byte::g() const {
	return "["+ std::to_string(data) +"]";
}
int dms::byte::g(std::string type) const {
	return data;
}
bool dms::byte::operator==(const byte rhs) const {
	return this->data == rhs.data;
}