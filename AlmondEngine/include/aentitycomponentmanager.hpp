/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
 // acomponentmanager.hpp
#pragma once

#include "aplatform.hpp"   // must always come first

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>

namespace almondnamespace::ecs
{
    /// The basic ID type
    using EntityID = std::size_t;

    /// Underlying storage:
    ///   map EntityID → ( map type_index → erased shared_ptr )
    using ComponentStorage =
        std::unordered_map<EntityID,
        std::unordered_map<std::type_index, std::shared_ptr<void>>>;

    /**
     * add_component
     *   - storage: your global ComponentStorage
     *   - entity:  the ID
     *   - comp:    the new component (by value or moveable)
     */
    template<typename T>
    inline void add_component(ComponentStorage& storage,
        EntityID entity,
        T comp)
    {
        storage[entity][std::type_index(typeid(T))]
            = std::make_shared<T>(std::move(comp));
    }

    /**
     * has_component
     *   - storage: your ComponentStorage
     *   - entity:  the ID
     * Returns true if entity has a T.
     */
    template<typename T>
    inline bool has_component(ComponentStorage const& storage,
        EntityID entity)
    {
        auto it = storage.find(entity);
        return it != storage.end()
            && it->second.count(std::type_index(typeid(T))) > 0;
    }

    /**
     * get_component
     *   - storage: your ComponentStorage
     *   - entity:  the ID
     * Returns a reference to the stored T. Asserts if missing.
     */
    template<typename T>
    inline T& get_component(ComponentStorage& storage,
        EntityID entity)
    {
        assert(has_component<T>(storage, entity) && "Component not found!");
        auto& ptr = storage[entity][std::type_index(typeid(T))];
        return *static_cast<T*>(ptr.get());
    }

    /**
     * remove_component
     *   - storage: your ComponentStorage
     *   - entity:  the ID
     * Erases the T for that entity.
     */
    template<typename T>
    inline void remove_component(ComponentStorage& storage,
        EntityID entity)
    {
        auto it = storage.find(entity);
        if (it != storage.end()) {
            it->second.erase(std::type_index(typeid(T)));
        }
    }

} // namespace almondnamespace::ecs
