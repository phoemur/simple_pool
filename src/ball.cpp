#include "ball.h"

#include <cmath>

Ball::Ball()
    : texture{},
      posData{0.0, 0.0, 0.0},
      movData{0.0, 0.0, 0.0, 0.0}
{

}

Ball::Ball(const std::string& path)
    : Ball()
{
    setTex(path);
}

void Ball::setTex(const std::string& path)
{
    texture.loadFromFile(path);
    posData.radius = texture.getWidth() / 2.0;
}

void Ball::setPos(double px, double py)
{
    posData.pos_x = px;
    posData.pos_y = py;
}

void Ball::setVel(double sx, double sy)
{
    movData.speed_x = sx;
    movData.speed_y = sy;
}

void Ball::move()
{
    if (is_movable)
    {
        // Update position
        posData.pos_x += movData.speed_x;
        posData.pos_y += movData.speed_y;

        // Update Speed
        movData.speed_x += movData.accel_x;
        movData.speed_y += movData.accel_y;

        // Apply friction
        movData.speed_x *= 0.995;
        movData.speed_y *= 0.995;

        // Check for complete stop
        if (std::abs(movData.speed_x) < 0.08 && std::abs(movData.speed_y) < 0.08)
        {
            movData.speed_x = 0.0;
            movData.speed_y = 0.0;
        }

        // Notify event
        notify(Event::SUBJECT_MOVED);
    }
}

void Ball::render()
{
    texture.render(static_cast<int>(posData.pos_x - posData.radius),
                   static_cast<int>(posData.pos_y - posData.radius));
}

bool Ball::is_moving() const noexcept
{
    return is_movable && (std::abs(movData.speed_x) > 0.0 ||
                          std::abs(movData.speed_y) > 0.0);
}
