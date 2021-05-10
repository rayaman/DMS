#pragma once
#include "pch.h"
#include "core.h"
#include "utils.h"
namespace dms {
    namespace core {
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
                value temp = args->args[0];
                if (temp.type == datatypes::custom && temp.ctype != "")
                    return temp.ctype;
                else
                    return datatype[args->args[0].type];
            }
            return "nil";
        }
        value concat(void* self, dms_state* state, dms_args* args) {
            return dms::value();
        }
        void init(dms_state* state) {
            state->invoker.registerFunction("print", print);
            state->invoker.registerFunction("type", type);
            //state->invoker.registerFunction("concat", concat);
        }
    }
}