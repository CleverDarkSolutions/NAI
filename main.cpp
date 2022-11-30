#include <iostream>
#include <functional>
#include <map>
#include <math.h>
#include <random>
#include <chrono>

using namespace std;
using myfunction_t = std::function<double(pair<double, double>)>;
std::random_device rd;
std::mt19937 mt_generator(rd());
using chromosome_t = std::vector<int>;
using population_t = std::vector<chromosome_t>;

double scale(double number, double oldMax, double oldMin, double newMin, double newMax)
{
    double OldRange = (oldMax - oldMin);
    double NewRange = (newMax - newMin);
    return (((number - oldMin) * NewRange) / OldRange) + newMin;
}

population_t populate(int popSize, int chromSize){
    srand(time(nullptr));
    population_t population;
    for(int i=0;i<popSize;i++){
        chromosome_t chromosome;
        for(int j=0;j<chromSize;j++){
            chromosome.push_back(rand()%2);
        }
        population.push_back(chromosome);
    }
    return population;
}

pair<double, double> translate(chromosome_t chromosome){
    pair<double, double> result={0,0};
    bool flagNegativeX = false;
    if(chromosome.at(0)==1)flagNegativeX=true;
    bool flagNegativeY = false;
    if(chromosome.at(1)==1)flagNegativeY=true;

    int splitpoint = chromosome.size()/2;

    int j = 0;
    for(int i=splitpoint;i>1;i--){
        result.first += (chromosome.at(i) * pow(2,j));
        j++;
    }
    j=0;
    for (int i = chromosome.size()-1; i > splitpoint; i--) {
        result.second += (chromosome.at(i) * pow(2,j));
        j++;
    }

    if(flagNegativeX){
        result.first*=-1;
    }
    if(flagNegativeY){
        result.second*=-1;
    }
    result.first = result.first/1000000000000000;
    result.second = result.second/1000000000000000;
    return result;
}

auto genetic_algorithm = [](
        auto initial_population, auto fitness, auto term_condition,
        auto selection,
        double p_crossover,
        auto crossover,
        double p_mutation,
        auto mutation,
        myfunction_t function,
        vector<double> domain,
        double goal){
    using namespace std;
    uniform_real_distribution<double> uniform(0.0,1.0);
    auto population = initial_population;
    vector<double> population_fit = fitness(population, function, domain, goal);
    while (!term_condition(population,population_fit)) {
        auto parents_indexes = selection(population_fit);
        decltype(population) new_population;
        for (int i = 0 ; i < parents_indexes.size(); i+=2) {
            decltype(initial_population) offspring = {population[i],population[i+1]};
            if (uniform(mt_generator) < p_crossover) {
                offspring = crossover(offspring);
            }
            for (auto chromosome : offspring) new_population.push_back(chromosome);
        }
        for (auto & chromosome : new_population) {
            chromosome = mutation(chromosome,p_mutation);
        }
        population = new_population;
        population_fit = fitness(population, function, domain, goal);
    }
    return population;
};
std::vector<double> fitness_function(population_t pop, myfunction_t function, vector<double> domain, double goal){
    vector<double> result;
    pair <double, double> currPair;
    for (int i = 0; i < pop.size(); i++){
        currPair = translate(pop.at(i));
        if (currPair.first > domain.at(0) && currPair.first < domain.at(1) && currPair.second > domain.at(0) && currPair.second < domain.at(1)){
            result.push_back(10000 - function(currPair));
        }
        else {
            result.push_back(1 - scale(abs(currPair.first) ,100, domain.at(1), 0, 1) + 1 - scale(abs(currPair.second) ,100, domain.at(1), 0, 1));
        }
    }
    int goodCount = 0;
    for (double p: result) {
        if(p>9990){
            goodCount++;
        }
    }
    cout << goodCount;
    return result;
}
std::vector<int> selection(std::vector<double> fitnesses) {
    uniform_real_distribution<> randomNumb(0.0,1.0);
    double R = randomNumb(mt_generator);
    double S = 0;
    double P = 0;
    double lastP = 0;
    for (double elem : fitnesses){
        S += elem;
    }
    double p = 0;
    std::vector<int> resVector;
    for (int i = 0; i < fitnesses.size(); i++) {
        p = fitnesses.at(i) / S;
        P = lastP + p;
        if(lastP <= R && lastP <= P){
            resVector.push_back(i);
        }
        lastP = P;
    }
    return resVector;
}
std::vector<chromosome_t > crossover_empty(std::vector<chromosome_t > parents) {
    uniform_real_distribution<double> randomPoint(0,parents.at(0).size());
    int swapPoint = randomPoint(mt_generator);
    for (int i = swapPoint; i < parents.at(0).size(); i++) {
        int temp = parents.at(0).at(i);
        parents.at(0).at(i) = parents.at(1).at(i);
        parents.at(1).at(i) = temp;
    }
    return parents;
}
chromosome_t mutation_empty(chromosome_t parent, double p_mutation) {
    uniform_real_distribution<> randomNumb(0.0,1.0);
    uniform_real_distribution<> randomCount(0, 5);
    uniform_real_distribution<> randomPoint(0, parent.size());
    if(randomNumb(mt_generator) < p_mutation){
        for (int i = 0; i < randomCount(mt_generator); i++) {
            int currPoint = randomPoint(mt_generator);
            if (parent.at(currPoint) == 0){
                parent.at(currPoint) = 1;
            } else{
                parent.at(currPoint) = 0;
            }
        }
    }

    return parent;
}

int main(int argc, char **argv){

    map<string, myfunction_t> myFunctions;
    map<string, vector<double>> domain;
    map<string, double> goal;

    myFunctions["beale"] = [](pair<double, double> xy) {
        return (pow((1.5 - xy.first + (xy.first * xy.second)),2)) + (pow(2.25 - xy.first + (xy.first * pow(xy.second,2)),2))+(pow(2.625 - xy.first + xy.first * pow(xy.second,3),2));
    };
    myFunctions["himmel"] = [](pair<double, double> xy) {
        return pow(pow(xy.first,2) + xy.second - 11,2) + pow(xy.first + pow(xy.second,2) - 7,2);
    };
    myFunctions["threeHumpCamel"] = [](pair<double, double> xy) {
        return ((2 * pow(xy.first,2) - (1.05 * pow(xy.first,4)) + ((pow(xy.first,6))/6) + (xy.first*xy.second + (pow(xy.second,2)))));
    };
    myFunctions["matyas"] = [](pair<double, double> xy) {
        return (0.26 * (pow(xy.first, 2) + pow(xy.second, 2)) - (0.48 * xy.first * xy.second));
    };

    domain["beale"] = {-4.5,4.5};
    domain["himmel"] = {-5,5};
    domain["threeHumpCamel"] = {-5,5};
    domain["matyas"] = {-10,10};

    goal["beale"] = 0;
    goal["himmel"] = 0;
    goal["threeHumpCamel"] = 0;
    goal["matyas"] = 0;

   population_t population = populate(1000, 100+(23315%10)*2);


   try {
       vector<string> arguments(argv, argv + argc);
       auto selectedFunction = arguments.at(1);
       population = genetic_algorithm(population,
                                       fitness_function,
                                       [](auto a, auto b) {
                                           for (auto elem: b) {
                                                if(elem > 9999){
                                                    return true;
                                                }
                                           };
                                           return false;
                                           },
                                       selection,
                                       1.0,
                                       crossover_empty,
                                       0.05,
                                       mutation_empty,
                                       myFunctions.at(selectedFunction),
                                       domain.at(selectedFunction),
                                       goal.at(selectedFunction));
   }
   catch (std::out_of_range aor) {
       cout << "Wrong arg. Choose from: ";
       for (auto [k, v] : myFunctions) cout << k << ", ";
       cout << endl;
       return 1;
   }

   return 0;
}