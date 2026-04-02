// EntityRegistry.h
#pragma once
#include "EntityID.h"
#include "ComponentTypes.h"
#include <vector>
#include <bitset>
#include <any>
#include <cassert>

class EntityRegistry {
public:
    // --- Entity lifecycle ---
    EntityID createEntity();
    void     destroyEntity(EntityID id);
    bool     isAlive(EntityID id) const;

    // Returns all currently alive EntityIDs (used for serialization in Phase 5)
    std::vector<EntityID> allAlive() const;

    // --- Component management ---

    template<typename T>
    T& addComponent(EntityID id, T value = T{}) {
        assert(isAlive(id) && "addComponent: entity is not alive");
        const size_t cid{ componentTypeID<T>() };
        ensurePool<T>(cid);
        auto& p{ pool<T>(cid) };
        if (p.size() <= id.index)
            p.resize(id.index + 1);
        p[id.index] = std::move(value);
        m_masks[id.index].set(cid);
        return p[id.index];
    }

    template<typename T>
    T& getComponent(EntityID id) {
        assert(hasComponent<T>(id) && "getComponent: component not present");
        return pool<T>(componentTypeID<T>())[id.index];
    }

    template<typename T>
    const T& getComponent(EntityID id) const {
        assert(hasComponent<T>(id) && "getComponent: component not present");
        return pool<T>(componentTypeID<T>())[id.index];
    }

    template<typename T>
    [[nodiscard]] bool hasComponent(EntityID id) const {
        if (!isAlive(id)) return false;
        const size_t cid{ componentTypeID<T>() };
        return cid < MAX_COMPONENTS && m_masks[id.index].test(cid);
    }

    template<typename T>
    void removeComponent(EntityID id) {
        if (hasComponent<T>(id))
            m_masks[id.index].reset(componentTypeID<T>());
    }

    // Iterate all alive entities that have every listed component.
    // Callback signature: void(EntityID, T1&, T2&, ...)
    template<typename... Ts, typename Fn>
    void forEach(Fn&& fn) {
        for (uint32_t i{ 0 }; i < static_cast<uint32_t>(m_alive.size()); ++i) {
            if (!m_alive[i]) continue;
            EntityID id{ i, m_generations[i] };
            if ((hasComponent<Ts>(id) && ...))
                fn(id, getComponent<Ts>(id)...);
        }
    }

private:
    std::vector<uint32_t>                    m_generations;
    std::vector<bool>                        m_alive;
    std::vector<uint32_t>                    m_freeList;
    std::vector<std::bitset<MAX_COMPONENTS>> m_masks;
    std::vector<std::any>                    m_pools; // index = component type ID

    template<typename T>
    std::vector<T>& pool(size_t cid) {
        assert(cid < m_pools.size() && m_pools[cid].has_value());
        return *std::any_cast<std::vector<T>>(&m_pools[cid]);
    }

    template<typename T>
    const std::vector<T>& pool(size_t cid) const {
        assert(cid < m_pools.size() && m_pools[cid].has_value());
        return *std::any_cast<std::vector<T>>(&m_pools[cid]);
    }

    template<typename T>
    void ensurePool(size_t cid) {
        if (m_pools.size() <= cid)
            m_pools.resize(cid + 1);
        if (!m_pools[cid].has_value())
            m_pools[cid] = std::vector<T>{};
    }
};