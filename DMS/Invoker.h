#pragma once
#include "errors.h"
#include <unordered_map>
namespace dms {
    struct dms_state;
    class Invoker {
        std::unordered_map<std::string, value* (*)(dms_state*, dms_args*)>funcs;
    public:
        bool preventOverwriting = true;
        bool registerFunction(std::string str, value* (*f)(dms_state*, dms_args*));
        bool registerFunction(std::string str, value* (*f)(dms_state*, dms_args*), bool preventoverride);
        value* Invoke(std::string str, dms_state* state, dms_args* args);
    };
}
