#pragma once

#include "core/types.hpp"

namespace Core {

enum class ItemType {
    HeartRecovery,
    SmallKey,
    HeartContainer
};

struct ItemDrop {
    Vec2 pos;
    ItemType type = ItemType::HeartRecovery;
    bool active = true;
    float lifetime = 12.0f;

    Rect getHitbox() const {
        return Rect(pos.x + 2.0f, pos.y + 2.0f, 12.0f, 12.0f);
    }

    std::string getTextureName() const {
        switch (type) {
            case ItemType::HeartRecovery:  return "heart_full";
            case ItemType::SmallKey:       return "small_key";
            case ItemType::HeartContainer: return "heart_container";
        }
        return "heart_full";
    }

    void update(float dt) {
        if (!active) return;
        if (type == ItemType::SmallKey || type == ItemType::HeartContainer) return; // Progression items do not expire
        lifetime -= dt;
        if (lifetime <= 0.0f) {
            active = false;
        }
    }

    bool isBlinking() const {
        // Blink during last 3 seconds
        if (lifetime < 3.0f) {
            int cycle = static_cast<int>(lifetime * 8.0f);
            return (cycle % 2) == 0;
        }
        return false;
    }
};

} // namespace Core
