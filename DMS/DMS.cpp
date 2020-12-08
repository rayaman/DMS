#include "dms.h"
#include "memory.h"
#include <windows.h>
#include "utils.h"
#include <iostream>
#include "value.h"
#include "enviroment.h"
//#include <chrono>
using namespace dms;
//typedef void(*FNPTR)();
//using namespace std::chrono;
//value invokeTest(void* self, dms_state* state, dms_args* args) {
//    utils::print(args->args[0].getPrintable());
//    return "I work!";
//}
value print(void* self, dms_state* state, dms_args* args) {
    std::string str = "";
    for (size_t i = 0; i < args->args.size(); i++) {
        str += args->args[i].getPrintable() + "\t";
    }
    printf((str + "\n").c_str());
    return NULL;
}
value type(void* self, dms_state* state, dms_args* args) {
    if (args->size() > 0) {
        return datatype[args->args[0].type];
    }
    return "nil";
}
//value concat(void* self, dms_state* state, dms_args* args) {
//    std::stringstream str;
//    for (size_t i = 0; i < args->size() - 1; i++)
//        str << args->args[i].getPrintable();
//    return value(str.str());
//}
int main()
{
    /*milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    );
    utils::print(ms.count());*/
    enviroment* envio = new enviroment;
    LineParser parser = LineParser("test.dms");
    dms_state* state = parser.Parse();
    envio->registerFunction("print", print);
    state->invoker.registerFunction("print", print);
    state->injectEnv("io",envio);
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