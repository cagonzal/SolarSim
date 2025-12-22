// main.cpp 

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "nbody.hpp"
#include "planet_data.hpp"

int main() {

    // create simulator 
    NBodySimulator sim;
    PlanetDatabase db; 

    std::vector<std::string> planets_to_simulate = {
        "sun",
        "mercury",
        "venus",
        "earth",
        // "moon",
        "mars"
        // "jupiter",
        // "neptune",
        // "uranus",
        // "neptune"
    };


    // for (const auto& planet_name : planets_to_simulate) {
    //     if (db.has_planet(planet_name)) {
    //         Body body = db.create_body(planet_name);
    //         sim.add_body(body, planet_name);
    //         std::cout << "Added " << planet_name << std::endl;
    //     }
    //     else {
    //         std::cerr << "Warning: Planet '" << planet_name << "' not found" << std::endl;
    //     }
    // }
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

    // sim params 
    double dt = 0.00001; // years 
    // double t_end = 2.5;
    double t_end = 1.0;
    int output_interval = 100; 

    double E0 = sim.total_energy(); // initial energy 
    std::cout << "Initial energy: " << E0 << std::endl;

    std::ofstream outfile("orbit.dat");
    outfile << "# time";
    for (const auto& name : sim.get_names()) {
        outfile << " x_" << name << " y_" << name;
    }
    outfile << " energy_error\n";

    sim.bootstrap(dt); // run single Euler step 
                       
    // main integration loop
    int step = 0; 
    while (sim.get_time() < t_end) {
        sim.step(dt);
        step++;

        if (step % output_interval == 0) {
            const auto& bodies = sim.get_bodies();
            double E = sim.total_energy();
            double energy_error = (E - E0) / E0;
            outfile << sim.get_time();

            // Write all planet positions
            for (const auto& body : bodies) {
                outfile << " " << body.position.x << " " << body.position.y;
            }

            // Write energy error
            outfile << " " << energy_error << "\n";

            std::cout << "t = " << sim.get_time()
                << ", E_error = " << energy_error << std::endl;
        }
    }

    outfile.close();

    std::cout << "\nFinal state:\n";
    sim.print_state();
    std::cout << "\nEnergy drift: " << (sim.total_energy() - E0) / E0 * 100 << "%" << std::endl;

    return 0;
}
