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
		utils::write(speaker->getName(), ": ");
		return true;
	}

	bool Handler::handleMessageDisplay(dms_state* state, std::string msg) const {
		utils::write(msg);
		return true;
	}

	bool Handler::handleMessageAppend(dms_state* state, std::string msg) const {
		utils::write(msg);
		return true;
	}

	// Simple one conn event
	bool Handler::OnEnviromentCreated(dms_state* state, enviroment* env) const {
		return true;
	}
	bool Handler::OnSpeakerCreated(dms_state* state, character* chara) const {
		return true;
	}
	bool Handler::OnStateInit(dms_state* state) const {
		return true;
	}
}