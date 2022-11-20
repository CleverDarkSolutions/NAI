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

double scale(double number, double oldMax, double oldMin, double newMin, double newMax)
{
    double OldRange = (oldMax - oldMin);
    double NewRange = (newMax - newMin);
    return (((number - oldMin) * NewRange) / OldRange) + newMin;
}

auto randomSingularXY = [](double a, double b) {
    uniform_real_distribution<> dis(a, b);
    return pair<double, double>(dis(mt_generator), dis(mt_generator));
};

auto normalDistSingularXY = [](double a, double b) {
    normal_distribution<> dis(a, 0.1);
    normal_distribution<> dis2(b, 0.1);

    return pair<double, double>(dis(mt_generator), dis2(mt_generator));
};

auto randomVectorXY = [](pair<double, double> p, int a, int b) -> vector<pair<double, double>> {
    normal_distribution<> dis(a, b);
    return {pair<double, double>(dis(mt_generator), dis(mt_generator))};
};

tuple<double,double,double> bruteForce (myfunction_t function, vector<double> domain, int maxIterations=1000) {

    FILE *fp = NULL;
    fp = fopen("bruteforce.txt", "a");

    auto currentPair = randomSingularXY(domain.at(0), domain.at(1));
    auto bestPair = currentPair;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < maxIterations; ++i) {
        if (function(currentPair) < function(bestPair)) {
            bestPair = currentPair;
        }
        currentPair = randomSingularXY(domain.at(0), domain.at(1));
    }
    fclose(fp);
    return make_tuple(function(bestPair),bestPair.first, bestPair.second);
};

tuple<double,double,double> hillClimbing(myfunction_t function, vector<double> domain, int maxIterations=1000){

    FILE *fp = NULL;
    fp = fopen("climbing.txt", "a");

    pair<double,double> sk = normalDistSingularXY(domain.at(0), domain.at(1));


    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < maxIterations; i++){
        auto tk = randomVectorXY(sk, domain.at(0), domain.at(1));
        auto bestNeighbour = *min_element(
                tk.begin(),
                tk.end(),
                [function](auto domainStart, auto domainEnd) {
                    return function(domainStart) > function(domainEnd);
                }
        );
        if (function(bestNeighbour) < function(sk))
            sk = bestNeighbour;
    }

    fclose(fp);
    return make_tuple(function(sk),sk.first, sk.second);
}

tuple<double,double,double> simulatedAnnealing (myfunction_t function, vector<double> domain, int maxIterations=1000) {
    FILE *fp = NULL;
    fp = fopen("annealing.txt", "a");

    vector<pair<double, double>> ArrayOfXY;
    uniform_real_distribution<double> rand01(0, 1);

    auto sk = normalDistSingularXY(domain.at(0), domain.at(1));
    ArrayOfXY.push_back(sk);
    auto prevSK = sk;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < maxIterations - 1; i++) {
        double uk = rand01(mt_generator);
        pair<double, double> tk = normalDistSingularXY(tk.first, tk.second); // ----
        if (function(tk) <= function(sk)) {
            sk = tk;
            ArrayOfXY.push_back(sk);
        } else {
            if (uk < exp(-(abs(function(tk) - function(prevSK))) / (scale(i, maxIterations, 1, abs((domain.at(0)+domain.at(1)/2)), 0.0000001) / 1000))) {
                sk = tk;
                ArrayOfXY.push_back(sk);
            }
            else{
                sk = prevSK;
            }
        }
        prevSK = sk;
    }
    fclose(fp);
    return make_tuple(function(sk),sk.first, sk.second);
};

int main(int argc, char **argv){
    map<string, myfunction_t> myFunctions;
    map<string, vector<double>> domain;

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
    myFunctions["ackley"] = [](pair<double, double> xy) {
        return ( -20 * exp(-0.2 * sqrt(0.5 * pow(xy.first,2) + pow(xy.second,2))) - exp(0.5 * (cos(2*M_1_PI*xy.first) + cos(2*M_1_PI*xy.second))) + exp(1) + 20 );
    };

    domain["beale"] = {-4.5,4.5};
    domain["himmel"] = {-5,5};
    domain["threeHumpCamel"] = {-5,5};
    domain["matyas"] = {-10,10};
    domain["ackley"] = {-5,5};

    if( remove( "climbing.txt" ) != 0 )
        cout << "Creating file\n";
    if( remove( "annealing.txt" ) != 0 )
        cout << "Creating file\n";
    if( remove( "bruteforce.txt" ) != 0 )
        cout << "Creating file\n";

    try {
        vector<string> arguments(argv, argv + argc);
        auto selectedFunction = arguments.at(1);
        cout << "BruteForce: ";
        cout << get<0>(bruteForce(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "X: "<<get<1>(bruteForce(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "Y: "<<get<2>(bruteForce(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "HillClimbing: ";
        cout << get<0>(hillClimbing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "X: "<<get<1>(hillClimbing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "Y: "<<get<2>(hillClimbing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "Simulated Annealing: ";
        cout << get<0>(simulatedAnnealing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "X: "<<get<1>(simulatedAnnealing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
        cout << "Y: "<<get<2>(simulatedAnnealing(myFunctions.at(selectedFunction), domain.at(selectedFunction), 100000)) << endl;
    }
    catch (std::out_of_range aor) {
        cout << "Error, possible arguments:  ";
        for (auto [k, v] : myFunctions) cout << k << ", ";
        cout << endl;
        return 1;
    }
    return 0;
}