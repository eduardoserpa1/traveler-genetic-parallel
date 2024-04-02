#include <iostream>
#include <vector>
#include "City.h"
#include "Route.h"

using namespace std;

int main() {
  vector<City> cities = {
    City("A", 0, 0),
    City("B", 1, 0),
    City("C", 1, 1),
    City("D", 0, 1)
  };

  Route route(cities);

  route.print();

  return 0;
};
