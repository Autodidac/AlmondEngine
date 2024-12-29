#pragma once
#include "aleIPlugin.h"

#include <concepts>
#include <type_traits>

namespace almond::plugin {

    // Concept to check if a class has Initialize() and Shutdown() methods.
    template <typename T>
    concept PluginConcept = requires(T t) {
        { t.Initialize() } -> std::same_as<void>;
        { t.Shutdown() } -> std::same_as<void>;
    };

    // Validated plugin ensures the type satisfies the PluginConcept
    template <PluginConcept T>
    class ValidatedPlugin : public IPlugin {
    public:
        ValidatedPlugin() = default;

        void Initialize() override {
            static_cast<T*>(this)->Initialize();
        }

        void Shutdown() override {
            static_cast<T*>(this)->Shutdown();
        }
    };

} // namespace almond::plugin
