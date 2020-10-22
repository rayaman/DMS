#include "dms.h"
//#include <windows.h>
#include <iostream>
using namespace dms;
//typedef void(*FNPTR)();

value* invokeTest(void* self, dms_state* state, dms_args* args) {
    utils::print(args->args[0]->getPrintable());
    return buildValue("I work!");
}
int main()
{
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->invoker.registerFunction("invokeTest", invokeTest);
    state->dump();
    state->run();
    utils::print("Exitcode: ",state->exitcode);


    /*HINSTANCE hInst = LoadLibrary(L"C:\\Users\\rayam\\Desktop\\test.dll");
    if (!hInst) {
        std::cout << "\nCould not load the library!";
        return EXIT_FAILURE;
    }

    FNPTR fn = (FNPTR)GetProcAddress(hInst, "_init");
    if (!fn) {
        std::cout << "\nCould not load function from library!";
        return EXIT_FAILURE;
    }

    fn();*/
}