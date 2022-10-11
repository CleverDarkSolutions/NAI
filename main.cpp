#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>
using myfunction_t = std::function<double(std::vector<double>)>;
void calculate(std::vector<double> numbers, myfunction_t func) {
    using namespace std;
    cout << func(numbers) << endl;
}
int main(int argc, char **argv) {
    using namespace std;
    map<string, myfunction_t> formaters;
    formaters["mod"] = [](vector<double> numbers) { return (int)numbers.front() % (int)numbers.back();};
    formaters["add"] = [](vector<double> numbers) { return  numbers.front() + numbers.back();};
    formaters["sin"] = [](vector<double> numbers) { return sin(numbers.front());};
    try {
        vector<string> arguments(argv, argv + argc);
        auto chosen_function = arguments.at(2);
        if (arguments.size() > 5){
            cout << "Too much args " << arguments.size() - 1;
            return 1;
        }
        else if(chosen_function == "sin" && arguments.size() > 4){
            cout << "Too much args, sin takes only one";
            return 1;
        }
        else if (arguments.at(1) != "lab1"){
            cout << "No lab1 specified at the beginning";
            return 1;
        }
        vector<double> numbers = {{stod(arguments.at(3)), stod(arguments.back())}};
        (numbers, formaters.at(chosen_function));
    } catch (std::out_of_range aor) {
        cout << "Incorrect arg. Choose from: ";
        for (auto [k, v] : formaters) cout << " " << k;
        cout << endl;
        return 1;
    }
    return 0;
}
