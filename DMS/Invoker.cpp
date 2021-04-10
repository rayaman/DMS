#include "pch.h"
#include "Invoker.h"
#include "dms_state.h"
#include "utils.h"
namespace dms {
    bool Invoker::registerFunction(std::string str, dms_func f) {
        if (preventOverwriting && funcs.count(str)) {
            return false;
        }
        funcs.insert_or_assign(str, f);
        return true;
    }
    bool Invoker::registerFunction(std::string str, dms_func f, bool preventoverride) {
        if (preventoverride && funcs.count(str)) {
            return false;
        }
        funcs.insert_or_assign(str, f);
        return true;
    }
    value Invoker::Invoke(std::string str, dms_state* state, dms_args* args) {
        if (funcs.count(str)) {
            if(args->size())
                for (int i = 0; i < args->args.size() - 1; i++)
                    args->args[i] = args->args[i].resolve(state);

            return funcs[str](self, state, args);
        }
        state->push_error(errors::error{ errors::non_existing_function, utils::concat("Attempt to call '",str,"' a nil value!") });
        return value(datatypes::error);
    }
    value Invoker::Invoke(std::string str,void* ref, dms_state* state, dms_args* args) {
        if (funcs.count(str)) {
            if (args->size())
                for (int i = 0; i < args->args.size() - 1; i++)
                    args->args[i] = args->args[i].resolve(state);

            return funcs[str](ref, state, args);
        }
        state->push_error(errors::error{ errors::non_existing_function, utils::concat("Attempt to call '",str,"' a nil value!") });
        return value(datatypes::error);
    }
    std::unordered_map<std::string, dms_func> Invoker::Export() {
        return funcs;
    }
    void Invoker::Import(std::unordered_map<std::string, dms_func> tempf) {
        for (auto const& x : tempf)
        {
            // Copy the contents of the imported invoker into
            funcs.insert_or_assign(x.first, x.second);
        }
    }
    void Invoker::_init(void* ref) {
        // You can only init once
        if (self != nullptr)
            return;
        self = ref;
    }
}