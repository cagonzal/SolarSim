// planet_data.hpp
#ifndef PLANET_DATA_H
#define PLANET_DATA_H

#include "body.hpp"
#include "vector3.hpp"
#include <string>
#include <map>
#include <cmath>

struct PlanetData {
    std::string name;
    double mass; // Solar masses
    double semi_major_axis; // AU 
    double eccentricity;
    std::string parent;
};

class PlanetDatabase {
    private:
        std::map<std::string, PlanetData> planets;

    public:
        PlanetDatabase() {
            // name, M / M_sun, mean distance from parent body, eccentricity
            planets["sun"] = {"Sun", 1.0, 0.0, 0.0};

            planets["mercury"] = {"Mercury", 1.66e-7, 0.387, 0.206, "sun"};
            planets["venus"] = {"Venus", 2.45e-6, 0.723, 0.007, "sun"};
            planets["earth"] = {"Earth", 3.00e-6, 1.0, 0.017, "sun"};
            planets["moon"] = {"Moon", 3.69e-8, 0.00257, 0.055, "earth"};
            planets["mars"] = {"Mars", 3.23e-7, 1.524, 0.093, "sun"};

            planets["jupiter"] = {"Jupiter", 9.55e-4,5.203, 0.048, "sun"}; 
            planets["saturn"] = {"Saturn", 2.86e-4, 9.537, 0.054, "sun"};
            planets["uranus"] = {"Uranus", 4.37e-5, 19.191, 0.047, "sun"};
            planets["neptune"] = {"Neptune", 5.15e-5, 30.069, 0.009, "sun"};

            // set mass to machine precision -> exert no influence on the earth 
            // distance is set to a high earth orbit 
            // ciruclar so eccentricity is zero 
            planets["satellite"] = {"Mars Reconnaissance Orbiter", 1e-15, 2.392e-4, 0.0, "earth"}; 
        }

        bool has_planet(const std::string& name) const {
            return planets.find(name) != planets.end();
        }

        PlanetData get_planet(const std::string& name) const {
            auto it = planets.find(name);
            if (it != planets.end()) {
                return it->second;
            }
            throw std::runtime_error("Planet not found: " + name);
        }

        Body create_body(const std::string& name, double G = 4.0 * M_PI * M_PI) const {
            PlanetData data = get_planet(name);

            // special case for sun 
            if (name == "sun") {
                return Body(data.mass, Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0));
            }

            double a = data.semi_major_axis;
            double e = data.eccentricity;

            // distance at perihelion 
            double r = a * (1.0 - e);

            Vector3 position(r, 0.0, 0.0);

            // velocity at perhilion 
            // v = sqrt(G * M_sun * (1 + e) / (a * (1 - e)))
            double v = std::sqrt(G * 1.0 * (1.0 + e) / (a * (1.0 - e)));
            Vector3 velocity(0.0, v, 0.0);

            return Body(data.mass, position, velocity);
        }

        Body create_body_hierarchical(const std:: string& name,
                const std::map<std::string, Body>& existing_bodies,
                double G = 4.0 * M_PI * M_PI) const {

            PlanetData data = get_planet(name);

            if (name == "sun" || data.parent.empty()) {
                return Body(data.mass, Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0));
            }

            // case where body orbits the sun directly 
            if (data.parent == "sun") {

                double a = data.semi_major_axis;
                double e = data.eccentricity; 
                double r = a * (1.0 - e); 

                Vector3 position(r, 0, 0);
                double v = std::sqrt(G * 1.0 * (1.0 + e) / (a * (1.0 - e)));
                Vector3 velocity(0, v, 0);

                return Body(data.mass, position, velocity);

            }

            auto parent_it = existing_bodies.find(data.parent);
            if (parent_it == existing_bodies.end()) {
                throw std::runtime_error("Parent body '" + data.parent + "' must be created before '" + name + "'");
            }

            const Body& parent = parent_it->second;

            double a = data.semi_major_axis;
            double e = data.eccentricity;
            double r = a * (1.0 - e);  // Distance at perihelion

            // Position relative to parent
            Vector3 rel_pos(r, 0, 0);

            // Velocity relative to parent (orbiting around the parent)
            // Use parent's mass instead of sun's mass
            double v_rel = std::sqrt(G * parent.mass * (1.0 + e) / (a * (1.0 - e)));
            Vector3 rel_vel(0, v_rel, 0);

            // Absolute position and velocity = parent + relative
            Vector3 abs_pos = parent.position + rel_pos;
            Vector3 abs_vel = parent.velocity + rel_vel;

            return Body(data.mass, abs_pos, abs_vel);

        }
};

#endif 
