#include <iostream>

#include <functional>

#include <map>

#include <math.h>

#include <random>

#include <chrono>

using namespace std;
using myfunction_t = std:: function < double(pair < double, double > ) > ;
std::random_device rd;
std::mt19937 mt_generator(rd());
using chromosome_t = std::vector < int > ;
using population_t = std::vector < chromosome_t > ;

double scale(double number, double oldMax, double oldMin, double newMin, double newMax) {
  double OldRange = (oldMax - oldMin);
  double NewRange = (newMax - newMin);
  return (((number - oldMin) * NewRange) / OldRange) + newMin;
}

population_t populate(int popSize, int chromSize) {
  srand(time(nullptr));
  population_t population;
  for (int i = 0; i < popSize; i++) {
    chromosome_t chromosome;
    for (int j = 0; j < chromSize; j++) {
      chromosome.push_back(rand() % 2);
    }
    population.push_back(chromosome);
  }
  return population;
}

pair < double, double > translate(chromosome_t chromosome) {
  pair < double, double > result = {
    0,
    0
  };
  bool flagNegativeX = false;
  if (chromosome.at(0) == 1) flagNegativeX = true;
  bool flagNegativeY = false;
  if (chromosome.at(1) == 1) flagNegativeY = true;

  int splitpoint = chromosome.size() / 2;

  int j = 0;
  for (int i = splitpoint; i > 1; i--) {
    result.first += (chromosome.at(i) * pow(2, j));
    j++;
  }
  j = 0;
  for (int i = chromosome.size() - 1; i > splitpoint; i--) {
    result.second += (chromosome.at(i) * pow(2, j));
    j++;
  }

  if (flagNegativeX) {
    result.first *= -1;
  }
  if (flagNegativeY) {
    result.second *= -1;
  }
  result.first = result.first / 100000000000000;
  result.second = result.second / 100000000000000;
  return result;
}

auto genetic_algorithm = [](
  auto initial_population, auto fitness, auto term_condition,
  auto selection,
  double p_crossover,
  auto crossover,
  double p_mutation,
  auto mutation,
  myfunction_t
  function,
  vector < double > domain,
  double goal) {
  using namespace std;
  uniform_real_distribution < double > uniform(0.0, 1.0);
  auto population = initial_population;
  vector < double > population_fit = fitness(population, function, domain, goal);
  while (!term_condition(population, population_fit)) {
    auto parents_indexes = selection(population_fit);
    decltype(population) new_population;
    for (int i = 0; i < parents_indexes.size(); i += 2) {
      decltype(initial_population) offspring = {
        population[i],
        population[i + 1]
      };
      if (uniform(mt_generator) < p_crossover) {
        offspring = crossover(offspring);
      }
      for (auto chromosome: offspring) new_population.push_back(chromosome);
    }
    for (auto & chromosome: new_population) {
      chromosome = mutation(chromosome, p_mutation);
    }
    population = new_population;
    population_fit = fitness(population, function, domain, goal);
  }
  return population;
};
std::vector < double > fitness_function(population_t pop, myfunction_t
  function, vector < double > domain, double goal) {
  vector < double > result;
  pair < double, double > currPair;
  for (int i = 0; i < pop.size(); i++) {
    currPair = translate(pop.at(i));
    if (currPair.first > domain.at(0) && currPair.first < domain.at(1) && currPair.second > domain.at(0) && currPair.second < domain.at(1)) {
      result.push_back(1000000 - function (currPair));
    } else {
      result.push_back(1 - scale(abs(currPair.first), 100, domain.at(1), 0, 1) + 1 - scale(abs(currPair.second), 100, domain.at(1), 0, 1));
    }
  }
  return result;
}
std::vector < int > selection_empty(std::vector < double > fitnesses) {
  return {};
}
std::vector < chromosome_t > crossover_empty(std::vector < chromosome_t > parents) {
  return parents;
}
chromosome_t mutation_empty(chromosome_t parents, double p_mutation) {
  return parents;
}

int main(int argc, char ** argv) {

  map < string, myfunction_t > myFunctions;
  map < string, vector < double >> domain;
  map < string, double > goal;

  myFunctions["beale"] = [](pair < double, double > xy) {
    return (pow((1.5 - xy.first + (xy.first * xy.second)), 2)) + (pow(2.25 - xy.first + (xy.first * pow(xy.second, 2)), 2)) + (pow(2.625 - xy.first + xy.first * pow(xy.second, 3), 2));
  };
  myFunctions["himmel"] = [](pair < double, double > xy) {
    return pow(pow(xy.first, 2) + xy.second - 11, 2) + pow(xy.first + pow(xy.second, 2) - 7, 2);
  };
  myFunctions["threeHumpCamel"] = [](pair < double, double > xy) {
    return ((2 * pow(xy.first, 2) - (1.05 * pow(xy.first, 4)) + ((pow(xy.first, 6)) / 6) + (xy.first * xy.second + (pow(xy.second, 2)))));
  };
  myFunctions["matyas"] = [](pair < double, double > xy) {
    return (0.26 * (pow(xy.first, 2) + pow(xy.second, 2)) - (0.48 * xy.first * xy.second));
  };

  domain["beale"] = {
    -4.5,
    4.5
  };
  domain["himmel"] = {
    -5,
    5
  };
  domain["threeHumpCamel"] = {
    -5,
    5
  };
  domain["matyas"] = {
    -10,
    10
  };

  goal["beale"] = 0;
  goal["himmel"] = 0;
  goal["threeHumpCamel"] = 0;
  goal["matyas"] = 0;

  population_t population = populate(100, 100 + (23316 % 10) * 2);

  try {
    vector < string > arguments(argv, argv + argc);
    auto selectedFunction = arguments.at(1);

    auto result = genetic_algorithm(population,
      fitness_function,
      [](auto a, auto b) {
        return true;
      },
      selection_empty,
      1.0,
      crossover_empty,
      0.01,
      mutation_empty,
      myFunctions.at(selectedFunction),
      domain.at(selectedFunction),
      goal.at(selectedFunction));
    for (chromosome_t chromosome: result) {
      cout << "[";
      for (int p: chromosome) {
        cout << p;
      }
      cout << "]";
    }
    cout << endl;
  } catch (std::out_of_range aor) {
    cout << "Wrong arg. Choose from:  ";
    for (auto[k, v]: myFunctions) cout << k << ", ";
    cout << endl;
    return 1;
  }

  return 0;
}