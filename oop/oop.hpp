#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>

// typedef void (*MethodType)();

// Define the map type
struct FunctionTable {
    using FunctionMap = std::unordered_map<std::string, std::function<void()>>;
    FunctionMap funcs;
    FunctionTable* parent_table;
};

// Declare the map for each class
template <typename Class>
FunctionTable& GetFunctionTable() {
    static FunctionTable table;
    return table;
}

template <typename Class>
FunctionTable* GetObjectsFunctionTable(Class& obj) {
    return obj.func_table_ptr;
}

template <typename Class>
FunctionTable* GetObjectsFunctionTable(Class* obj_ptr) {
    return obj_ptr->func_table_ptr;
}

#define VIRTUAL_BEGIN(ClassName) \
    struct ClassName { \
        FunctionTable* func_table_ptr;

#define VIRTUAL_END(ClassName) \
        ClassName() { \
            func_table_ptr = &GetFunctionTable<ClassName>(); \
        } \
    }; \
    namespace { \
        struct ParentTableInitializer_##ClassName { \
            ParentTableInitializer_##ClassName() { \
                GetFunctionTable<ClassName>().parent_table = nullptr; \
            } \
        }; \
        ParentTableInitializer_##ClassName instance_##ClassName; \
    };

#define VIRTUAL_DERIVED_BEGIN(BaseClassName, DerivedClassName) \
    struct DerivedClassName : BaseClassName { \
        using ParentClass = BaseClassName; \

#define VIRTUAL_DERIVED_END(BaseClassName, DerivedClassName) \
        DerivedClassName() { \
            func_table_ptr = &GetFunctionTable<DerivedClassName>(); \
        } \
    }; \
    namespace { \
        struct ParentTableInitializer_##DerivedClassName { \
            ParentTableInitializer_##DerivedClassName() { \
                GetFunctionTable<DerivedClassName>().parent_table = &GetFunctionTable<BaseClassName>(); \
            } \
        }; \
        ParentTableInitializer_##DerivedClassName instance_##DerivedClassName; \
    };

#define REGISTER_CONSTRUCTOR_BEGIN(ClassName, ...) \
    ClassName::ClassName(__VA_ARGS__) { \
        func_table_ptr = &GetFunctionTable<ClassName>(); \

#define REGISTER_CONSTRUCTOR_END(ClassName, ...) \
    }

#define REGISTER_VIRTUAL_METHOD(ClassName, MethodName) \
    void _ ## ClassName ## _ ## MethodName ## _(); \
    namespace { \
        struct MethodInitializer_ ## ClassName ## _ ## MethodName ## _ { \
            MethodInitializer_ ## ClassName ## _ ## MethodName ## _() { \
                GetFunctionTable<ClassName>().funcs[#MethodName] = _ ## ClassName ## _ ## MethodName ## _; \
            } \
        }; \
        MethodInitializer_ ## ClassName ## _ ## MethodName ## _ instance_ ## ClassName ## _ ## MethodName ## _; \
    }; \
    void _ ## ClassName ## _ ## MethodName ## _()

#define CALL_VIRTUAL_METHOD(object, MethodName) \
    { \
        FunctionTable* table = (object)->func_table_ptr; \
        std::function<void()> method; \
        while (table != nullptr) { \
            if (table->funcs.find(#MethodName) != table->funcs.end()) {\
                method = table->funcs[#MethodName]; \
                break; \
            } \
            table = table->parent_table; \
        } \
        if (method) { \
            method();\
        } else {\
            throw std::runtime_error("Virtual method does not exist"); \
        } \
    }
