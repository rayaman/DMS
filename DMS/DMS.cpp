#include "dms.h"
//#include <windows.h>
#include <iostream>
using namespace dms;
typedef void(*FNPTR)();

int main()
{
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

    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->dump();
    state->run();
    utils::print("Exitcode: ",state->exitcode);

}