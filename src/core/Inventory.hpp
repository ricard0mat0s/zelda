#pragma once

namespace Core {

class Inventory {
public:
    Inventory() = default;

    bool hasSword = false;
    bool hasBow = false;
    int keys = 0;

    void obtainSword() { hasSword = true; }
    void obtainBow() { hasBow = true; }

    void addKeys(int amount = 1) {
        if (amount > 0) keys += amount;
    }

    bool useKey() {
        if (keys > 0) {
            keys--;
            return true;
        }
        return false;
    }
};

} // namespace Core
