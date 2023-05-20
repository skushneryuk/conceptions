#include <vector>
#include <csetjmp>
#include <memory>
#include <functional>
#include <iostream>

using ErrorType = int;
// Define the global variable for the latest error code
ErrorType LAST_REGISTERED = -1;

// Define the macro to register a new error code
#define REGISTER_ERROR(ErrorCode) \
    ErrorType ErrorCode = ++LAST_REGISTERED;

// Example usage of the macro
REGISTER_ERROR(Ok)
REGISTER_ERROR(MathError)
REGISTER_ERROR(IOError)

class Context {
    std::jmp_buf env;
    std::vector<std::function<void()>> destructors;

public:
    std::jmp_buf& GetJmpBuf() {
        return env;
    }

    void ClearDestructors() {
        destructors.clear();
    }

    void ClearVariables() {
        for (auto destructor_call : destructors) {
            destructor_call();
        }
        ClearDestructors();
    }

    template<typename T>
    void Follow(std::shared_ptr<T>&& var) {
        destructors.emplace_back([var = std::move(var)]() mutable {});
    }

    ~Context() {
        ClearVariables();
    }
};

class ExceptionHandler {
    std::vector<Context> contexts;

public:
    ExceptionHandler() {
        contexts.emplace_back();
        ErrorType error = setjmp(LastJmpBuf());
        if (error != Ok) {
            std::cerr << "ERROR: Uncaught exception with code " << error << std::endl;
            exit(1);
        }
    }

    void InitContext() {
        contexts.emplace_back();
    }

    std::jmp_buf& LastJmpBuf() {
        return contexts.back().GetJmpBuf();
    }

    void PopBack() {
        contexts.pop_back();
    }

    template<typename T>
    void Follow(std::shared_ptr<T>&& var) {
        contexts.back().Follow(std::move(var));
    }

    void ClearContexts() {
        while (!contexts.empty()) {
            PopBack();
        }
    }

    void Throw(ErrorType error_code) {
        if (error_code != Ok) { // avoiding cycles
            std::longjmp(LastJmpBuf(), error_code);
        }
    }
};

ExceptionHandler GLOBAL_EXCEPTION_HANDLER;

#define TRY_BEGIN \
    GLOBAL_EXCEPTION_HANDLER.InitContext(); \
    int _CURRENT_STATUS = setjmp(GLOBAL_EXCEPTION_HANDLER.LastJmpBuf()); \
    if (_CURRENT_STATUS == Ok)

#define CATCH(ErrorCode) \
    else if (_CURRENT_STATUS == ErrorCode)

#define THROW(ErrorCode) \
    GLOBAL_EXCEPTION_HANDLER.Throw(ErrorCode);

#define TRY_END \
    else { GLOBAL_EXCEPTION_HANDLER.PopBack(); THROW(_CURRENT_STATUS); }\
    GLOBAL_EXCEPTION_HANDLER.PopBack();

#define CONSTRUCT(Type, variable, ...) \
    auto _SPECIAL_VAR_PTR = std::make_shared<Type>(__VA_ARGS__); \
    Type& variable = *_SPECIAL_VAR_PTR; \
    GLOBAL_EXCEPTION_HANDLER.Follow(std::move(_SPECIAL_VAR_PTR));
