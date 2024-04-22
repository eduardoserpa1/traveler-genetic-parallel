#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cstdlib>
#include <omp.h>
#include "City.h"
#include "Route.h"

using namespace std;

//GLOBAL
vector<City> cityData;
const string filename = "../database/estados-30.csv";

//ALGORITMO
int POPULATION_LENGTH = 1000;
int ELITE_LENGTH = 50;
int MUTATION_FACTOR = 2;
int MUTATION_CHANCE = 80;
//PARALLELIZACAO
int NUM_THREADS = 4;
int COMUNICATION_INTERVAL = 30;
double TARGET_DISTANCE = 215;

//MEMORIA COMPARTILHADA
vector<Route> elite;
double eliteSumDistance = 0;

//DECLARATIONS
void loadCityData();
Route createRandomRoute();
void sortVectorByDistance(vector<Route> *v);
void generateRandomPopulation(vector<Route> *firstPopulation);
void train(vector<Route> *firstPopulation);
void generateChilds(vector<Route> *elite, vector<Route> *avaibleForElite, vector<Route> *childs);
bool containsCity(vector<City> *v, City *c);
void makeMutation(Route *r);
void updateEliteSumDistance();
bool isBetterElite(vector<Route> *currentElite);
void makeChild(vector<City> *child, vector<City> *mainDNA, vector<City> *secondDNA, int indexCrossover, int half);

//IMPLEMENTATION
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

void sortVectorByDistance(vector<Route> *v){
    sort( v->begin(), v->end(), [](Route &a, Route &b){ return (a.getDistance() < b.getDistance());});
}

void generateRandomPopulation(vector<Route> *firstPopulation){
    for (int i = 0; i < POPULATION_LENGTH; ++i) {
        firstPopulation->push_back(createRandomRoute());
    }
}

void updateEliteSumDistance(){
    double sum = 0;

    for (Route r:elite) {
        sum += r.getDistance();
    }

    eliteSumDistance = sum;
}

bool isBetterElite(vector<Route> *currentElite){
    double sum = 0;

    for (Route r:*currentElite) {
        sum += r.getDistance();
    }

    return sum < eliteSumDistance;
}


void train(vector<Route> *firstPopulation){
    //cout << "thread " << omp_get_thread_num() << " iniciou o treinamento, melhor caso inicial = " << firstPopulation[0].getDistance() << endl;
    bool running = true;
    int currentEra = 0;
    vector<Route> currentPopulation;
    vector<Route> currentElite;
    int divisor = POPULATION_LENGTH - ELITE_LENGTH;

    for (int i = 0; i < ELITE_LENGTH; ++i) {
        currentElite.push_back(firstPopulation->at(i));
    }

    for (int i = ELITE_LENGTH; i < divisor + ELITE_LENGTH; i++) {
        currentPopulation.push_back(firstPopulation->at(i));
    }


    while(running) {
        vector<Route> childs;
        vector<Route> avaibleForElite;

        if(currentEra % (COMUNICATION_INTERVAL + (omp_get_thread_num() * 2)) == 0 && currentEra != 0){
            updateEliteSumDistance();

            if(isBetterElite(&currentElite)){
                vector<Route> newElite;

                for (int i = 0; i < currentElite.size(); ++i) {
                    if(elite[i].getDistance() <= currentElite[i].getDistance())
                        newElite.push_back(elite[i]);
                    else
                        newElite.push_back(currentElite[i]);
                }

                sortVectorByDistance(&newElite);
                elite.clear();
                makeMutation(&newElite[2]);
                makeMutation(&newElite[4]);
                for (int i = 0; i < newElite.size(); ++i) {
                    elite.push_back(newElite[i]);
                }
            }else{
                currentElite = elite;
            }


            cout << "Melhor rota da era " << currentEra << " na thread " << omp_get_thread_num() << ": distancia = " << currentElite[0].getDistance() << "  |  elite global: " << elite[0].getDistance() << endl;

        }

        for(int i = 0; i < ELITE_LENGTH; i++){
            avaibleForElite.push_back(currentPopulation[i]);
        }

        generateChilds(&currentElite,&avaibleForElite,&childs);

        for (int i = 0; i < currentElite.size(); ++i) {
            currentPopulation.push_back(currentElite[i]);
        }

        for (int i = 0; i < childs.size(); ++i) {
            currentPopulation.push_back(childs.at(i));
        }

        sortVectorByDistance(&currentPopulation);

        currentElite.clear();

        for (int i = 0; i < ELITE_LENGTH; ++i) {
            currentElite.push_back(currentPopulation[i]);
        }

        currentPopulation.clear();

        for (int i = 0; i < divisor; ++i) {
            currentPopulation.push_back(createRandomRoute());
        }

        if(elite.at(0).getDistance() <= TARGET_DISTANCE)
            running = false;

        currentEra++;
    }

}

void makeChild(vector<City> *child, vector<City> *mainDNA, vector<City> *secondDNA, int indexCrossover, int half){
    for (int j = mainDNA->size()/2; j <  mainDNA->size(); ++j) {

        if(!containsCity(child,&secondDNA->at(j - half))){
            child->push_back(secondDNA->at(j - half));
        }else{
            while(containsCity(child,&mainDNA->at(indexCrossover))){
                indexCrossover++;
            }
            child->push_back(mainDNA->at(indexCrossover));
        }
    }
}

void generateChilds(vector<Route> *elite, vector<Route> *avaibleForElite, vector<Route> *childs){
    for (int i = 0; i < elite->size(); ++i) {
        vector<City> fatherDNA = elite->at(i).getCities();
        vector<City> motherDNA = avaibleForElite->at(i).getCities();
        vector<City> child1,child2,child3,child4;

        int half = motherDNA.size() / 2;

        for (int j = 0; j < half; ++j) {
            child1.push_back(motherDNA[j]);
            child2.push_back(fatherDNA[j]);
            child3.push_back(fatherDNA[half + j]);
            child4.push_back(motherDNA[half + j]);
        }

        makeChild(&child1,&motherDNA,&fatherDNA,half,0);
        makeChild(&child2,&fatherDNA,&motherDNA,half,0);

        makeChild(&child3,&fatherDNA,&motherDNA,0,half);
        makeChild(&child4,&motherDNA,&fatherDNA,0,half);


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

bool containsCity(vector<City> *v, City *c){
    for(City city : *v)
        if(city.equals(*c))
            return true;
    return false;
}

int main() {
    srand((int)time(0));
    loadCityData();

    for (int it = 0; it < 1; ++it) {
        for (int numt = NUM_THREADS; numt <= NUM_THREADS; ++numt) {
            elite.clear();

            omp_set_num_threads(numt);

            int i;

            double starttime = omp_get_wtime();

            vector<vector<Route>> populations;

            for (int x = 0; x < numt; ++x) {
                vector<Route> initialPopulation;

                generateRandomPopulation(&initialPopulation);
                sortVectorByDistance(&initialPopulation);

                populations.push_back(initialPopulation);
            }

            for(int y = 0; y < ELITE_LENGTH; ++y){
                elite.push_back(populations.at(0).at(y));
            }

            updateEliteSumDistance();

#pragma omp parallel private ( i ) shared ( elite, eliteSumDistance )
#pragma omp for schedule (dynamic)
            for (i = 0; i < numt; ++i) {
                train(&populations.at(i));
            }

            double finaltime = omp_get_wtime();

            cout << endl << finaltime - starttime << "ms" << endl;
        }
        cout << endl;
    }

    return 0;
};
