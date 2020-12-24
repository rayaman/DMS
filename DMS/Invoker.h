#pragma once
#include "pch.h"
#include "errors.h"
namespace dms {
    struct dms_state;
    class Invoker {
        std::unordered_map<std::string, value (*)(void*, dms_state*, dms_args*)>funcs;
        void* self = nullptr;
    public:
        bool preventOverwriting = true;
        bool registerFunction(std::string str, value (*f)(void*, dms_state*, dms_args*));
        bool registerFunction(std::string str, value (*f)(void*, dms_state*, dms_args*), bool preventoverride);
        void _init(void* ref);
        value Invoke(std::string str, dms_state* state, dms_args* args);
        value Invoke(std::string str, void* ref, dms_state* state, dms_args* args);
        // Exports the methods from an Invoker object
        std::unordered_map<std::string, value (*)(void*, dms_state*, dms_args*)> Export();
        // Imports methods from another Invoker, this will add and overwrite any method with the same name
        void Import(std::unordered_map<std::string, value (*)(void*, dms_state*, dms_args*)> tempf);
    };
}
