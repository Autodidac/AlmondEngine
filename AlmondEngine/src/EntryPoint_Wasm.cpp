
//#include "pch.h"
#include "EntryPoint.h"
#include "EntryPoint_Wasm.h"

#if defined(__EMSCRIPTEN__)
    int main() {
        almond::EntryPoint entryPoint(1280, 720, L"Almond Shell - Web Assembly");
        entryPoint.show();
        return 0;
    }
#endif