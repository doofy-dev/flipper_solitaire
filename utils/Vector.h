#pragma once

#include <cmath>
#include "Helpers.h"

/**
 * @class Vector
 * @brief Represents a 2D vector and provides various operations on it.
 *
 * This class represents a 2D vector with `x` and `y` components. It provides
 * various operators and functions to perform operations on vectors such as
 * addition, subtraction, multiplication, division, normalization, rotation, etc.
 */
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

    /**
     * @brief Represents the addition of two vectors.
     *
     * @param other The vector to add with.
     * @return The sum of the two vectors.
     */
    Vector operator+(Vector const &other) {
        return Vector({x + other.x, y + other.y});
    }

    /**
     * @brief Represents the addition of a constant float to a vector.
     *
     * This function overloads the '+' operator to allow the addition of a constant float value to a vector.
     *
     * @param other The constant float value to add.
     * @return A new vector that is the sum of the original vector and the constant float value.
     */
    Vector operator+(float const &other) {
        return Vector({x + other, y + other});
    }

    /**
     * @brief Adds a vector to the current vector.
     *
     * This operator overloads the '+=' operator to allow adding another vector to the current vector.
     *
     * @param other The vector to be added to the current vector.
     * @return A reference to the modified current vector.
     */
    Vector &operator+=(Vector const &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /**
     * @brief Represents the subtraction of two vectors.
     *
     * This operator overloads the '-' operator to subtract one vector from another.
     *
     * @param other The vector to subtract with.
     * @return The result of subtracting the two vectors.
     */
    Vector operator-(Vector const &other) const{
        return Vector{x - other.x, y - other.y};
    }

    /**
     * @brief Subtract another vector from the current vector.
     *
     * This operator overloads the '-=' operator to subtract another vector from the current vector.
     *
     * @param other The vector to subtract.
     * @return A reference to the modified current vector.
     */
    Vector &operator-=(Vector const &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /**
     * @brief Represents the multiplication of two vectors.
     *
     * This operator overloads the '*' operator to multiply two vectors element-wise.
     *
     * @param other The vector to multiply with.
     * @return The result of multiplying the two vectors element-wise.
     */
    Vector operator*(Vector const &other) {
        return Vector{x * other.x, y * other.y};
    }


    /**
     * @brief Multiplies the vector with a constant float value.
     *
     * This operator overloads the '*' operator to allow the multiplication of the
     * vector with a constant float value. The result is a new vector with each
     * component of the original vector multiplied by the constant float value.
     *
     * @param other The constant float value to multiply with.
     * @return A new vector that is the result of multiplying the original vector
     *         with the constant float value.
     */
    Vector operator*(float other) {
        return Vector{x * other, y * other};
    }

    /**
     * @brief Multiplies the vector with another vector element-wise.
     *
     * This operator overloads the "*=" operator to multiply the vector with another vector
     * element-wise. Each component of the current vector is multiplied by the corresponding
     * component of the other vector.
     *
     * @param other The vector to multiply with.
     * @return A reference to the modified current vector.
     */
    Vector &operator*=(Vector const &other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    /**
     * @brief Multiplies the vector by a constant float value in place.
     *
     * This operator overloads the "*=" operator to multiply each component of the vector by a constant float value.
     *
     * @param other The constant float value to multiply the vector by.
     * @return A reference to the modified vector.
     */
    Vector &operator*=(float other) {
        x *= other;
        y *= other;
        return *this;
    }

    /**
     * @brief Represents the division of two vectors.
     *
     * This operator overloads the '/' operator to divide one vector by another element-wise.
     *
     * @param other The vector to divide by.
     * @return The result of dividing the two vectors element-wise.
     */
    Vector operator/(Vector const &other) {
        return Vector{x / other.x, y / other.y};
    }

    /**
     * @brief Divides the vector by another vector element-wise.
     *
     * This operator overloads the '/=' operator to divide the vector by another vector
     * element-wise. Each component of the current vector is divided by the corresponding
     * component of the other vector.
     *
     * @param other The vector to divide by.
     * @return A reference to the modified current vector.
     */
    Vector &operator/=(Vector const &other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    /**
     * @brief Divides the vector by a constant float value.
     *
     * This operator overloads the '/' operator to divide each component of the vector
     * by a constant float value. The result is a new vector with each component of the
     * original vector divided by the constant float value.
     *
     * @param other The constant float value to divide the vector by.
     * @return A new vector that is the result of dividing the original vector by the
     *         constant float value.
     */
    Vector operator/(float other) {
        return Vector{x / other, y / other};
    }

    /**
     * @brief Divides the vector by a constant float value in place.
     *
     * This operator overloads the '/=' operator to divide each component of the vector
     * by a constant float value. The result is a new vector with each component of the
     * original vector divided by the constant float value.
     *
     * @param other The constant float value to divide the vector by.
     * @return A reference to the modified vector.
     */
    Vector &operator/=(float other) {
        x /= other;
        y /= other;
        return *this;
    }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * The magnitude of a 2D vector is the length of the vector computed using the Euclidean distance formula.
     * The magnitude is computed as the square root of the sum of the squares of the vector's x and y components.
     *
     * @return The magnitude of the vector.
     */
    float magnitude() {
        return sqrtf(x * x + y * y);
    }

    /**
     * @brief Calculates the distance between the current vector and another vector.
     *
     * This function calculates the distance between the current vector and another vector by subtracting the two vectors and then calculating the magnitude of the resulting vector.
     *
     * @param other The vector to calculate the distance from.
     * @return The distance between the two vectors.
     */
    float distance(Vector const &other) {
        Vector v = *this - other;
        return v.magnitude();
    }

    /**
     * @brief Normalizes the vector.
     *
     * This function normalizes the vector by dividing its components by its magnitude.
     * If the magnitude is zero, the vector is set to (0,0).
     *
     * @return void
     */
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

    /**
     * @brief Returns a new vector that is the normalized version of the current vector.
     *
     * This function normalizes the vector by dividing its components by its magnitude.
     * If the magnitude is zero, the vector is set to (0,0).
     *
     * @return The normalized vector.
     */
    Vector normalized() {
        float m = magnitude();
        if (m == 0) return {0, 0};

        return {x / m, y / m};
    }

    /**
     * @brief Returns the inverse of the vector.
     *
     * The inverse of a vector is a new vector with the negated values of its `x` and `y` components.
     *
     * @return The inverse of the vector.
     */
    Vector inverse() {
        return {-x, -y};
    }

    /**
     * @brief Calculates the dot product of the current vector and another vector.
     *
     * The dot product is a scalar value that represents the projection of one vector onto another.
     *
     * The dot product of two vectors A and B can be calculated using the formula:
     * dot(A, B) = A.x * B.x + A.y * B.y
     *
     * @param b The vector to calculate the dot product with.
     * @return The dot product of the two vectors.
     */
    float dot(Vector const &b) {
        return x * b.x + y * b.y;
    }

    /**
     * @brief Rotates the vector by the specified angle in degrees.
     *
     * This function rotates the vector by the specified angle in degrees.
     * The rotation is performed using the cosine and sine functions, and the result is stored in the x and y components of the vector.
     *
     * @param deg The angle to rotate the vector by in degrees.
     * @return void
     */
    void rotate(float deg) {
        float tx = x;
        float ty = y;
        x = (float) (cos(deg) * tx - sin(deg) * ty);
        y = (float) (sin(deg) * tx + cos(deg) * ty);
    }

    /**
     * @brief Rounds the x and y components of the vector to the nearest integer.
     *
     * This function rounds the x and y components of the vector to the nearest integer by using the round function
     * from the cmath library. The result is stored back into the x and y
     * components of the vector.
     *
     * @return void
     */
    void rounded() {
        x = (float) round(x);
        y = (float) round(y);
    }

    /**
     * @brief Calculates and returns a 2D vector that is rotated by the specified angle in degrees.
     *
     * This function takes a floating-point value representing an angle in degrees and returns a new 2D vector that is rotated by
     * that angle. The rotation is performed in the counter-clockwise direction.
     *
     * @param deg The angle to rotate the vector by in degrees.
     * @return The rotated 2D vector.
     *
     * @note This function assumes that the input angle is in degrees, not radians. If the input angle is in radians, it needs to be
     * converted to degrees before being passed to this function.
     *
     * @note The returned vector is a new instance and does not modify the original vector.
     *
     * @warning This function assumes that the vector is represented using floating-point values. If the vector uses integer values,
     * the return type and calculations should be adjusted accordingly.
     *
     * @see https://en.wikipedia.org/wiki/Rotation_matrix for more information on rotation matrices.
     *
     * @sa rotate(float deg) to rotate the vector in-place.
     *
     * Example usage:
     * @code
     * Vector v1(2, 3);
     * Vector v2 = v1.rotated(45.0f);
     * // v2 = { -0.707, 3.536 }
     * @endcode
     */
    Vector rotated(float deg) {
        return {
                (float) (cos(deg) * x - sin(deg) * y),
                (float) (sin(deg) * x + cos(deg) * y)
        };
    }

    /**
     * @brief Calculates the cross product between two vectors.
     *
     * This method calculates the cross product between the current vector and the
     * specified vector, and returns the result. The cross product is calculated
     * using the formula: cross product = (x1 * y2) - (y1 * x2).
     *
     * @param other The vector to calculate the cross product with.
     * @return The cross product between the two vectors.
     */
    float cross(Vector const &other) {
        return x * other.x - y * other.y;
    }

    /**
     * @brief Calculates the perpendicular vector.
     *
     * This function calculates the perpendicular vector of the current vector.
     *
     * @return The perpendicular vector.
     */
    Vector perpendicular() {
        return {-y, x};
    }

    /**
     * @brief Projects a vector onto a line segment defined by two points.
     *
     * Given a line segment defined by two points lineA and lineB, this function calculates the projection of the current vector onto that line segment.
     * It returns the projected vector point and also sets the success flag to indicate whether the projection was successful or not.
     *
     * @param lineA The first point defining the line segment.
     * @param lineB The second point defining the line segment.
     * @param success A pointer to a boolean value that will be set to true if the projection is successful, and false otherwise.
     *
     * @return The projected vector point if the projection is successful, otherwise an empty vector.
     */
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

    /**
     * @brief Linearly interpolates between two vectors.
     *
     * This function performs linear interpolation between two vectors using the given time value.
     * It is assumed that the start and end vectors have the same dimension.
     *
     * @param start The start vector.
     * @param end The end vector.
     * @param time The interpolation time value between 0 and 1.
     * @return The interpolated vector.
     */
    static Vector Lerp(Vector const &start, Vector const &end, float time) {
        return {
                lerp(start.x, end.x, time),
                lerp(start.y, end.y, time)
        };
    }

    /**
     * @brief Calculate the quadratic Bezier interpolation between three vectors.
     *
     * This function calculates the position of a point along a quadratic Bezier curve, given three control points
     * and a time value.
     *
     * @param start The start point of the curve.
     * @param control The control point of the curve, which affects the shape of the curve.
     * @param end The end point of the curve.
     * @param time The time value parameter that specifies the position along the curve.
     * @return The interpolated vector position.
     *
     * @see Vector::Lerp()
     */
    static Vector Quadratic(Vector const &start, Vector const &control, Vector const &end, float time) {
        Vector a = Vector::Lerp(start, control, time);
        Vector b = Vector::Lerp(control, end, time);
        return Vector::Lerp(a, b, time);
    }
};
