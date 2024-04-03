#include <iostream>
#include <string>
#include <utility>
#include "City.h"

using namespace std;

City::City(string name, int x, int y) {
    this->name = std::move(name);
    this->x = x;
    this->y = y;
}

string City::getName() {
  return name;
}

int City::getX() const {
    return x;
}

int City::getY() const {
  return y;
}

void City::print() {
  cout << name << " (" << x << ", " << y << ")" << endl;
}
