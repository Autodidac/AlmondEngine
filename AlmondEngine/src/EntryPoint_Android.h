#pragma once

#ifdef __ANDROID__

#include "EntryPoint_Crossplatform.h"

#include <jni.h>
#include <android/native_activity.h>

namespace almond {
    class AndroidEntryPoint : public EntryPoint_Crossplatform {
    public:
        void createEntryPoint(int width, int height, const char* title) override {
            // NativeActivity automatically creates an entry point, no direct setup is needed
            // Initialization happens via native callbacks from the Java side
        }

        void show() override {
            // Android will handle showing and rendering the entry point
        }

        void pollEvents() override {
            // Polling events through the looper or native activity input queue
        }
    };

} // namespace almond

extern "C" JNIEXPORT void JNICALL
Java_com_yourcompany_YourActivity_onCreate(JNIEnv* env, jobject obj) {
    almond::EntryPoint entryPoint(1024, 768, L"Almond Engine - Android");
    entryPoint.show();
    entryPoint.pollEvents();
}

#endif // __ANDROID__
