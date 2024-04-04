#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "City.h"
#include "Route.h"

using namespace std;

//GLOBAL
vector<City> cityData;
const string filename = "../database/estados-30.csv";
int FIRST_POPULATION_LENGTH = 1024;
int POPULATION_DIVISOR = 8;
vector<Route> firstPopulation;

//DECLARATIONS
void setup();
void loadCityData();
void createFirstElite();
void generateFirstPopulation();
void sortVectorByDistance();

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


Route createRandomRoute(){
    vector<int> randomVector;
    vector<City> finalVector;

    for (int i = 0; i < cityData.size(); ++i) {
        int index = rand() % cityData.size();
        while( find(randomVector.begin(), randomVector.end(), index) != randomVector.end() ){
            index = rand() % cityData.size();
        }
        randomVector.push_back(index);
    }

    for (int i = 0; i < cityData.size(); ++i) {
        finalVector.push_back(cityData[randomVector[i]]);
    }

    return Route(finalVector);
}


void createFirstElite(){
    int quarter = FIRST_POPULATION_LENGTH / POPULATION_DIVISOR;
}

void sortVectorByDistance(vector<Route> v){
    sort(v.begin(), v.end(), [](const auto& lhs, const auto& rhs){
        return lhs.distance() < rhs.distance();
    });
}

void generateFirstPopulation(){
    for (int i = 0; i < FIRST_POPULATION_LENGTH; ++i) {
        firstPopulation.push_back(createRandomRoute());
    }
}

int main() {
    setup();
    Route route(cityData);

    route.print();

    Route randomRoute = createRandomRoute();
    Route randomRoute2 = createRandomRoute();

    randomRoute.print();
    randomRoute2.print();

    return 0;
};
