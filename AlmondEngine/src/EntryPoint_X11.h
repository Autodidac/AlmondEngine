#pragma once

#ifdef __linux__

#include "EntryPoint_Crossplatform.h"
#include <X11/Xlib.h>

namespace almond {

    class X11EntryPoint : public EntryPoint_Crossplatform {
    public:
        void createEntryPoint(int width, int height, const char* title) override {
            display = XOpenDisplay(NULL);
            int screen = DefaultScreen(display);
            entryPoint = XCreateSimpleWindow(display, RootWindow(display, screen),
                10, 10, width, height, 1,
                BlackPixel(display, screen),
                WhitePixel(display, screen));

            XStoreName(display, entryPoint, title);
            XSelectInput(display, entryPoint, ExposureMask | KeyPressMask);
        }

        void show() override {
            XMapWindow(display, entryPoint);
            XFlush(display);
        }

        void pollEvents() override {
            while (XPending(display)) {
                XEvent event;
                XNextEvent(display, &event);
                if (event.type == Expose) {
                    // Handle redraw
                } else if (event.type == KeyPress) {
                    // Handle key press
                }
            }
        }

    private:
        Display* display;
        Window entryPoint;
    };
} // namespace almond

#endif // __linux__
