// nbody.hpp 
#ifndef NBODY_H
#define NBODY_H

#include <vector>
#include "body.hpp"

class NBodySimulator {
    private:
        std::vector<Body> bodies;
        std::vector<Body> bodies_prev;
        std::vector<std::string> body_names; 
        double G; // gravitational constant 
        double time; 

    public:
        static constexpr double pi = 3.14159265358979;
        // System is nondimensionalized using M_sun, 1 AU = 1, 1 year = 1 
        // this leads to the value G = 4 * pi^2
        NBodySimulator(double gravitational_constant = 4.0 * pi * pi) : G(gravitational_constant), time(0.0) {}

        void add_body(const Body& body, const std::string& name = "") {
            bodies.push_back(body);
            bodies_prev.push_back(body);
            body_names.push_back(name);
        }

        const std::vector<std::string>& get_names() const {
            return body_names;
        }

        // find body by name 
        int find_body_index(const std::string& name) const {
            for (size_t i = 0; i < body_names.size(); ++i) {
                if (body_names[i] == name) {
                    return i;
                }
            }
            return -1;
        }

        // compute accelerations for all bodies 
        void compute_accelerations() {
            // Reset accelerations 
            for (auto& body : bodies) { 
                body.acceleration = Vector3(0.0, 0.0, 0.0);
            }

            // Compute pairwise forces 
            // size_t is unsigned int and is typically used for the size of arrays 
            for (size_t i = 0; i < bodies.size(); ++i) {
                for (size_t j = i + 1; j < bodies.size(); ++j) {
                    Vector3 r = bodies[i].position - bodies[j].position;
                    double r_mag = r.magnitude();

                    // avoid division by zero 
                    if (r_mag < 1e-10) continue;

                    double force_mag = G * bodies[i].mass * bodies[j].mass / (r_mag * r_mag * r_mag);
                    Vector3 force = r * (-1.0 * force_mag);

                    bodies[i].acceleration += force / bodies[i].mass;
                    bodies[j].acceleration -= force / bodies[j].mass;
                }
            }
        }

        // Bootstrap with Euler
        void bootstrap(double dt) {
            compute_accelerations();

            for (size_t i = 0; i < bodies.size(); ++i) {
                bodies_prev[i] = bodies[i]; // store initial state 
                
                bodies[i].position += bodies[i].velocity * dt; 
                bodies[i].velocity += bodies[i].acceleration * dt;
            }

            time += dt; 
        }

        // leapfrog 
        void step(double dt) {
            compute_accelerations();

            std::vector<Body> bodies_new = bodies;

            for (size_t i = 0; i < bodies.size(); ++i) {
                
                bodies_new[i].velocity = bodies_prev[i].velocity + bodies[i].acceleration * 2.0 * dt;

                bodies_new[i].position = bodies_prev[i].position + bodies[i].velocity * 2.0 * dt;
            }

            bodies_prev = bodies;
            bodies = bodies_new;

            time += dt;
        }

        // energy 
        double total_energy() const {
            double kinetic = 0.0;
            double potential = 0.0;

            for (const auto& body : bodies) {
                kinetic += body.kinetic_energy();
            }

            for (size_t i = 0; i < bodies.size(); ++i) {
                for (size_t j = i + 1; j < bodies.size(); ++j) {
                    Vector3 r = bodies[i].position - bodies[j].position;
                    double r_mag = r.magnitude();
                    if (r_mag > 1e-10) {
                        potential -= G * bodies[i].mass * bodies[j].mass / r_mag;
                    }
                }
            }

            return kinetic + potential;
        }

        double get_time() const { return time; } 

        const std::vector<Body>& get_bodies() const { return bodies; }

        void print_state() const {
            std::cout << "Time: " << time << std::endl;
            for (size_t i = 0; i < bodies.size(); ++i) {
                std::cout << "Body " << i << ": pos = ";
                bodies[i].position.print();
                std::cout << ", vel = ";
                bodies[i].velocity.print();
                std::cout << std::endl;
            }
            std::cout << "Total Energy: " << total_energy() << std::endl;
        }
};

#endif 
