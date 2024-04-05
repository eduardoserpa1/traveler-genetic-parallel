#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "City.h"
#include "Route.h"

using namespace std;

//GLOBAL
vector<City> cityData;
const string filename = "../database/estados-30.csv";
int ERA_LIMIT = 1000;
int FIRST_POPULATION_LENGTH = 1024 * 4;
int POPULATION_DIVISOR = 8;
vector<Route> firstPopulation;
vector<Route> elite;

//DECLARATIONS
void setup();
void loadCityData();
Route createRandomRoute();
void createFirstElite();
void sortVectorByDistance(vector<Route> *v);
void generateFirstPopulation();
void train();
void generateChilds(vector<Route> elite, vector<Route> *childs);

//IMPLEMENTATION
void setup(){
    loadCityData();
    generateFirstPopulation();
    sortVectorByDistance(&firstPopulation);
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
        //Condição do while não está verificando e gerando individuos com cidades duplicadas
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
    for (int i = 0; i < quarter; ++i) {
        elite.push_back(firstPopulation[i]);
    }
}

void sortVectorByDistance(vector<Route> *v){
    sort( v->begin(), v->end(), [](Route &a, Route &b){ return (a.getDistance() < b.getDistance());});
}

void generateFirstPopulation(){
    for (int i = 0; i < FIRST_POPULATION_LENGTH; ++i) {
        firstPopulation.push_back(createRandomRoute());
    }
}

void train(){
    bool running = true;
    int currentEra = 0;
    vector<Route> currentPopulation;
    vector<Route> currentElite;
    int divisor = FIRST_POPULATION_LENGTH - (FIRST_POPULATION_LENGTH / (POPULATION_DIVISOR / 2));

    for (Route r:elite) {
        currentElite.push_back(r);
    }

    for (int i = 0; i < divisor; i++) {
        currentPopulation.push_back(firstPopulation[i]);
    }


    while(running) {
        vector<Route> childs;

        cout << "Melhor rota da era " << currentEra << ": distancia = " << currentElite[0].getDistance() << endl;
        for(City c : currentElite[0].getCities()){
            c.print();
        }

        generateChilds(currentElite,&childs);

        for (int i = 0; i < currentElite.size(); ++i) {
            currentPopulation.push_back(currentElite[i]);
            currentPopulation.push_back(childs.at(i));
        }

        sortVectorByDistance(&currentPopulation);

        currentElite.clear();

        for (int i = 0; i < FIRST_POPULATION_LENGTH / POPULATION_DIVISOR; ++i) {
            currentElite.push_back(currentPopulation[i]);
        }

        currentPopulation.clear();

        for (int i = 0; i < divisor; ++i) {
            currentPopulation.push_back(createRandomRoute());
        }

        if(currentEra >= ERA_LIMIT)
            running = false;

        currentEra++;
    }

    elite.clear();
    for(Route r: currentElite){
        elite.push_back(r);
    }
}

void generateChilds(vector<Route> elite, vector<Route> *childs){
    for (int i = 0; i < elite.size() / 2; ++i) {
        vector<City> fatherDNA = elite[i].getCities();
        vector<City> motherDNA = elite[i+1].getCities();

        vector<City> child1;
        vector<City> child2;

        int half = motherDNA.size() / 2;

        for (int j = 0; j < half; ++j) {
            child1.push_back(motherDNA[j]);
            child1.push_back(fatherDNA[j + half]);
            child2.push_back(fatherDNA[j]);
            child2.push_back(motherDNA[j + half]);
        }

        childs->push_back(Route(child1));
        childs->push_back(Route(child2));
    }
}

int main() {
    setup();

    train();

    return 0;
};
