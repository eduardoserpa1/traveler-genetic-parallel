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
int ERA_LIMIT = 10000;
int FIRST_POPULATION_LENGTH = 1024;
int POPULATION_DIVISOR = 16;
int MUTATION_FACTOR = 4;
int MUTATION_CHANCE = 80;
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
void generateChilds(vector<Route> elite, vector<Route> avaibleForElite, vector<Route> *childs);
bool containsCity(vector<City> v, City c);
void makeMutation(Route *r);

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
        while( find(randomVector.begin(), randomVector.end(), index) != randomVector.end() ){
            index = rand() % cityData.size();
        }
        randomVector.push_back(index);
    }

    for (int i = 0; i < cityData.size(); ++i) {
        finalVector.push_back(cityData[randomVector[i]]);
    }

//    cout << "[";
//    for(int i:randomVector){
//        cout << i << ", ";
//    }
//    cout << "]";

    return Route(finalVector);
}

void createFirstElite(){
    int divisor = FIRST_POPULATION_LENGTH / POPULATION_DIVISOR;
    for (int i = 0; i < divisor; ++i) {
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
        //cout << currentPopulation.size() << endl;
        vector<Route> childs;
        vector<Route> avaibleForElite;

        if(currentEra % 50 == 0){
            cout << "Melhor rota da era " << currentEra << ": distancia = " << currentElite[0].getDistance() << endl;
//            for(City c : currentElite[0].getCities()){
//                c.print();
//            }
            cout << endl << "-----------------------------------" << endl;
        }

        for(int i = 0; i < FIRST_POPULATION_LENGTH / POPULATION_DIVISOR; i++){
            avaibleForElite.push_back(createRandomRoute());
        }

        generateChilds(currentElite,avaibleForElite,&childs);

        for (int i = 0; i < currentElite.size(); ++i) {
            currentPopulation.push_back(currentElite[i]);
            currentPopulation.push_back(childs.at(i));
        }

        sortVectorByDistance(&currentPopulation);

        currentElite.clear();

        for (int i = 0; i < (FIRST_POPULATION_LENGTH / POPULATION_DIVISOR) / 2; ++i) {
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

void generateChilds(vector<Route> elite, vector<Route> avaibleForElite, vector<Route> *childs){
    for (int i = 0; i < elite.size(); ++i) {
        vector<City> fatherDNA = elite[i].getCities();
        vector<City> motherDNA = avaibleForElite[i].getCities();

        vector<City> child1;
        vector<City> child2;

        int half = motherDNA.size() / 2;

        for (int j = 0; j < half; ++j) {
            child1.push_back(motherDNA[j]);
            //child1.push_back(fatherDNA[j + half]);
            child2.push_back(fatherDNA[j]);
            //child2.push_back(motherDNA[j + half]);
        }


        int indexCrossover = half;
        for (int j = half; j < motherDNA.size(); ++j) {

            if(!containsCity(child1,fatherDNA[j])){
                child1.push_back(fatherDNA[j]);
            }else{
                while(containsCity(child1,motherDNA[indexCrossover])){
                    indexCrossover++;
                }
                child1.push_back(motherDNA[indexCrossover]);
            }
        }

        indexCrossover = half;
        for (int j = half; j < fatherDNA.size(); ++j) {

            if(!containsCity(child2,motherDNA[j])){
                child2.push_back(motherDNA[j]);
            }else{
                while(containsCity(child2,fatherDNA[indexCrossover])){
                    indexCrossover++;
                }
                child2.push_back(fatherDNA[indexCrossover]);
            }
        }
        Route route1 = Route(child1);
        Route route2 = Route(child2);

        if((rand() % 100) < MUTATION_CHANCE){
            makeMutation(&route1);
            makeMutation(&route2);
        }

        childs->push_back(route1);
        childs->push_back(route2);
    }
}

void makeMutation(Route *r){
    for (int i = 0; i < MUTATION_FACTOR; ++i) {
        int indexA = rand() % cityData.size();
        int indexB = rand() % cityData.size();
        City aux = r->getCities().at(indexA);
        r->getCities().at(indexA) = r->getCities().at(indexB);
        r->getCities().at(indexB) = aux;
    }
}

bool containsCity(vector<City> v, City c){
    for(City city : v)
        if(city.equals(c))
            return true;
    return false;
}

int main() {
    setup();

    train();


    City c1 = City("A",0,0);
    City c2 = City("B",0,0);
    City c3 = City("C",0,0);
    City c4 = City("D",0,0);

    vector<City> fatherDNA;
    fatherDNA.push_back(c1);
    fatherDNA.push_back(c2);
    fatherDNA.push_back(c3);
    fatherDNA.push_back(c4);

    Route fatherTest = Route(fatherDNA);

    vector<City> motherDNA;
    motherDNA.push_back(c4);
    motherDNA.push_back(c3);
    motherDNA.push_back(c2);
    motherDNA.push_back(c1);

    Route motherTest = Route(motherDNA);

    vector<Route> eliteTest;
    eliteTest.push_back(fatherTest);
    eliteTest.push_back(motherTest);

    vector<Route> childs;

    //generateChilds(eliteTest,&childs);

    for(Route r : childs){
        r.print();
    }


    return 0;
};
