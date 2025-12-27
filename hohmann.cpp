// main.cpp 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "nbody.hpp"
#include "planet_data.hpp"
#include "hohmann.hpp"

int main() {

    std::cout << "=== N-BODY SIMULATOR WITH HOHMANN TRANSFERS ===" << std::endl;
    std::cout << "Select mission type:" << std::endl;
    std::cout << "1. Simple altitude change (Earth orbit)" << std::endl;
    std::cout << "2. Interplanetary transfer (Earth to Mars)" << std::endl;
    std::cout << "Enter choice (1 or 2): ";

    int choice = 1;
    // std::cin >> choice; 

    // create simulator 
    NBodySimulator sim;
    PlanetDatabase db; 

    std::vector<std::string> planets_to_simulate;

    if (choice == 1) {
        planets_to_simulate = {"sun", "earth", "moon", "satellite"};
    }

    else {

        planets_to_simulate = {"sun", "earth", "mars", "satellite"};
    }

    std::map<std::string, Body> bodies_created;
    
    for (const auto& name : planets_to_simulate) {
        if (!db.has_planet(name)) {
            std::cerr << "Warning: Body '" << name << "' not found!" << std::endl;
            continue;
        }
        
        try {
            Body body = db.create_body_hierarchical(name, bodies_created);
            sim.add_body(body, name);
            bodies_created[name] = body;
            std::cout << "Added " << name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error adding " << name << ": " << e.what() << std::endl;
            return 1;
        }
    }

    // Find body indices
    int satellite_idx = sim.find_body_index("satellite");
    int earth_idx = sim.find_body_index("earth");
    int mars_idx = (choice == 2) ? sim.find_body_index("mars") : -1;
    
    if (satellite_idx < 0 || earth_idx < 0) {
        std::cerr << "Could not find satellite or Earth!" << std::endl;
        return 1;
    }
    
    if (choice == 2 && mars_idx < 0) {
        std::cerr << "Could not find Mars!" << std::endl;
        return 1;
    }


    // sim params 
    double dt = 0.0000001; // years 
    // double t_end = 2.5;
    double t_end = (choice == 1) ? 0.2 : 3.0; 
    int output_interval = 500; 

    HohmannTransfer transfer; 
    double transfer_start_time = 0.1;

    if (choice == 1) {
        // Simple altitude change: double the orbit radius
        double initial_radius = 2.392e-4;  // Current satellite orbit (AU)
        double target_radius = 2.0 * initial_radius;
        std::cout << "\n*** Planning altitude change mission ***\n" << std::endl;
    } else {
        // Interplanetary transfer to Mars
        std::cout << "\n*** Planning Earth to Mars transfer ***\n" << std::endl;
    }

    double E0 = sim.total_energy(); // initial energy 
    std::cout << "Initial energy: " << E0 << std::endl;

    std::ofstream outfile("orbit.dat");
    outfile << "# time";
    for (const auto& name : sim.get_names()) {
        outfile << " x_" << name << " y_" << name;
    }
    outfile << "\n";
    // outfile << " energy_error transfer_state sat_radius\n";

    sim.bootstrap(dt); // run single Euler step 

    bool transfer_planned = false; 
    double sun_mass = 1.0; 
    double earth_mass = 3.00e-6;
                       
    // main integration loop
    int step = 0; 
    while (sim.get_time() < t_end) {
        sim.step(dt);

        const auto& bodies = sim.get_bodies();

        // Plan the transfer at the designated time
        if (!transfer_planned && sim.get_time() >= transfer_start_time) {
            if (choice == 1) {
                // Altitude change around Earth
                double r_sat = (bodies[satellite_idx].position - bodies[earth_idx].position).magnitude();
                double target_radius = 2.0 * r_sat;
                std::cout << "earth orbit vel = " << bodies[earth_idx].velocity.magnitude() << std::endl;
                std::cout << "satellite orbit vel = " << bodies[satellite_idx].velocity.magnitude() << std::endl;
                transfer.plan_transfer_to_radius(r_sat, target_radius, earth_mass, sim.get_time());
            } else {
                // Interplanetary to Mars (orbits around Sun)
                transfer.plan_interplanetary_transfer(
                        bodies[satellite_idx],
                        bodies[earth_idx],
                        bodies[mars_idx],
                        sun_mass,
                        sim.get_time()
                        );
            }
            transfer_planned = true;
        }

        // Update the transfer (waits for launch window, applies burns)
        if (transfer_planned) {
            auto& bodies_mut = const_cast<std::vector<Body>&>(sim.get_bodies());

            // For interplanetary transfers, pass Mars; for altitude changes, pass nullptr
            const Body* target_body = (choice == 2) ? &bodies[mars_idx] : nullptr;

            transfer.update(bodies_mut[satellite_idx], bodies_mut[earth_idx], target_body, sim.get_time());
        }

        step++;

        if (step % output_interval == 0) {
            double E = sim.total_energy();
            double energy_error = (E - E0) / E0;

            // Calculate satellite radius (relative to Earth for altitude change, Sun for interplanetary)
            double sat_radius;
            if (choice == 1) {
                sat_radius = (bodies[satellite_idx].position - bodies[earth_idx].position).magnitude();
            } else {
                sat_radius = bodies[satellite_idx].position.magnitude();
            }

            outfile << sim.get_time();

            // Write all positions
            for (const auto& body : bodies) {
                outfile << " " << body.position.x << " " << body.position.y;
            }

            outfile << "\n";

            // Write diagnostics
            // outfile << " " << energy_error 
                // << " " << static_cast<int>(transfer.get_state())
                // << " " << sat_radius << "\n";

            if (step % (output_interval * 10) == 0) {
                std::cout << "t = " << sim.get_time()
                    << ", E_err = " << energy_error 
                    << ", r_sat = " << sat_radius << " AU";

                if (choice == 2 && mars_idx >= 0) {
                    // Show phase angle for interplanetary mission
                    double sat_angle = std::atan2(bodies[satellite_idx].position.y, 
                            bodies[satellite_idx].position.x);
                    double mars_angle = std::atan2(bodies[mars_idx].position.y,
                            bodies[mars_idx].position.x);
                    double phase = mars_angle - sat_angle;
                    while (phase < 0) phase += 2.0 * M_PI;
                    std::cout << ", phase = " << phase * 180.0/M_PI << "°";
                }
                std::cout << std::endl;
            }
        }
    }

    outfile.close();

    std::cout << "\n=== FINAL STATE ===" << std::endl;
    const auto& bodies = sim.get_bodies();
    
    if (choice == 1) {
        double final_radius = (bodies[satellite_idx].position - bodies[earth_idx].position).magnitude();
        double initial_radius = 2.392e-4;
        double target_radius = 6.0 * initial_radius;
        std::cout << "Initial orbit radius: " << initial_radius << " AU" << std::endl;
        std::cout << "Target orbit radius: " << target_radius << " AU" << std::endl;
        std::cout << "Final orbit radius: " << final_radius << " AU" << std::endl;
        std::cout << "Error: " << std::abs(final_radius - target_radius) / target_radius * 100 
                  << "%" << std::endl;
    } else {
        double sat_radius = bodies[satellite_idx].position.magnitude();
        double mars_radius = bodies[mars_idx].position.magnitude();
        std::cout << "Satellite distance from Sun: " << sat_radius << " AU" << std::endl;
        std::cout << "Mars distance from Sun: " << mars_radius << " AU" << std::endl;
        std::cout << "Distance to Mars: " 
                  << (bodies[satellite_idx].position - bodies[mars_idx].position).magnitude() 
                  << " AU" << std::endl;
    }
    
    std::cout << "Energy drift: " << (sim.total_energy() - E0) / E0 * 100 << "%" << std::endl;

    return 0;
}
