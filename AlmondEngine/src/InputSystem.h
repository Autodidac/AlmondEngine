#pragma once

#include <unordered_map>
#include <cstdint>
//#include <WinUser.h>
//#include "framework.h"

namespace almond {
    namespace userinput {

        // Enum representing the state of a key or button
        enum class KeyState {
            Released,
            Pressed,
            Held,
        };

        // Enum for mouse buttons
        enum class MouseButton {
            Left,
            Right,
            Middle,
            Button4,
            Button5
        };

        // Enum for gamepad buttons
        enum class GamepadButton {
            A,
            B,
            X,
            Y,
            LB,          // Left Bumper
            RB,          // Right Bumper
            Select,
            Start,
            LeftStick,
            RightStick,
            DPadUp,
            DPadDown,
            DPadLeft,
            DPadRight,
        };

        class InputSystem {
        public:
            // Default constructor and destructor
            InputSystem() = default;
            ~InputSystem() = default;

            // Update the input states
            void update() {
                // Update key states from Pressed to Held
                for (auto& [key, state] : keyStates) {
                    if (state == KeyState::Pressed) {
                        state = KeyState::Held;
                    }
                }

                // Update mouse button states from Pressed to Held
                for (auto& [button, state] : mouseButtonStates) {
                    if (state == KeyState::Pressed) {
                        state = KeyState::Held;
                    }
                }

                // Update gamepad button states from Pressed to Held
                for (auto& [button, state] : gamepadButtonStates) {
                    if (state == KeyState::Pressed) {
                        state = KeyState::Held;
                    }
                }
            }

            // Keyboard input management
            void keyPressed(int key) {
                keyStates[key] = KeyState::Pressed;
            }

            void keyReleased(int key) {
                keyStates[key] = KeyState::Released;
            }

            // Mouse input management
            void mouseButtonPressed(MouseButton button) {
                mouseButtonStates[button] = KeyState::Pressed;
            }

            void mouseButtonReleased(MouseButton button) {
                mouseButtonStates[button] = KeyState::Released;
            }

            // Gamepad input management
            void gamepadButtonPressed(GamepadButton button) {
                gamepadButtonStates[button] = KeyState::Pressed;
            }

            void gamepadButtonReleased(GamepadButton button) {
                gamepadButtonStates[button] = KeyState::Released;
            }

            // Query functions
            bool isKeyPressed(int key) const {
                auto it = keyStates.find(key);
                return it != keyStates.end() && it->second == KeyState::Pressed;
            }

            bool isKeyHeld(int key) const {
                auto it = keyStates.find(key);
                return it != keyStates.end() && it->second == KeyState::Held;
            }

            bool isMouseButtonPressed(MouseButton button) const {
                auto it = mouseButtonStates.find(button);
                return it != mouseButtonStates.end() && it->second == KeyState::Pressed;
            }

            bool isMouseButtonHeld(MouseButton button) const {
                auto it = mouseButtonStates.find(button);
                return it != mouseButtonStates.end() && it->second == KeyState::Held;
            }

            bool isGamepadButtonPressed(GamepadButton button) const {
                auto it = gamepadButtonStates.find(button);
                return it != gamepadButtonStates.end() && it->second == KeyState::Pressed;
            }

            bool isGamepadButtonHeld(GamepadButton button) const {
                auto it = gamepadButtonStates.find(button);
                return it != gamepadButtonStates.end() && it->second == KeyState::Held;
            }

        private:
            // Maps to track the state of keys, mouse buttons, and gamepad buttons
            std::unordered_map<int, KeyState> keyStates;
            std::unordered_map<MouseButton, KeyState> mouseButtonStates;
            std::unordered_map<GamepadButton, KeyState> gamepadButtonStates;
        };

    } // namespace userinput
} // namespace almond
