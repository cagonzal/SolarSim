// body.hpp
#ifndef BODY_H
#define BODY_H

#include "vector3.hpp"

class Body {
    public:
        double mass;
        Vector3 position;
        Vector3 velocity;
        Vector3 acceleration;

        // Constructors
        Body() : mass(1.0) {}

        Body (double m, const Vector3& pos, const Vector3& vel) : mass(m), position(pos), velocity(vel), acceleration(0, 0, 0) {}

        // Compute kinetic energy 
        double kinetic_energy() const {
            return 0.5 * mass * velocity.magnitude_squared();
        }

        // Compute momentum 
        Vector3 momentum() const {
            return mass * velocity;
        }
};

#endif
