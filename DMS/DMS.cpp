#include "dms.h"
#include "Handlers.h"
#include "utils.h"
#include <iostream>
using namespace dms;
// You can use your own choice handler!
class myChoi : public choiceHandler {
    uint8_t manageChoice(dms_state* state, dms_args choices) const override {
        uint8_t count = choices.args.size();
        uint8_t pos;
        std::string prompt = choices.args[0]->s->getValue();
        for (size_t i = 1; i < count; i++)
            std::cout << i << ": " << choices.args[i]->s->getValue() << std::endl;
        std::cout << prompt;
        std::cin >> pos;
        return pos;
    }
};
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    try {
        //state->setChoiceHandler(new myChoi);
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return -1;
    }
    state->dump();
    state->run();
}