#include "dms.h"
//#include "memory.h"
//#include <windows.h>
//#include "utils.h"
#include <iostream>
#include "value.h"
#include <chrono>
using namespace dms;
using namespace utils;
//typedef void(*FNPTR)();
using namespace std::chrono;
value invokeTest(void* self, dms_state* state, dms_args* args) {
    utils::print(args->args[0].getPrintable());
    return "I work!";
}
value print(void* self, dms_state* state, dms_args* args) {
    for (size_t i = 0; i < args->size()-1; i++)
        std::cout << args->args[i].getPrintable() << '\t';
    std::cout << std::endl;
    return value();
}
int main()
{
    /*milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    );
    utils::print(ms.count());*/
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    state->invoker.registerFunction("invokeTest", invokeTest);
    state->invoker.registerFunction("print", print);
    state->dump();
    state->run();

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