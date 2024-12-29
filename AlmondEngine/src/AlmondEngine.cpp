 
// src/almondengine.cpp
#include "almondengine.h"

// This file exists entirely for cmake, I will have a think about how to integrate/manage this,
// renaming cmakehelper.cpp is one option, but I like everything to flow
// this would make a good place for public external functions also
// not sure yet have to decide...
// src/almondshell.cpp
#include <iostream>
/*
// Define the `main` function in AlmondShell
extern "C" int main() {
    std::cout << "AlmondEngine's main is running!" << std::endl;
    return 0;
}
*/

#ifdef __linux__
void placeholder_function() {
    // Temporary function for compilation
}
#endif