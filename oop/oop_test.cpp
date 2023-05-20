#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include "oop.hpp"

VIRTUAL_BEGIN(MyBaseClass)
    int a;
VIRTUAL_END(MyBaseClass)

REGISTER_VIRTUAL_METHOD(MyBaseClass, Both) {
    std::cout << "MyBaseClass::Both called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(MyBaseClass, OnlyBase) {
    std::cout << "MyBaseClass::OnlyBase called." << std::endl;
}

VIRTUAL_DERIVED_BEGIN(MyBaseClass, MyDerivedClass)
    int b;
VIRTUAL_DERIVED_END(MyBaseClass, MyDerivedClass)

REGISTER_VIRTUAL_METHOD(MyDerivedClass, Both) {
    std::cout << "MyDerivedClass::Both called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(MyDerivedClass, OnlyDerived){
    std::cout << "MyDerivedClass::OnlyDerived called." << std::endl;
}

void test1() {
	MyBaseClass base;
	MyDerivedClass derived;
    MyBaseClass* reallyDerived = reinterpret_cast<MyBaseClass*>(&derived);

    CALL_VIRTUAL_METHOD(&base, Both);
	CALL_VIRTUAL_METHOD(reallyDerived, Both);
	CALL_VIRTUAL_METHOD(reallyDerived, OnlyBase);
	CALL_VIRTUAL_METHOD(reallyDerived, OnlyDerived);
    try {
        CALL_VIRTUAL_METHOD(&base, OnlyDerived);
    } catch (const std::exception& e) {
        std::cout << "Can't access OnlyDerived from base" << std::endl;
        std::cout << "Got error: " << e.what() << std::endl;
    }
}

VIRTUAL_BEGIN(ClassA)
    int a;
VIRTUAL_END(ClassA)

VIRTUAL_DERIVED_BEGIN(ClassA, ClassB)
    int b;
VIRTUAL_DERIVED_END(ClassA, ClassB)

VIRTUAL_DERIVED_BEGIN(ClassB, ClassC)
    int c;
VIRTUAL_DERIVED_END(ClassB, ClassC)

REGISTER_VIRTUAL_METHOD(ClassA, AllABC) {
    std::cout << "ClassA::AllABC called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassB, AllABC) {
    std::cout << "ClassB::AllABC called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassC, AllABC) {
    std::cout << "ClassC::AllABC called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassA, OnlyAB) {
    std::cout << "ClassA::OnlyAB called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassB, OnlyAB) {
    std::cout << "ClassB::OnlyAB called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassB, OnlyBC) {
    std::cout << "ClassB::OnlyBC called." << std::endl;
}

REGISTER_VIRTUAL_METHOD(ClassC, OnlyBC) {
    std::cout << "ClassC::OnlyBC called." << std::endl;
}

void test2() {
	ClassA a;
	ClassB b;
	ClassC c;
	
	// полиморфизм
    ClassB* reallyC = reinterpret_cast<ClassB*>(&c);
    ClassA* reallyB = reinterpret_cast<ClassA*>(&b);
    ClassA* another_reallyC = reinterpret_cast<ClassA*>(&c);

    CALL_VIRTUAL_METHOD(&a, AllABC);
    CALL_VIRTUAL_METHOD(&b, AllABC);
    CALL_VIRTUAL_METHOD(&c, AllABC);
    
    std::cout << std::endl << " === Same results here: ..." << std::endl;
    CALL_VIRTUAL_METHOD(another_reallyC, OnlyBC);
    CALL_VIRTUAL_METHOD(another_reallyC, OnlyAB);

    std::cout << " === And here" << std::endl;
    CALL_VIRTUAL_METHOD(reallyC, OnlyBC);
    CALL_VIRTUAL_METHOD(reallyC, OnlyAB);
    std::cout << " ================ " << std::endl;

    std::cout << " === Still can call OnlyBC" << std::endl;
    CALL_VIRTUAL_METHOD(reallyB, OnlyBC);
    std::cout << " ================ " << std::endl << std::endl;
    try {
        CALL_VIRTUAL_METHOD(&a, OnlyBC);
    } catch (const std::exception& e) {
        std::cout << "Can't access OnlyBC from ClassA" << std::endl;
        std::cout << "Got error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "=============== TEST 1 =================" << std::endl;
    test1();
    std::cout << std::endl;
    std::cout << "=============== TEST 2 =================" << std::endl;
    test2();
    std::cout << std::endl;
}