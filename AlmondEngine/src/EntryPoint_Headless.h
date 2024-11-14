#pragma once 

#include "EntryPoint_Crossplatform.h"
#include "Utilities.h"
#include "Exports_DLL.h"

namespace almond {
    class ENTRYPOINTLIBRARY_API HeadlessEntryPoint : public EntryPoint_Crossplatform {
    public:
        void createEntryPoint(int width, int height, const wchar_t* title) override {}

        void show() override {}

        bool pollEvents() override {
            return false;
        }
    };
} // namespace almond
