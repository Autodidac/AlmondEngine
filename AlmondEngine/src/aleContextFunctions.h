#pragma once

#include <tuple>
#include <functional>
#include <string>

// Define function signatures for context operations
using ContextInitFunc = std::function<void()>;
using ContextProcessFunc = std::function<bool()>;
using ContextCleanupFunc = std::function<void()>;

// Define a tuple for function sets
using ContextFuncs = std::tuple<ContextInitFunc, ContextProcessFunc, ContextCleanupFunc>;

// Function to create a context based on user input
ContextFuncs createContextFuncs(const std::string& type);
