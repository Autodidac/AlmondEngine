
#include "EntryPoint.h"
#include "EntryPoint_X11.h"

#ifdef __linux__
    int main() {
        almond::EntryPoint entryPoint(1280, 720, "Almond Shell - Linux");
        entryPoint.show();
        entryPoint.pollEvents();
        return 0;
    }
#endif