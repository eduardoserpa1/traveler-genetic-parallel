#include <iostream>
#include <string>
#include "City.h"

using namespace std;

class City {

  string name;
  int x;
  int y;

  public:
    City(string name, int x, int y) {
      this->name = name;
      this->x = x;
      this->y = y;
    }

    string getName() {
      return name;
    }

    int getX() {
      return x;
    }

    int getY() {
      return y;
    }

    void print() {
      cout << name << " (" << x << ", " << y << ")" << endl;
    }
};
