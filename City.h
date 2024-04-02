#ifndef CITY_H
#define CITY_H

class City {

  string name;
  int x;
  int y;

  public:
    City(string name, int x, int y);
    string getName();
    int getX();
    int getY();
    void print();
};

#endif
