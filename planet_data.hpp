// planet_data.hpp
#ifndef PLANET_DATA_H
#define PLANET_DATA_H

#include "body.hpp"
#include <string>
#include <map>
#include <cmath>

struct PlanetData {
    std::string name;
    double mass; // Solar masses
    double semi_major_axis; // AU 
    double eccentricity;
};

class PlanetDatabase {
    private:
        std::map<std::string, PlanetData> planets;

    public:
        PlanetDatabase() {
            planets["sun"] = {"Sun", 1.0, 1.0, 0.0};

            planets["mercury"] = {"Mercury", 1.66e-7, 0.387, 0.206};
            planets["venus"] = {"Venus", 2.45e-6, 0.723, 0.007};
            planets["earth"] = {"Earth", 3.00e-6, 1.0, 0.017};
            planets["mars"] = {"Mars", 3.23e-7, 1.524, 0.093};

            planets["jupiter"] = {"Jupiter", 9.55e-4,5.203, 0.048}; 
            planets["saturn"] = {"Saturn", 2.86e-4, 9.537, 0.054};
            planets["uranus"] = {"Uranus", 4.37e-5, 19.191, 0.047};
            planets["neptune"] = {"Neptune", 5.15e-5, 30.069, 0.009};
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
};

#endif 
