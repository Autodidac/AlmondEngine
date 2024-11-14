#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <cassert>
#include <memory>

namespace almond {
// Renaming Entity type alias to EntityID to avoid conflict
using EntityID = size_t;

class ComponentManager {
public:
    template<typename T>
    void addComponent(EntityID entity, T component);

    template<typename T>
    T& getComponent(EntityID entity);

private:
    std::unordered_map<EntityID, std::unordered_map<std::type_index, std::unique_ptr<void>>> components;
};

template<typename T>
void ComponentManager::addComponent(EntityID entity, T component) {
    components[entity][std::type_index(typeid(T))] = std::make_unique<T>(component);
}

template<typename T>
T& ComponentManager::getComponent(EntityID entity) {
    assert(components[entity].count(std::type_index(typeid(T))) > 0 && "Component not found!");
    return *static_cast<T*>(components[entity][std::type_index(typeid(T))].get());
}
} // namespace almond
