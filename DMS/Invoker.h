#pragma once
#include "pch.h"
#include "errors.h"
#include <functional>
namespace dms {
    typedef std::function<value(void*,dms_state*,dms_args*)> dms_func;
    struct dms_state;
    class Invoker {
        std::unordered_map<std::string, dms_func>funcs;
        void* self = nullptr;
    public:
        bool preventOverwriting = true;
        bool registerFunction(std::string str, dms_func);
        bool registerFunction(std::string str, dms_func, bool preventoverride);
        void _init(void* ref);
        value Invoke(std::string str, dms_state* state, dms_args* args);
        value Invoke(std::string str, void* ref, dms_state* state, dms_args* args);
        // Exports the methods from an Invoker object
        std::unordered_map<std::string, dms_func> Export();
        // Imports methods from another Invoker, this will add and overwrite any method with the same name
        void Import(std::unordered_map<std::string, dms_func> tempf);
    };
}
