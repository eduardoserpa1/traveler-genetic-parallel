using namespace std;

#ifndef CITY_H
#define CITY_H

class City {

  string name;
  int x;
  int y;

  public:
    City(string name, int x, int y);
    string getName();
    int getX() const;
    int getY() const;
    void print();
    bool equals(City c);
};

#endif
