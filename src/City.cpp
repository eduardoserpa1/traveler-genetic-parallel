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
// (" << x << ", " << y << ")
void City::print() {
  cout << name << " -> ";
}

bool City::equals(City c) {
    if( this->getName() == c.getName() && this->getX() == c.getX() && this->getY() == c.getY())
        return true;
    return false;
}
