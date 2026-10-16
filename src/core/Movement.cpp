#include "core/Movement.hpp"
#include <cmath>

namespace Core {

MovementResult computeMovementVelocity(const MovementInput& input, Direction currentFacing, float speed) {
    float dx = 0.0f;
    float dy = 0.0f;

    if (input.left && !input.right) dx = -1.0f;
    else if (input.right && !input.left) dx = 1.0f;

    if (input.up && !input.down) dy = -1.0f;
    else if (input.down && !input.up) dy = 1.0f;

    const bool isMoving = (dx != 0.0f || dy != 0.0f);

    Direction facing = currentFacing;
    if (isMoving) {
        if (dy > 0.0f) facing = Direction::Down;
        else if (dy < 0.0f) facing = Direction::Up;
        else if (dx > 0.0f) facing = Direction::Right;
        else if (dx < 0.0f) facing = Direction::Left;
    }

    Vec2 velocity(0.0f, 0.0f);
    if (isMoving) {
        if (dx != 0.0f && dy != 0.0f) {
            constexpr float SQRT1_2 = 0.7071067811865475f;
            velocity.x = dx * speed * SQRT1_2;
            velocity.y = dy * speed * SQRT1_2;
        } else {
            velocity.x = dx * speed;
            velocity.y = dy * speed;
        }
    }

    return { velocity, facing, isMoving };
}

} // namespace Core
