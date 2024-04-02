#include <iostream>
#include <vector>
#include <cmath>
#include "City.h"
#include "Route.h"

using namespace std;

class Route {

  vector<City> cities;
  double distance;

  public:
    Route(vector<City> cities) {
      this->cities = cities;
      distance = getDistance();
    }

    vector<City> getCities() {
      return cities;
    }

    double getDistance() {
      double total = 0;
      for (int i = 0; i < cities.size(); i++) {
        City from = cities[i];
        City to = cities[(i + 1) % cities.size()];
        total += sqrt(pow(to.getX() - from.getX(), 2) + pow(to.getY() - from.getY(), 2));
        distance = total;
      }

      return distance;
    }

    void print() {
      cout << "Route: " << &cities << ":" << distance << endl;
    }
};
