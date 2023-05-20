#include <iostream>
#include "exceptions.hpp"


class ShowOffClass {
    std::string name;

public:
    ShowOffClass(): name("__default__") {
        std::cout << "Called default constructor of ShowOffClass" << std::endl;
    }

    ShowOffClass(const std::string& name): name(name) {
        std::cout << "Called constructor of " << name << std::endl;
    }

    ~ShowOffClass() {
        std::cout << "Called destructor of " << name << std::endl;
    }
};

void inner_function(ErrorType to_throw) {
    std:: cout << " ==================== INNER FUNC =====================" << std::endl;
    CONSTRUCT(ShowOffClass, inner_outside, "Inner variable outside TRY");

    TRY_BEGIN {
        CONSTRUCT(ShowOffClass, inner_inside, "Inner variable inside TRY");

        std::cout << "Before THROW error with code " << to_throw << std::endl;
        THROW(to_throw);
    } CATCH (MathError) {
        std::cout << "Successfully caught MathError" << std::endl;
    }
    TRY_END
    std::cout << "Variable in try is cleared" << std::endl;

    std::cout << "Finished inner function" << std::endl;
}

REGISTER_ERROR(NewError);

int main() {
    std:: cout << " ==================== START TEST =====================" << std::endl;
    std:: cout << " ==================== TRY-CATCH 1 ====================" << std::endl;
    
    CONSTRUCT(ShowOffClass, outer, "Outer variable outside TRY");
    TRY_BEGIN {
        std::cout << "In outer TRY" << std::endl;
        
        CONSTRUCT(ShowOffClass, outer, "Outer variable inside TRY");

        std:: cout << " ================= GONNA FINISH THIS =================" << std::endl;
        inner_function(MathError);
        std:: cout << " ===================== FINISHED ======================" << std::endl;
        
        std:: cout << " =============== NOT GONNA FINISH THIS ===============" << std::endl;
        inner_function(NewError);
        std:: cout << " ===================== UNREACHABLE ===================" << std::endl;
    } CATCH (NewError) {
        std:: cout << " ================== CAUGHT EXCEPTION =================" << std::endl;
        std::cout << "Successfully caught NewError" << std::endl;
    }
    TRY_END
    std:: cout << " =================== End TRY-CATCH ===================" << std::endl;
    
    std:: cout << " ===================== END TEST ======================" << std::endl;
}