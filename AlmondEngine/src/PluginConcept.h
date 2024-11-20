#pragma once
#include <type_traits>
#include <concepts>
#include "PluginManager.h"

namespace almond::plugin {
    template <typename T>
    concept PluginConcept = requires(T t) {
        { t.Initialize() } -> std::same_as<void>;
        { t.Shutdown() } -> std::same_as<void>;
    };

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
}
