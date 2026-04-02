// EntityRegistry.cpp
#include "EntityRegistry.h"

EntityID EntityRegistry::createEntity() {
    if (!m_freeList.empty()) {
        uint32_t idx = m_freeList.back();
        m_freeList.pop_back();
        m_alive[idx] = true;
        return { idx, m_generations[idx] };
    }
    uint32_t idx = static_cast<uint32_t>(m_generations.size());
    m_generations.push_back(0);
    m_alive.push_back(true);
    m_masks.emplace_back();
    return { idx, 0 };
}

void EntityRegistry::destroyEntity(EntityID id) {
    if (!isAlive(id)) return;
    m_alive[id.index] = false;
    m_masks[id.index].reset();
    ++m_generations[id.index]; // invalidate all existing copies of this EntityID
    m_freeList.push_back(id.index);
}

bool EntityRegistry::isAlive(EntityID id) const {
    return id.index < m_generations.size()
        && m_alive[id.index]
        && m_generations[id.index] == id.generation;
}

std::vector<EntityID> EntityRegistry::allAlive() const {
    std::vector<EntityID> result;
    for (uint32_t i{ 0 }; i < static_cast<uint32_t>(m_alive.size()); ++i)
        if (m_alive[i])
            result.push_back({ i, m_generations[i] });
    return result;
}