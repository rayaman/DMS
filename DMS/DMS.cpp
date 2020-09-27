#include "dms.h"
#include "Handlers.h"
#include "utils.h"
#include <iostream>
using namespace dms;
// You can use your own choice handler!
class myHandler : public Handler {
    uint8_t manageChoice(dms_state* state, std::string prompt, std::vector<std::string> args) const override {
        std::string pos;
        for (size_t i = 0; i < args.size(); i++)
            std::cout << i << ": " << args[i] << std::endl;
        std::cout << prompt << " ";
        std::cin >> pos;
        return std::stoi(pos) - 1;
    }
};
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    try {
        //state->setHandler(new myHandler);
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
        return -1;
    }
    state->dump();
    state->run();
}