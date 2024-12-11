#pragma once

#include "Ball.h"
#include "Map.h"
#include <Arduboy2.h>

class CollisionHandler
{
    CollisionHandler() = delete; // enforce this to be a static class

public:
    static void HandleCollisions(Ball &ball, const Map &map)
    {
        for (auto wall : map.walls)
        {
            if (IsColliding(ball, wall))
                HandleCollision(ball, wall);
        }

        for (auto circle : map.circles)
        {
            HandleCollision(ball, circle);
        }
    }

    static bool BallInHole(Ball &ball, const Map &map)
    {
        float dist = Distance(ball.X, ball.Y, map.end.x, map.end.y);
        return dist <= Map::HoleRadius;
    }

private:
    static float Distance(float x1, float y1, float x2, float y2)
    {
        return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }

    static bool IsColliding(const Ball &ball, const Wall &wall)
    {
        // Wall vector
        float dx = static_cast<float>(wall.p2.x - wall.p1.x);
        float dy = static_cast<float>(wall.p2.y - wall.p1.y);

        // Vector from p1 to the ball's center
        float fx = ball.X - static_cast<float>(wall.p1.x);
        float fy = ball.Y - static_cast<float>(wall.p1.y);

        // Project the ball's center onto the line defined by p1 and p2
        float lenSquared = dx * dx + dy * dy;
        float t = (fx * dx + fy * dy) / lenSquared;

        // Clamp t to the range [0, 1] to restrict to the segment
        t = constrain(t, 0.0, 1.0);

        // Find the closest point on the wall segment
        float closestX = wall.p1.x + t * dx;
        float closestY = wall.p1.y + t * dy;

        // Calculate the distance from the ball's center to the closest point
        float distanceToWall = Distance(ball.X, ball.Y, closestX, closestY);

        // Check if the distance is less than or equal to the radius
        return distanceToWall <= Ball::Radius;
    }

    static void HandleCollision(Ball &ball, const Wall &wall)
    {
        // Wall direction vector
        Vector wallDir = {wall.p2.x - wall.p1.x, wall.p2.y - wall.p1.y};

        // Wall normal vector (perpendicular to wall direction)
        Vector wallNormal = {-wallDir.y, wallDir.x};
        wallNormal = wallNormal.Normalize();

        // Reflect the ball's velocity
        float dotProduct = ball.Velocity.DotProduct(wallNormal);
        ball.Velocity.x -= 2 * dotProduct * wallNormal.x;
        ball.Velocity.y -= 2 * dotProduct * wallNormal.y;
    }

    static void HandleCollision(Ball &ball, const Circle &circle) {
        Vector circleToBall = {ball.X - circle.location.x, ball.Y - circle.location.y};
        float distance = circleToBall.Length();

        if (distance <= circle.radius + Ball::Radius) {
            // Resolve the collision by moving the ball outside of the circle 
            Vector correction = circleToBall.Normalize();
            ball.X += correction.x;
            ball.Y += correction.y;

            // Reflect the ball's velocity
            Vector normal = circleToBall.Normalize();
            float dotProduct = ball.Velocity.x * normal.x + ball.Velocity.y * normal.y;
            ball.Velocity.x -= 2 * dotProduct * normal.x;
            ball.Velocity.y -= 2 * dotProduct * normal.y;
        }
    }
};