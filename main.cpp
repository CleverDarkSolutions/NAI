#include <iostream>
#include <functional>
#include <any>
#include <map>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>

int main(int argc, char **argv) {
    std::cout<<"Test"<<std::endl;

    using namespace std;
    vector<double> arguments;

//    transform(argv+2, argv+argc, back_inserter(arguments), [](auto s){return stod(s);});

    for (int i = 2; i < argc; i++){
        arguments.push_back(stod(argv[i]));
    }

    for (auto e : arguments){
        cout << "func: " << argv[1] << endl;
        cout << "arg: " << e << endl;
    }
    map<string, function< double(vector<double>)> > myFunctions;
    myFunctions["add"] = [](vector<double> arg) { return arg.at(0) + arg.at(1); };
    myFunctions["sin"] = [](vector<double> arg) { return sin(arg.at(0)); };
    myFunctions["mod"] = [](vector<double> arg) {return (int)arg.at(0) % (int)arg.at(1);};



    return 0;
}
