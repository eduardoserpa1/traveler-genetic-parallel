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

double TARGET_DISTANCE = 200;
int COMUNICATION_INTERVAL = 5;
int FIRST_POPULATION_LENGTH = 1024;
int POPULATION_DIVISOR = 16;
int MUTATION_FACTOR = 1;
int MUTATION_CHANCE = 50;
int NUM_THREADS = 2;
int ERA_LIMIT = 1000;

vector<Route> firstPopulation;
vector<Route> elite;
double eliteSumDistance = 0;

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
void updateEliteSumDistance();
bool isBetterElite(vector<Route> currentElite);

//IMPLEMENTATION
void setup(){
    loadCityData();
    generateFirstPopulation();
    sortVectorByDistance(&firstPopulation);
    createFirstElite();
    updateEliteSumDistance();
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

void updateEliteSumDistance(){
    double sum = 0;

    for (Route r:elite) {
        sum += r.getDistance();
    }

    eliteSumDistance = sum;
}

bool isBetterElite(vector<Route> currentElite){
    double sum = 0;

    for (Route r:currentElite) {
        sum += r.getDistance();
    }

    return sum < eliteSumDistance;
}

void train(){
    bool running = true;
    int currentEra = 0;
    vector<Route> currentPopulation;
    vector<Route> currentElite;
    int divisor = FIRST_POPULATION_LENGTH - (FIRST_POPULATION_LENGTH / (POPULATION_DIVISOR / 4));

    for (Route r:elite) {
        currentElite.push_back(r);
    }

    for (int i = 0; i < divisor; i++) {
        currentPopulation.push_back(firstPopulation[i]);
    }


    while(running) {
        vector<Route> childs;
        vector<Route> avaibleForElite;

        if(currentEra % COMUNICATION_INTERVAL == 0 && currentEra != 0){
            //cout << "Melhor rota da era " << currentEra << " na thread " << omp_get_thread_num() << ": distancia = " << currentElite[0].getDistance() << endl;
//            for(City c : currentElite[0].getCities()){
//                c.print();
//            }
            //cout << "thread " << omp_get_thread_num() << " ve melhor caso compartilhado: " << elite[0].getDistance() << endl << endl;
            vector<Route> newElite;

            for (int i = 0; i < currentElite.size(); ++i) {
                if(currentElite[i].getDistance() > elite[i].getDistance())
                    newElite.push_back(elite[i]);
                else
                    newElite.push_back(currentElite[i]);
            }

            sortVectorByDistance(&newElite);

            updateEliteSumDistance();

            if(isBetterElite(currentElite)){
                elite.clear();
                for (int i = 0; i < newElite.size(); ++i) {
                    elite.push_back(newElite[i]);
                }
            }else{
                currentElite = elite;
            }
        }

        for(int i = 0; i < FIRST_POPULATION_LENGTH / POPULATION_DIVISOR; i++){
            avaibleForElite.push_back(createRandomRoute());
        }

        //cout << currentElite.size() << endl;
        //cout << avaibleForElite.size() << endl;

        generateChilds(currentElite,avaibleForElite,&childs);

        for (int i = 0; i < currentElite.size(); ++i) {
            currentPopulation.push_back(currentElite[i]);
        }

        for (int i = 0; i < childs.size(); ++i) {
            currentPopulation.push_back(childs.at(i));
        }

        sortVectorByDistance(&currentPopulation);

        currentElite.clear();

        for (int i = 0; i < (FIRST_POPULATION_LENGTH / POPULATION_DIVISOR); ++i) {
            currentElite.push_back(currentPopulation[i]);
        }

        //cout << currentPopulation.size() << endl;

        currentPopulation.clear();

        for (int i = 0; i < divisor; ++i) {
            currentPopulation.push_back(createRandomRoute());
        }

        //if(currentEra >= ERA_LIMIT)
        if(elite[0].getDistance() <= TARGET_DISTANCE)
            running = false;

        currentEra++;
    }

}

void generateChilds(vector<Route> elite, vector<Route> avaibleForElite, vector<Route> *childs){
    for (int i = 0; i < elite.size(); ++i) {
        vector<City> fatherDNA = elite[i].getCities();
        vector<City> motherDNA = avaibleForElite[i].getCities();

        vector<City> child1,child2,child3,child4;

        int half = motherDNA.size() / 2;

        for (int j = 0; j < half; ++j) {
            child1.push_back(motherDNA[j]);
            child2.push_back(fatherDNA[j]);
            child3.push_back(fatherDNA[half + j]);
            child4.push_back(motherDNA[half + j]);
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

        indexCrossover = 0;
        for (int j = half; j < fatherDNA.size(); ++j) {

            if(!containsCity(child3,motherDNA[j - half])){
                child3.push_back(motherDNA[j - half]);
            }else{
                while(containsCity(child3,fatherDNA[indexCrossover])){
                    indexCrossover++;
                }
                child3.push_back(fatherDNA[indexCrossover]);
            }
        }

        indexCrossover = 0;
        for (int j = half; j < motherDNA.size(); ++j) {

            if(!containsCity(child4,fatherDNA[j - half])){
                child4.push_back(fatherDNA[j - half]);
            }else{
                while(containsCity(child4,motherDNA[indexCrossover])){
                    indexCrossover++;
                }
                child4.push_back(motherDNA[indexCrossover]);
            }
        }

        Route route1 = Route(child1);
        Route route2 = Route(child2);
        Route route3 = Route(child3);
        Route route4 = Route(child4);

        makeMutation(&route1);
        makeMutation(&route2);
        makeMutation(&route3);
        makeMutation(&route4);

        childs->push_back(route1);
        childs->push_back(route2);
        childs->push_back(route3);
        childs->push_back(route4);
    }
}

void makeMutation(Route *r){
    if((rand() % 100) < MUTATION_CHANCE) {
        for (int i = 0; i < MUTATION_FACTOR; ++i) {
            int indexA = rand() % cityData.size();
            int indexB = rand() % cityData.size();
            City aux = r->getCities().at(indexA);
            r->getCities().at(indexA) = r->getCities().at(indexB);
            r->getCities().at(indexB) = aux;
        }
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

    omp_set_num_threads(NUM_THREADS);

    int i;

    double starttime = omp_get_wtime();

#pragma omp parallel private ( i ) shared ( elite, eliteSumDistance )
#pragma omp for schedule (dynamic)
    for (i = 0; i < NUM_THREADS; ++i){

        train();
    }

    double finaltime = omp_get_wtime();

    cout << "tempo demorado para atingir a rota minima de " << TARGET_DISTANCE << ": " << finaltime - starttime << "ms" << endl;


    return 0;
};
