#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <climits>

using namespace std;

struct city {
    int index;
    int x;
    int y;
};

int getDistance(city source, city dest) {
    return round(sqrt(pow(dest.x - source.x, 2) + pow(dest.y - source.y, 2)));
}

void findClosestCity(vector<city>& cities, vector<city>& result,  int& totalDistance) {
    city source = *cities.begin();
    vector<city>::iterator closestCity = cities.begin();
     int currentDistance = 0;
     int minDistance = 0;

    if (cities.size() > 1) {
        for (vector<city>::iterator it = cities.begin() + 1; it != cities.end(); it++) {
            // Euclidian distance
            currentDistance = getDistance(source, *it);
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

 int glouton(vector<city>& cities, vector<city>& result) {
     int totalDistance = 0;
    
    result.push_back(cities[0]);
    
    while (!cities.empty())
        findClosestCity(cities, result, totalDistance);

    // Go back to first city
    totalDistance += getDistance(result[0], result[result.size()-1]);
    result.push_back(result[0]);

    return totalDistance;
}

// printPath and progdyn made with the help of :
// https://stackoverflow.com/questions/61869112/travelling-salesman-problem-in-dynamic-programming
void printPath(vector<vector<int>>& distancesMatrix, size_t numCities,  int mask,  int pos, vector<vector<int>>& vertices) {
    // Reached initial city
    if (mask == (1 << numCities) - 1) 
        return;

     int minDistance = INT_MAX, cityIndex = 0;

    for (int i = 0; i < numCities; i++) {
        if ((mask & (1 << i)) == 0) { //Unvisited city
             int currentDistance = distancesMatrix[pos][i] + vertices[mask | (1 << i)][i];

            if (currentDistance < minDistance) {
                minDistance = currentDistance;
                cityIndex = i;
            }
        }
    }
    cout << cityIndex << endl;
    printPath(distancesMatrix, numCities, mask | (1 << cityIndex), cityIndex, vertices);
}

 int progdyn(vector<vector<int>>& distancesMatrix, size_t numCities,  int mask,  int pos, vector<vector<int>>& vertices) {
    // Return to initial city
    if (mask == (1 << numCities) - 1)
        return distancesMatrix[pos][0];

    if (vertices[mask][pos] != -1)
        return vertices[mask][pos];

     int minDistance = INT_MAX;

    for (int i = 0; i < numCities; i++) {
        if ((mask & (1 << i)) == 0) {
             int distance = distancesMatrix[pos][i] + progdyn(distancesMatrix, numCities, mask | (1 << i), i, vertices);
            minDistance = min(minDistance, distance);
        }
    }
    return vertices[mask][pos] = minDistance;
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
    for (int i = 1; i < argc; i++) {
       string arg(argv[i]);
       if (arg == "-a") {
           prog_args.algo = argv[i + 1]; i++;
       }
       else if (arg == "-e") {
           prog_args.file_path = argv[i + 1]; i++;
       }
       else if (arg == "-p") {
           prog_args.print_res = true;
       }
       else if (arg == "-t") {
           prog_args.print_time = true;
       }
    }

    // Read cities coordinates into vector
    vector<city> cities;

    fstream ex_file(prog_args.file_path);
     int index = 0, m, n, result;

    // Don't consider first number (number of cities)
    ex_file >> m;

    while (ex_file >> m) {
        ex_file >> n;
        cities.push_back({index, m, n});
        index++;
    }
    
    using namespace chrono;
    if (prog_args.algo == "glouton") {
        vector<city> pathResult;

        auto start = steady_clock::now();
        result = glouton(cities, pathResult);
        auto end = steady_clock::now();

        if (prog_args.print_res) {
            for (auto& c : pathResult)
                cout << c.index << endl;
        }
        if (prog_args.print_time) {
            duration<double> s = (end - start)*1000;
            cout << fixed << s.count() << endl;
        }
    } else if (prog_args.algo == "progdyn") {
        vector<vector<int>> distanceMatrix;
        for (int i = 0; i < cities.size(); i++) {
            distanceMatrix.push_back(vector<int>());
            for (int j = 0; j < cities.size(); j++) {
                distanceMatrix[i].push_back(getDistance(cities[i], cities[j]));
            }
        }

        vector<vector<int>> vertices;
        for (int i = 0; i < (1 << cities.size()); i++) {
            vertices.push_back(vector<int>());
            for (int j = 0; j < cities.size(); j++) {
                vertices[i].push_back(-1);
            }
        }

        auto start = steady_clock::now();
        result = progdyn(distanceMatrix, cities.size(), 1, 0, vertices);
        auto end = steady_clock::now();

        if (prog_args.print_res) {
            cout << 0 << endl;
            printPath(distanceMatrix, cities.size(), 1, 0, vertices);
        }
        if (prog_args.print_time) {
            duration<double> s = (end - start)*1000;
            cout << fixed << s.count() << endl;
        }
    }
    
    // To print final distance
    // cout << result << endl;

}
