// ComponentTypes.h
// Assigns a unique integer ID to each component type at runtime (first use).
// IDs are stable within a single run; do not serialize them directly.
#pragma once
#include <cstddef>
#include <cassert>

inline constexpr size_t MAX_COMPONENTS{ 32 };

namespace internal {
    inline size_t g_componentCounter{ 0 };
}

template<typename T>
size_t componentTypeID() {
    static const size_t id{ internal::g_componentCounter++ };
    assert(id < MAX_COMPONENTS && "Exceeded MAX_COMPONENTS. Increase it in ComponentTypes.h.");
    return id;
}