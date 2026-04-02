// InputSystem.h
#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>
#include <vector>

class EventBus;

class InputSystem {
public:
    bool loadFromJSON(const std::string& path);
    void processEvent(const SDL_Event& event, EventBus& bus) const;

    // --- Rebinding API (used by the future settings screen) ---

    // Returns all current bindings as (action, keycode) pairs.
    // One action may appear multiple times (e.g. MoveUp for both Up and W).
    std::vector<std::pair<std::string, SDL_Keycode>> getBindings() const;

    // Replace every key currently bound to `action` with `newKey`.
    // Passing SDLK_UNKNOWN clears the action entirely.
    void rebind(const std::string& action, SDL_Keycode newKey);

    // Write current bindings back to the JSON file they were loaded from.
    // Call this after rebind() so the choice persists across sessions.
    void saveToJSON() const;

private:
    std::unordered_map<SDL_Keycode, std::string> m_bindings;
    std::string m_jsonPath; // remembered so saveToJSON needs no argument

    void applyDefaults();
};