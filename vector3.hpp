// vector3.hpp 

#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream> 

class Vector3 {
    public:
        double x, y, z;

        // Constructors 
        Vector3() : x(0), y(0), z(0) {}
        Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

        // Vector Operations 
        Vector3 operator+(const Vector3& v) const {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        Vector3 operator-(const Vector3& v) const {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        Vector3 operator*(double scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(double scalar) const {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        Vector3& operator+=(const Vector3& v) {
            x += v.x; y += v.y; z += v.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& v) {
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }

        // Magnitude 
        double magnitude() const {
            return std::sqrt(x*x + y*y + z*z);
        }

        double magnitude_squared() const {
            return x*x + y*y + z*z;
        }

        // Dot product 
        double dot(const Vector3& v) const {
            return x*v.x + y*v.y + z*v.z;
        }

        // Normalized vector 
        Vector3 normalized() const {
            double mag = magnitude();
            if (mag > 0) {
                return *this / mag; 
            }
            return Vector3(0, 0, 0);
        }

        void print() const {
            std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
        }
};

// Scalar multplication from the left 
// Must be defined outside the class 
inline Vector3 operator*(double scalar, const Vector3& v) {
    return v * scalar;
}

#endif
