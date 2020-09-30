#include "Handlers.h"
namespace dms {
	uint8_t Handler::handleChoice(dms_state* state, std::string prompt, std::vector<std::string> args) const {
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
			return handleChoice(state,prompt,args);
		}
	}
	bool Handler::handleSpeaker(dms_state* state, character* speaker) const {
		if (speaker == nullptr)
			return false; // Something went wrong and we pushed the error!
		if (speaker->seen) {
			if (speaker->lname != "") {
				utils::write(speaker->fname, " ", speaker->lname, ": ");
			}
			else {
				utils::write(speaker->fname, ": ");
			}
		}
		else {
			utils::write(speaker->unknown, ": ");
			speaker->seen = true;
		}
	}

	// Simple one conn event
	bool Handler::OnSpeakerCreated(dms_state* state, character* chara) const {
		return true;
	}
}