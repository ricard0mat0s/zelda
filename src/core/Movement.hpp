#pragma once

#include "core/types.hpp"

namespace Core {

struct MovementInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
};

struct MovementResult {
    Vec2 velocity;
    Direction facing = Direction::Down;
    bool isMoving = false;
};

MovementResult computeMovementVelocity(const MovementInput& input, Direction currentFacing, float speed);

} // namespace Core
