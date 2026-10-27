#pragma once

#include <cmath>
#include <string>

namespace Core {

enum class Direction {
    Down = 0,
    Up,
    Left,
    Right
};

inline const char* directionToString(Direction dir) {
    switch (dir) {
        case Direction::Down: return "Down";
        case Direction::Up: return "Up";
        case Direction::Left: return "Left";
        case Direction::Right: return "Right";
    }
    return "Unknown";
}

inline Direction oppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::Down: return Direction::Up;
        case Direction::Up: return Direction::Down;
        case Direction::Left: return Direction::Right;
        case Direction::Right: return Direction::Left;
    }
    return Direction::Down;
}

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    constexpr Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    constexpr Vec2 operator*(float s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const { return {x / s, y / s}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }

    constexpr bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2& o) const { return !(*this == o); }

    float length() const { return std::sqrt(x * x + y * y); }
    
    Vec2 normalized() const {
        float len = length();
        if (len > 0.0001f) {
            return {x / len, y / len};
        }
        return {0.0f, 0.0f};
    }

    static Vec2 fromDirection(Direction dir) {
        switch (dir) {
            case Direction::Down: return {0.0f, 1.0f};
            case Direction::Up: return {0.0f, -1.0f};
            case Direction::Left: return {-1.0f, 0.0f};
            case Direction::Right: return {1.0f, 0.0f};
        }
        return {0.0f, 0.0f};
    }
};

struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rect() = default;
    constexpr Rect(float x_, float y_, float w_, float h_)
        : x(x_), y(y_), width(w_), height(h_) {}

    constexpr float left() const { return x; }
    constexpr float right() const { return x + width; }
    constexpr float top() const { return y; }
    constexpr float bottom() const { return y + height; }

    constexpr bool overlaps(const Rect& o) const {
        return left() < o.right() && right() > o.left() &&
               top() < o.bottom() && bottom() > o.top();
    }

    constexpr bool contains(const Vec2& p) const {
        return p.x >= x && p.x <= x + width &&
               p.y >= y && p.y <= y + height;
    }
};

enum class GameState {
    Title,
    Overworld,
    Dungeon,
    Dialogue,
    GameOver,
    Victory
};

struct GameInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool confirmPressed = false;      // SPACE or ENTER
    bool interactPressed = false;     // E or ENTER
    bool attackPressed = false;       // SPACE or Z
    bool bowPressed = false;          // X
    bool newGamePressed = false;      // N
    bool backToTitlePressed = false;  // BACKSPACE
    bool debugDamagePressed = false;  // H
    bool debugHealPressed = false;    // J
};

} // namespace Core
