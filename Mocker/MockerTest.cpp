#include "Mocker.h"

#include <iostream>

void foo() {
    std::cout << "In foo." << std::endl;
}

void foo_mock() {
    std::cout << "In foo_mock." << std::endl;
}

typedef void (*functionPrototype)();

int main(int argc, char ** argv) {
    std::cout << "Calling foo without the mock" << std::endl;
    foo();
    Mocker<functionPrototype> * mock = new Mocker<functionPrototype>(foo, foo_mock);
    std::cout << "Calling foo with the mock." << std::endl;
    foo();
    delete mock;
    std::cout << "Calling foo after deleting the mock." << std::endl;
    foo();
}
