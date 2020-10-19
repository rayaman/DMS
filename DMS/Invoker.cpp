#include "Invoker.h"
#include "dms_state.h"
namespace dms {
    bool Invoker::registerFunction(std::string str, value* (*f)(dms_state*, dms_args*)) {
        if (preventOverwriting && funcs.count(str)) {
            return false;
        }
        funcs.insert_or_assign(str, f);
        return true;
    }
    bool Invoker::registerFunction(std::string str, value* (*f)(dms_state*, dms_args*), bool preventoverride) {
        if (preventoverride && funcs.count(str)) {
            return false;
        }
        funcs.insert_or_assign(str, f);
        return true;
    }
    value* Invoker:: Invoke(std::string str, dms_state* state, dms_args* args) {
        if (funcs.count(str)) {
            for (int i = 0; i < args->args.size() - 1; i++) {
                args->args[i] = args->args[i]->resolve(state->memory);
            }
            return funcs[str](state, args);
        }
        state->push_error(errors::error{ errors::non_existing_function, "Attempt to call a nil value!" });
        return buildNil();
    }
}