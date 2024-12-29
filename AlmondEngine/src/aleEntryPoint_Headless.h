#pragma once 

#include "aleEntryPoint.h"
#include "aleUtilities.h"
//#include "aleExports_DLL.h"

#ifndef __linux__

namespace almond {
    class HeadlessEntryPoint : public EntryPoint {
    public:
       // void createEntryPoint(int width, int height, const wchar_t* title) override {}

        //void show() override {}

       // bool pollEvents() override {
      //      return false;
      //  }
    };
} // namespace almond

#endif