// EntityID.h
// An entity is identified by an index (slot in the pool) and a generation counter.
// When a slot is reused, the generation increments, making old EntityIDs invalid.
#pragma once
#include <cstdint>
#include <limits>

struct EntityID {
    uint32_t index{ 0 };
    uint32_t generation{ 0 };

    bool operator==(const EntityID& o) const noexcept {
        return index == o.index && generation == o.generation;
    }
    bool operator!=(const EntityID& o) const noexcept { return !(*this == o); }
};

inline constexpr EntityID NULL_ENTITY{ std::numeric_limits<uint32_t>::max(), 0 };