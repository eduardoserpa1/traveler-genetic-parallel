#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "City.h"
#include "Route.h"

using namespace std;

//GLOBAL
vector<City> cityData;
const string filename = "estados-30.csv";

//DECLARATIONS
void setup();
void loadCityData();
void createFirstElite();

//IMPLEMENTATION
void setup(){
    loadCityData();
    createFirstElite();
}

void loadCityData(){
    vector<string> linhas;
    ifstream arquivo(filename);

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo " << filename << endl;
        return;
    }

    string linha;
    while (getline(arquivo, linha)) {
        linhas.push_back(linha);
    }

    arquivo.close();

    int index = 0;

    for (const auto& linha : linhas) {
        istringstream iss(linha);
        string name, x, y;

        getline(iss, name, ',');
        getline(iss, x, ',');
        getline(iss, y, ',');

        City city = City(name, stoi(x), stoi(y));
        cityData.push_back(city);
        index++;
    }
}

void createFirstElite(){

}

int main() {
    setup();
    Route route(cityData);

    route.print();

    return 0;
};
