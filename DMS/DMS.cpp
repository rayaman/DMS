#include "dms.h"
#include <iostream>
using namespace dms;
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->dump();
    state->run();
    utils::print("Exitcode: ",state->exitcode);
}