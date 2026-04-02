// InputSystem.cpp
#include "InputSystem.h"
#include "EventBus.h"
#include "Events.h"

#pragma warning(push, 0)
#include <nlohmann/json.hpp>
#pragma warning(pop)

#include <fstream>
#include <algorithm>

// ---------------------
// Key name <-> keycode
// ---------------------
static SDL_Keycode keyNameToKeycode(const std::string& name) {
    if (name == "Up")    return SDLK_UP;
    if (name == "Down")  return SDLK_DOWN;
    if (name == "Left")  return SDLK_LEFT;
    if (name == "Right") return SDLK_RIGHT;
    if (name == "W")     return SDLK_W;
    if (name == "A")     return SDLK_A;
    if (name == "S")     return SDLK_S;
    if (name == "D")     return SDLK_D;
    // Extend here when Phase 6 adds more bindable actions
    SDL_Log("[InputSystem] Unknown key name: '%s'", name.c_str());
    return SDLK_UNKNOWN;
}

static std::string keycodeToKeyName(SDL_Keycode code) {
    switch (code) {
    case SDLK_UP:    return "Up";
    case SDLK_DOWN:  return "Down";
    case SDLK_LEFT:  return "Left";
    case SDLK_RIGHT: return "Right";
    case SDLK_W:     return "W";
    case SDLK_A:     return "A";
    case SDLK_S:     return "S";
    case SDLK_D:     return "D";
    default:         return "Unknown";
    }
}

// ---------------------
// Load / defaults
// ---------------------
void InputSystem::applyDefaults() {
    m_bindings[SDLK_UP] = "MoveUp";
    m_bindings[SDLK_DOWN] = "MoveDown";
    m_bindings[SDLK_LEFT] = "MoveLeft";
    m_bindings[SDLK_RIGHT] = "MoveRight";
    m_bindings[SDLK_W] = "MoveUp";
    m_bindings[SDLK_S] = "MoveDown";
    m_bindings[SDLK_A] = "MoveLeft";
    m_bindings[SDLK_D] = "MoveRight";
}

bool InputSystem::loadFromJSON(const std::string& path) {
    m_jsonPath = path;

    std::ifstream f{ path };
    if (!f.is_open()) {
        SDL_Log("[InputSystem] Cannot open '%s'. Using defaults.", path.c_str());
        applyDefaults();
        return false;
    }
    try {
        nlohmann::json j{ nlohmann::json::parse(f) };
        for (auto& b : j.at("bindings")) {
            const std::string key{ b.at("key") };
            const std::string action{ b.at("action") };
            SDL_Keycode code{ keyNameToKeycode(key) };
            if (code != SDLK_UNKNOWN)
                m_bindings[code] = action;
        }
        SDL_Log("[InputSystem] Loaded %zu bindings from '%s'",
            m_bindings.size(), path.c_str());
        return true;
    }
    catch (const std::exception& e) {
        SDL_Log("[InputSystem] Parse error: %s. Using defaults.", e.what());
        m_bindings.clear();
        applyDefaults();
        return false;
    }
}

// ---------------------
// Process
// ---------------------
void InputSystem::processEvent(const SDL_Event& event, EventBus& bus) const {
    if (event.type != SDL_EVENT_KEY_DOWN) return;

    auto it{ m_bindings.find(event.key.key) };
    if (it == m_bindings.end()) return;

    const std::string& action{ it->second };
    if (action == "MoveUp")    bus.publish(MoveActionEvent{ 0, -1 });
    else if (action == "MoveDown")  bus.publish(MoveActionEvent{ 0,  1 });
    else if (action == "MoveLeft")  bus.publish(MoveActionEvent{ -1,  0 });
    else if (action == "MoveRight") bus.publish(MoveActionEvent{ 1,  0 });
}

// ---------------------
// Rebinding API
// ---------------------
std::vector<std::pair<std::string, SDL_Keycode>> InputSystem::getBindings() const {
    std::vector<std::pair<std::string, SDL_Keycode>> result{};
    result.reserve(m_bindings.size());
    for (const auto& [code, action] : m_bindings)
        result.emplace_back(action, code);
    return result;
}

void InputSystem::rebind(const std::string& action, SDL_Keycode newKey) {
    // Remove all existing keys bound to this action
    for (auto it{ m_bindings.begin() }; it != m_bindings.end(); ) {
        if (it->second == action)
            it = m_bindings.erase(it);
        else
            ++it;
    }
    // Assign the new key (SDLK_UNKNOWN = clear only, no new binding)
    if (newKey != SDLK_UNKNOWN)
        m_bindings[newKey] = action;
}

void InputSystem::saveToJSON() const {
    if (m_jsonPath.empty()) {
        SDL_Log("[InputSystem] saveToJSON: no path set (was loadFromJSON called?)");
        return;
    }
    nlohmann::json j{};
    auto& arr{ j["bindings"] = nlohmann::json::array() };
    for (const auto& [code, action] : m_bindings) {
        std::string keyName{ keycodeToKeyName(code) };
        if (keyName == "Unknown") continue; // skip keys we can't round-trip
        arr.push_back({ {"key", keyName}, {"action", action} });
    }
    std::ofstream f{ m_jsonPath };
    if (!f.is_open()) {
        SDL_Log("[InputSystem] saveToJSON: cannot write to '%s'", m_jsonPath.c_str());
        return;
    }
    f << j.dump(2);
    SDL_Log("[InputSystem] Saved %zu bindings to '%s'",
        m_bindings.size(), m_jsonPath.c_str());
}