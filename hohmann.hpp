// hohmann.hpp
#ifndef HOHMANN_H 
#define HOHMANN_H 

#include "vector3.hpp"
#include "body.hpp"
#include <cmath>
#include <iostream>

class HohmannTransfer {
    public:
        enum State {
            PLANNING,
            WAITING_PHASING,
            BURN1_READY,
            COASTING,
            BURN2_READY,
            COMPLETE
        };

    private:
        State state; 
        double G;
        double mu_central; 

        int satellite_idx;
        int target_idx;
        bool is_interplanetary;

        double r1; 
        double r2; 

        double transfer_time;
        double departure_time;
        double phase_angle_required;
        Vector3 delta_v1;
        Vector3 delta_v2; 

        double total_delta_v;
        double planning_time;

        double compute_phase_angle(const Vector3& pos1, const Vector3& pos2) const {
            double angle = std::atan2(pos2.y, pos2.x) - std::atan2(pos1.y, pos1.x);

            // normalize to [0, 2pi)
            while (angle < 0) angle += 2.0 * M_PI;
            while (angle >= 2.0* M_PI) angle -= 2.0 * M_PI;
            return angle;
        }

        double compute_synodic_period(double r_inner, double r_outer) const {
            double omega_inner = std::sqrt(mu_central / std::pow(r_inner, 3));
            double omega_outer = std::sqrt(mu_central / std::pow(r_outer, 3));
            return 2.0 * M_PI / std::abs(omega_inner - omega_outer);
        }


    public:
        HohmannTransfer(double gravitational_constant = 4.0 * M_PI * M_PI) 
            : state(PLANNING), G(gravitational_constant), mu_central(0.0),
            satellite_idx(-1), target_idx(-1), is_interplanetary(false),
            r1(0.0), r2(0.0), transfer_time(0.0), departure_time(0.0),
            phase_angle_required(0.0), total_delta_v(0.0), planning_time(0.0) {}

        void plan_transfer_to_radius(double radius1, double radius2, double central_body_mass, double current_time) {
            r1 = radius1; 
            r2 = radius2;
            mu_central = G * central_body_mass;
            std::cout << "radius1 " << r1 << std::endl;
            std::cout << "radius2 " << r2 << std::endl;
            is_interplanetary = false;
            planning_time = current_time; 

            double a_transfer = (r1 + r2) / 2.0; 

            transfer_time = M_PI * std::sqrt(std::pow(a_transfer, 3) / mu_central);

            // Velocities in circular orbits
            double v1_circular = std::sqrt(mu_central / r1);
            double v2_circular = std::sqrt(mu_central / r2);

            std::cout << "V_circ from formula = " << v1_circular << std::endl;

            // Velocities on transfer orbit at periapsis and apoapsis
            double v1_transfer = std::sqrt(mu_central * (2.0/r1 - 1.0/a_transfer));
            double v2_transfer = std::sqrt(mu_central * (2.0/r2 - 1.0/a_transfer));

            // Delta-v magnitudes (direction determined at burn time)
            double dv1_mag = v1_transfer - v1_circular;
            double dv2_mag = v2_circular - v2_transfer;

            total_delta_v = std::abs(dv1_mag) + std::abs(dv2_mag);

            // For simple altitude change, we can burn immediately
            departure_time = current_time;
            state = BURN1_READY;

            std::cout << "\n=== HOHMANN TRANSFER PLANNED (ALTITUDE CHANGE) ===" << std::endl;
            std::cout << "Initial radius: " << r1 << " AU" << std::endl;
            std::cout << "Final radius: " << r2 << " AU" << std::endl;
            std::cout << "Transfer time: " << transfer_time << " years" << std::endl;
            std::cout << "Delta-v1: " << dv1_mag << " AU/year" << std::endl;
            std::cout << "Delta-v2: " << dv2_mag << " AU/year" << std::endl;
            std::cout << "Total delta-v: " << total_delta_v << " AU/year" << std::endl;
            std::cout << "Departure time: " << departure_time << " years" << std::endl;
            std::cout << "=================================================\n" << std::endl;
        }

        void plan_interplanetary_transfer(const Body& satellite, const Body& origin_planet,
                const Body& target_planet, double central_body_mass, double current_time) {

            mu_central = G * central_body_mass;
            is_interplanetary = true;
            planning_time = current_time;

            r1 = origin_planet.position.magnitude();
            r2 = target_planet.position.magnitude();

            double a_transfer = (r1 + r2) / 2.0;

            transfer_time = M_PI * std::sqrt(std::pow(a_transfer, 3) / mu_central);

            double omega_target = std::sqrt(mu_central / std::pow(r2, 3));
            double target_travel_angle = omega_target * transfer_time;

            phase_angle_required = M_PI - target_travel_angle;

            // Normalize to [0, 2π)
            while (phase_angle_required < 0) phase_angle_required += 2.0 * M_PI;
            while (phase_angle_required >= 2.0 * M_PI) phase_angle_required -= 2.0 * M_PI;

            double v1_circular = std::sqrt(mu_central / r1);
            double v2_circular = std::sqrt(mu_central / r2);
            double v1_transfer = std::sqrt(mu_central * (2.0/r1 - 1.0/a_transfer));
            double v2_transfer = std::sqrt(mu_central * (2.0/r2 - 1.0/a_transfer));

            double dv1_mag = std::abs(v1_transfer - v1_circular);
            double dv2_mag = std::abs(v2_circular - v2_transfer);
            total_delta_v = dv1_mag + dv2_mag;

            // Calculate synodic period (time between successive launch windows)
            double synodic_period = compute_synodic_period(r1, r2);

            state = WAITING_PHASING;

            std::cout << "\n=== INTERPLANETARY HOHMANN TRANSFER PLANNED ===" << std::endl;
            std::cout << "Origin radius: " << r1 << " AU" << std::endl;
            std::cout << "Target radius: " << r2 << " AU" << std::endl;
            std::cout << "Transfer time: " << transfer_time << " years" << std::endl;
            std::cout << "Required phase angle: " << phase_angle_required * 180.0/M_PI << " degrees" << std::endl;
            std::cout << "Synodic period: " << synodic_period << " years" << std::endl;
            std::cout << "Delta-v1: " << dv1_mag << " AU/year" << std::endl;
            std::cout << "Delta-v2: " << dv2_mag << " AU/year" << std::endl;
            std::cout << "Total delta-v: " << total_delta_v << " AU/year" << std::endl;
            std::cout << "Waiting for launch window..." << std::endl;
            std::cout << "================================================\n" << std::endl;
            
        }

        bool check_launch_window(const Body& satellite, const Body& target, 
                double tolerance = 0.05) {
            if (state != WAITING_PHASING) return false;

            double current_phase = compute_phase_angle(satellite.position, target.position);
            double phase_error = std::abs(current_phase - phase_angle_required);

            // Handle wrap-around
            if (phase_error > M_PI) {
                phase_error = 2.0 * M_PI - phase_error;
            }

            return phase_error < tolerance;
        }

        bool update(Body& satellite, const Body& earth, const Body* target_body, double current_time) {
            double time_since_departure = current_time - departure_time;

            switch(state) {
                case PLANNING: {
                    return false; 
                }

                case WAITING_PHASING: {
                    if (target_body && check_launch_window(satellite, *target_body)) {
                        departure_time = current_time;
                        state = BURN1_READY;

                        double current_phase = compute_phase_angle(satellite.position, 
                                target_body->position);
                        std::cout << "\n*** LAUNCH WINDOW REACHED ***" << std::endl;
                        std::cout << "Time: " << current_time << " years" << std::endl;
                        std::cout << "Time since planning: " << (current_time - planning_time) 
                            << " years" << std::endl;
                        std::cout << "Current phase angle: " << current_phase * 180.0/M_PI 
                            << " degrees" << std::endl;
                        std::cout << "Required phase angle: " << phase_angle_required * 180.0/M_PI 
                            << " degrees" << std::endl;
                        std::cout << "*****************************\n" << std::endl;
                    }
                    return false;
                }
                case BURN1_READY: {

                    Vector3 r_rel = satellite.position - earth.position;
                    Vector3 v_rel = satellite.velocity - earth.velocity;

                    // Calculate required delta-v
                    // double v_circular = satellite.velocity.magnitude();

                    // double v_circular = sqrt(mu_central / r_rel.magnitude());
                    // double v_circular = std::sqrt(mu_central / r1);

                    double v_circular = sqrt(mu_central / r1);
                    Vector3 r_hat = r_rel.normalized();
                    Vector3 v_tan = v_rel - v_rel.dot(r_hat) * r_hat;
                    Vector3 v_tan_hat = v_tan.normalized();

                    double a_transfer = (r1 + r2) / 2.0;
                    double v_transfer = std::sqrt(mu_central * (2.0 / r1 - 1.0 / a_transfer));
                    double dv1_mag = v_transfer - v_circular;

                    // double eccentricity = (r2 - r1) / (r2 + r1);

                    delta_v1 = v_tan_hat * dv1_mag;
                    // satellite.velocity += delta_v1;
                    v_rel += delta_v1;
                    satellite.velocity = v_rel + earth.velocity;

                    state = COASTING;

                    std::cout << "t = " << current_time 
                        << " | BURN 1 applied: delta-v = " << dv1_mag 
                        << " AU/year" << std::endl;
                    std::cout << "New velocity magnitude: " << satellite.velocity.magnitude()
                        << " AU/year" << std::endl;
                    return true;
                }

                case COASTING: {
                    // Check if we've reached apoapsis (for outer transfer) or periapsis (for inner)
                    Vector3 r_rel = satellite.position - earth.position; 
                    Vector3 v_rel = satellite.velocity - earth.velocity;
                    double v_radial = r_rel.dot(v_rel);
                    double eps = 1e-6;
                    // at apogee, v_radial = 0 
                    // if (time_since_departure >= transfer_time - 1e-6) {
                    if (time_since_departure >= transfer_time * 0.9 && std::abs(v_radial) < eps) {
                        state = BURN2_READY;
                        std::cout << "\nt = " << current_time 
                            << " | Reached target orbit altitude" << std::endl;
                        std::cout << "Current radius: " << (satellite.position - earth.position).magnitude()
                            << " AU (target: " << r2 << " AU)" << std::endl;
                    }
                    return false;
                }

                case BURN2_READY: {

                    Vector3 r_rel = satellite.position - earth.position;
                    Vector3 v_rel = satellite.velocity - earth.velocity;

                    double v_circular = sqrt(mu_central / r2);
                    Vector3 r_hat = r_rel.normalized();
                    Vector3 v_tan = v_rel - v_rel.dot(r_hat) * r_hat;
                    Vector3 v_tan_hat = v_tan.normalized();

                    double a_transfer = (r1 + r2) / 2.0;
                    double v_transfer = std::sqrt(mu_central * (2.0 / r2 - 1.0 / a_transfer));

                    double dv2_mag = v_circular - v_transfer;

                    delta_v2 = v_tan_hat * dv2_mag;
                    // satellite.velocity += delta_v1;
                    v_rel += delta_v2;
                    satellite.velocity = v_rel + earth.velocity;
                    // this should be zero if properly burning at apogee
                    // std::cout << "v_radial at burn2 = " << r_rel.dot(v_rel) << "\n";

                    state = COMPLETE;

                    std::cout << "t = " << current_time 
                        << " | BURN 2 applied: delta-v = " << dv2_mag 
                        << " AU/year" << std::endl;
                    std::cout << "New velocity magnitude: " << satellite.velocity.magnitude()
                        << " AU/year" << std::endl;
                    std::cout << "=== TRANSFER COMPLETE ===" << std::endl;
                    std::cout << "Total delta-v used: " << total_delta_v << " AU/year\n" << std::endl;
                    return true;
                    }

                case COMPLETE: {
                    return false;
                }

            return false;
            }
        }

        State get_state() const { return state; }
        double get_total_delta_v() const { return total_delta_v; }
        double get_transfer_time() const { return transfer_time; }
        double get_departure_time() const { return departure_time; }
        double get_phase_angle_required() const { return phase_angle_required; }

};

#endif
