// EventBus.h
#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <any>
#include <SDL3/SDL.h>

class EventBus {
public:
    template<typename EventT>
    void subscribe(std::function<void(const EventT&)> handler) {
        m_handlers[typeid(EventT)].push_back(
            [h = std::move(handler)](const std::any& e) {
                h(std::any_cast<const EventT&>(e));
            }
        );
    }

    template<typename EventT>
    void publish(const EventT& event) {
#ifdef _DEBUG
        SDL_Log("[EventBus] %s", typeid(EventT).name());
#endif
        auto it = m_handlers.find(typeid(EventT));
        if (it == m_handlers.end()) return;
        std::any wrapped{ event };
        for (auto& fn : it->second)
            fn(wrapped);
    }

private:
    std::unordered_map<
        std::type_index,
        std::vector<std::function<void(const std::any&)>>
    > m_handlers;
};