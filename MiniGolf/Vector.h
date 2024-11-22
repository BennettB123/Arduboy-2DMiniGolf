#pragma once

struct Vector {
    float x, y;
    
    float DotProduct(const Vector& other) const {
        return x * other.x + y * other.y;
    }

    float Length() const {
        return sqrt(x * x + y * y);
    }

    Vector Normalize() const {
        float len = Length();
        return {x / len, y / len};
    }
};