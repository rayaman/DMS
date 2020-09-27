#include "Handlers.h"
namespace dms {
	uint8_t Handler::manageChoice(dms_state* state, std::string prompt, std::vector<std::string> args) const {
		std::string pos;
		for (size_t i = 0; i < args.size(); i++)
			std::cout << i+1 << ": " << args[i] << std::endl;
		std::cout << prompt << " ";
		std::cin >> pos;
		try {
			if (std::stoi(pos) > 0 && std::stoi(pos) < args.size())
				return std::stoi(pos) - 1;
			else
				throw exceptions::InvalidChoice();
		}
		catch (std::exception e) {
			std::cout << "Invalid Choice!" << std::endl;
			return manageChoice(state,prompt,args);
		}
		
	}
}