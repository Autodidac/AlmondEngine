#pragma once

#include <coroutine>
#include <iostream>
#include <exception>

namespace almond {
    class Coroutine {
    public:
        struct promise_type {
            Coroutine get_return_object() {
                return Coroutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }

            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            void unhandled_exception() {
                exceptionPtr = std::current_exception();
            }

            void return_void() {}

            std::suspend_always yield_value(int value) {
                currentValue = value;
                return {};
            }

            int currentValue{};
            std::exception_ptr exceptionPtr;
        };

        using handle_type = std::coroutine_handle<promise_type>;

        Coroutine(handle_type h) : handle(h) {}

        // Move constructor and assignment
        Coroutine(Coroutine&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

        Coroutine& operator=(Coroutine&& other) noexcept {
            if (this != &other) {
                if (handle) handle.destroy();
                handle = std::exchange(other.handle, nullptr);
            }
            return *this;
        }

        ~Coroutine() { if (handle) handle.destroy(); }

        // Resume the coroutine
        bool resume() {
            if (handle && !handle.done()) {
                handle.resume();
                if (handle.promise().exceptionPtr) {
                    std::rethrow_exception(handle.promise().exceptionPtr);
                }
                return !handle.done();
            }
            return false;
        }

        int current_value() const {
            if (!handle || handle.done()) {
                throw std::runtime_error("Coroutine not in valid state to retrieve value.");
            }
            return handle.promise().currentValue;
        }

    private:
        handle_type handle;
    };
} // namespace almond

