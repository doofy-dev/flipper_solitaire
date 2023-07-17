#pragma once

#include <cmath>
#include "Helpers.h"

struct Vector {
    float x;
    float y;

    Vector &operator=(const Vector &other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    Vector(const Vector &other) : x(other.x), y(other.y) {}

    Vector(float _x, float _y) : x(_x), y(_y) {}

    Vector() : x(0), y(0) {}

    Vector operator+(Vector const &other) {
        return Vector({x + other.x, y + other.y});
    }

    Vector operator+(float const &other) {
        return Vector({x + other, y + other});
    }

    Vector &operator+=(Vector const &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector operator-(Vector const &other) {
        return Vector{x - other.x, y - other.y};
    }

    Vector &operator-=(Vector const &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector operator-(Vector const &other) const {
        return Vector{x - other.x, y - other.y};
    }

    Vector operator*(Vector const &other) {
        return Vector{x * other.x, y * other.y};
    }


    Vector operator*(float other) {
        return Vector{x * other, y * other};
    }

    Vector &operator*=(Vector const &other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    Vector &operator*=(float other) {
        x *= other;
        y *= other;
        return *this;
    }

    Vector operator/(Vector const &other) {
        return Vector{x / other.x, y / other.y};
    }

    Vector &operator/=(Vector const &other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    Vector operator/(float other) {
        return Vector{x / other, y / other};
    }

    Vector &operator/=(float other) {
        x /= other;
        y /= other;
        return *this;
    }

    float magnitude() {
        return sqrtf(x * x + y * y);
    }

    float distance(Vector const &other) {
        Vector v = *this - other;
        return v.magnitude();
    }

    void normalize() {
        float m = magnitude();
        if (m == 0) {
            x = 0;
            y = 0;
        }else{
            x = x / m;
            y = y / m;
        }
    }

    Vector normalized() {
        float m = magnitude();
        if (m == 0) return {0, 0};

        return {x / m, y / m};
    }

    Vector inverse() {
        return {-x, -y};
    }

    float dot(Vector const &b) {
        return x * b.x + y * b.y;
    }

    void rotate(float deg) {
        float tx = x;
        float ty = y;
        x = (float) (cos(deg) * (double) tx - sin(deg) * (double) ty);
        y = (float) (sin(deg) * (double) tx + cos(deg) * (double) ty);
    }

    void rounded() {
        x = (float) round(x);
        y = (float) round(y);
    }

    Vector rotated(float deg) {
        return {
                (float) (cos(deg) * (double) x - sin(deg) * (double) y),
                (float) (sin(deg) * (double) x + cos(deg) * (double) y)
        };
    }

    float cross(Vector const &other) {
        return x * other.x - y * other.y;
    }

    Vector perpendicular() {
        return {-y, x};
    }

    Vector project(Vector const &lineA, Vector const &lineB, bool *success) {
        Vector AB = lineB - lineA;
        Vector AC = *this - lineA;

        float k = AC.dot(AB) / AB.dot(AB);
        if (k < 0 || k > 1) {
            *success = false;
            return {};
        }
        *success = true;
        return {
            k * AB.x + lineA.x,
            k * AB.y + lineA.y
        };
    }

    static Vector Lerp(Vector const &start, Vector const &end, float time) {
        return {
                lerp(start.x, end.x, time),
                lerp(start.y, end.y, time)
        };
    }

    static Vector Quadratic(Vector const &start, Vector const &control, Vector const &end, float time) {
        Vector a = Vector::Lerp(start, control, time);
        Vector b = Vector::Lerp(control, end, time);
        return Vector::Lerp(a, b, time);
    }
};
