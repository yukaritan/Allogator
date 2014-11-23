#include <stddef.h>
#include <iostream>
#include "allogator.h"

int main ()
{
    /**
      * TODO: Find a way to inject objects between other objects to make better use of deallocated memory
      *
      */

    auto allogator = Allogator(1024);
    auto number = allogator.Allocate<int>();

    *number = 128;

    std::cout << "Original value" << std::endl;
    std::cout << number << " => " << *number << std::endl;

    std::cout << " ! Begin transaction" << std::endl;
    allogator.BeginTransaction();

    *number = 512;
    std::cout << "Current value" << std::endl;
    std::cout << number << " => " << *number << std::endl;

    std::cout << " ! Rollback" << std::endl;
    allogator.RollBackTransaction();



    std::cout << "The value is now" << std::endl;
    std::cout << number << " => " << *number << std::endl;



    std::cout << " ! Begin transaction" << std::endl;
    allogator.BeginTransaction();

    *number = 1024;
    std::cout << "Current value" << std::endl;
    std::cout << number << " => " << *number << std::endl;

    std::cout << " ! Commit" << std::endl;
    allogator.CommitTransaction();

    std::cout << "The value is now" << std::endl;
    std::cout << number << " => " << *number << std::endl;

    allogator.Deallocate(number);

    number = nullptr;

    return 0;
}