#include "Handlers.h"
namespace dms {
	uint8_t choiceHandler::manageChoice(dms_state* state, dms_args choices) const {
		uint8_t count = choices.args.size();
		std::string pos;
		std::string prompt = choices.args[0]->s->getValue();
		for (size_t i = 1; i < count; i++)
			std::cout << i << ": " << choices.args[i]->s->getValue() << std::endl;
		std::cout << prompt << " ";
		std::cin >> pos;
		return std::stoi(pos)-1;
	}
}