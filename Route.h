using namespace std;

#ifndef ROUTE_H
#define ROUTE_H

class Route {

  vector<City> cities;
  double distance;

  public:
    Route(vector<City> cities);
    vector<City> getCities();
    double getDistance();
    void print();
};

#endif
