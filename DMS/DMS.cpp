#include "dms.h"
using namespace dms;
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->dump();
}