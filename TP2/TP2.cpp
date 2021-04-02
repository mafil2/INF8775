#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <set>
#include <regex>

using namespace std;

struct city {
    int index;
    int x;
    int y;
};

using Algo = const function<vector<city>&(vector<city>&)>&;

void run(Algo algo, vector<city> cities, bool print_res, bool print_time) {
    using namespace chrono;
    auto start = steady_clock::now();
    vector<city> result = algo(cities);
    auto end = steady_clock::now();

    if (print_time) {
        duration<double> s = end - start;
        cout << fixed << s.count() << endl;
    }

    if (print_res) {
        cout << fixed;
        for (auto n : result)
            cout << n.x << " " << n.y << endl;
    }
}

void findClosestCity(vector<city>& cities, vector<city>& result, int& totalDistance) {
    city source = *cities.begin();
    vector<city>::iterator closestCity = cities.begin();
    int currentDistance = 0;
    int minDistance = 0;

    if (cities.size() > 1) {
        for (vector<city>::iterator it = cities.begin() + 1; it != cities.end(); it++) {
            // Euclidian distance
            currentDistance = round(sqrt(pow((*it).x - source.x, 2) + pow((*it).y - source.y, 2)));
            if (it == cities.begin() + 1) {
                minDistance = currentDistance;
                closestCity = it;
            }
            else if (currentDistance < minDistance) {
                minDistance = currentDistance;
                closestCity = it;
            }
        }
        // Reached destination : new source point
        *cities.begin() = *closestCity;
        totalDistance += minDistance;
        result.push_back(*closestCity);
    }
    cities.erase(closestCity);
}

vector<city> glouton(vector<city>& cities) {
    vector<city> result;
    int totalDistance = 0;
    
    result.push_back(*cities.begin());
    
    while (!cities.empty()) {
        findClosestCity(cities, result, totalDistance);
    }

    // Go back to first city
    totalDistance += round(sqrt(pow(result[result.size()-1].x - result[0].x, 2) + pow(result[result.size()-1].y - result[result.size()-1].y, 2)));
    result.push_back(result[0]);

    cout << totalDistance << endl;

    return result;
}

//int progdyn(vector<city>& cities, vector<city>::iterator source) {
//    int distance = UINT_MAX;
//    vector<city>::iterator closestCity;
//    cities.erase(source);
//
//    //TODO
//    if (cities.size() == 2) {
//        distance = round(sqrt(pow(abs((cities[1]).x - cities[0].x), 2) + pow(abs(cities[1].y - cities[0].y), 2)));
//    }
//    else {
//        for (vector<city>::iterator it1 = cities.begin(); it1 != cities.end(); it1++) {
//            for (vector<city>::iterator it2 = cities.begin(); it2 != cities.end(); it2++) {
//                if (find(cities.begin(), cities.end(), it2) == cities.end() && it1 != it2 && it1 != source) {
//                    vector<city> citiesCopy = cities;
//                    citiesCopy.erase(it2);
//                    int currentDistance = progdyn(citiesCopy, it1) + round(sqrt(pow(abs((*it1).x - (*it2).x), 2) + pow(abs((*it1).y - (*it2).y), 2)));
//                    if (currentDistance < distance) {
//                        distance = currentDistance;
//                        closestCity = it2;
//                    }
//                }
//            }
//        }
//    cout << (*closestCity).index << endl;
//    cities.erase(closestCity);
//    }
//
//    return distance;
//}

vector<city>& approx(vector<city>& cities) {
    vector<city> result;
    int totalDistance = 0;

    result.push_back(*cities.begin());

    //TODO

    cout << totalDistance << endl;

    return result;
}


// Mainly from guide bash
int main(int argc, char* argv[]) {
    struct {
        string algo;
        string file_path;
        bool print_res{ false };
        bool print_time{ false };
    } prog_args;

    // Read program arguments
    //for (int i = 1; i < argc; i++) {
    //    string arg(argv[i]);
    //    if (arg == "-a") {
    //        prog_args.algo = argv[i + 1]; i++;
    //    }
    //    else if (arg == "-e") {
    //        prog_args.file_path = argv[i + 1]; i++;
    //    }
    //    else if (arg == "-p") {
    //        prog_args.print_res = true;
    //    }
    //    else if (arg == "-t") {
    //        prog_args.print_time = true;
    //    }
    //}

    // Read cities coordinates into vector

    vector<city> cities;

    fstream ex_file("./JeuTest/N1000_0");
    int index = 0, m, n;

    // Don't consider first number (number of cities)
    ex_file >> m;

    while (ex_file >> m) {
        ex_file >> n;
        cities.push_back({index, m, n});
        index++;
    }

    using namespace chrono;
    auto start = steady_clock::now();
    vector<city> result = glouton(cities);
    auto end = steady_clock::now();

    duration<double> s = end - start;
    cout << fixed << s.count() << endl;

    cout << fixed;
    for (city n : result)
        cout << n.index << endl;

    //Apply correct algorithm
    /*if (prog_args.algo == "brute")
        run(glouton, cities, prog_args.print_res, prog_args.print_time);
    else if (prog_args.algo == "recursif")
        run(progdyn, cities, prog_args.print_res, prog_args.print_time);
    else if (prog_args.algo == "seuil")
        run(approx, cities, prog_args.print_res, prog_args.print_time);*/

}
