#include <vector>
#include <fstream>
#include <functional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;
using Algo = const function<vector<vector<int>>* (vector<vector<int>>&)>&;

void removeRedundancy(vector<vector<int>>* result) {
    vector<vector<int>>::iterator it = result->begin();

    int previousHeight = (*it)[1];
    it = next(it);

    while (it != result->end()) {
        if ((*it)[1] == previousHeight)
            it = result->erase(it);
        else {
            previousHeight = (*it)[1];
            it = next(it);
        }
    }
}

// From guide bash
void run(Algo algo, vector<vector<int>>& buildings, bool print_res, bool print_time) {
    using namespace chrono;
    auto start = steady_clock::now();
    vector<vector<int>>* result = algo(buildings);
    removeRedundancy(result); // Make sure there are no redundant points
    auto end = steady_clock::now();

    if (print_time) {
        duration<double> s = end - start;
        cout << fixed << s.count() << endl;
    }

    if (print_res) {
        cout << fixed;
        for (auto n : *result)
            cout << n[0] << " " << n[1] << endl;
    }
    delete result;
}

// Inspired from https://www.youtube.com/watch?v=gCkbVt166iU&t=1040s&ab_channel=Sheep
vector<vector<int>>* brute(vector<vector<int>>& buildings) {
    vector<vector<int>>* result = new vector<vector<int>>();
    vector<pair<int, int>> points;

    // b[0] = x1
    // b[1] = x2
    // b[2] = h
    for (vector<int>& b : buildings) {
        points.emplace_back(b[0], -b[2]);
        points.emplace_back(b[1], b[2]);
    }
    sort(points.begin(), points.end());
    // Start at h = 0
    int lastHeight = 0, currentHeight = 0;

    // For sorted container with duplicates
    multiset<int> heightSet;

    // Last silhouette point always at h = 0
    heightSet.insert(0);

    for (pair<int, int>& p : points) {
        if (p.second < 0)
            heightSet.insert(-p.second);
        else
            heightSet.erase(heightSet.find(p.second));

        currentHeight = *heightSet.rbegin();

        if (lastHeight != currentHeight) {
            vector<int> silhouette;
            silhouette.push_back(p.first);
            silhouette.push_back(currentHeight);
            result->push_back(silhouette);

            lastHeight = currentHeight;
        }
    }
    return result;
    delete result;
}

// pushPointIfRequired was made to simplify mergeSkylines and worked when the algorithm was tested on Windows with Visual Studio,
// but for some reasons it doesn't output the right results when being tested via tp.sh on Linux. 
// Therefore it was not included in final source code.
/*
void pushPointIfRequired(int currentPos, int currentHeight, int otherHalfHeight, int& newHeight, vector<vector<int>>* result) {
    vector<int> lastInsert;

    if (currentHeight != newHeight) {
        newHeight = max(currentHeight, otherHalfHeight);
        if (result->size() > 0) {
            lastInsert = *(result->rbegin());
            if (lastInsert[1] != newHeight) // lastInsert[1] = Height of last point inserted
                result->push_back({ currentPos, newHeight });
        }
        else {
            result->push_back({ currentPos, newHeight });
        }
    }
} 
*/

vector<vector<int>>* mergeSkylines(vector<vector<int>>& firstHalf, vector<vector<int>>& secondHalf) {
    // Index 0 of iterator is position (x)
    // Index 1 of iterator is height   (h)
    vector<vector<int>>::iterator it1 = firstHalf.begin();
    vector<vector<int>>::iterator it2 = secondHalf.begin();
    vector<vector<int>>* result = new vector<vector<int>>();
    vector<int> lastInsert;

    int h1 = 0, h2 = 0, newHeight = 0;

    // See comment above on why there is duplicated logic here
    while (it1 != firstHalf.end() && it2 != secondHalf.end()) {
        if ((*it1)[0] <= (*it2)[0]) {
            h1 = (*it1)[1];
            if (h1 != newHeight) {
                newHeight = max(h1, h2);
                if (result->size() > 0) {
                    lastInsert = *(result->rbegin());
                    if (lastInsert[1] != newHeight)
                        result->push_back({ (*it1)[0], newHeight });
                }
                else {
                    result->push_back({ (*it1)[0], newHeight });
                }
            }
            it1++;
        }
        else {
            h2 = (*it2)[1];
            if (h2 != newHeight) {
                newHeight = max(h1, h2);
                if (result->size() > 0) {
                    lastInsert = *(result->rbegin());
                    if (lastInsert[1] != newHeight)
                        result->push_back({ (*it2)[0], newHeight });
                }
                else {
                    result->push_back({ (*it2)[0], newHeight });
                }
            }
            it2++;
        }
    }

    // Because of how we split, secondHalf can be same size or bigger than firstHalf
    while (it2 != secondHalf.end()) {
        result->push_back(*it2);
        it2++;
    }

    return result;
    delete result;
}

vector<vector<int>>* recursif(vector<vector<int>>& buildings) {
    if (buildings.size() <= 2)
        return brute(buildings);
    else {
        vector<vector<int>>* firstSkyline = new vector<vector<int>>();
        vector<vector<int>>* secondSkyline = new vector<vector<int>>();
        vector<vector<int>>* result = new vector<vector<int>>();
        size_t const half = buildings.size() / 2;
        vector<vector<int>> firstHalf = vector<vector<int>>(buildings.begin(), buildings.begin() + half);
        vector<vector<int>> secondHalf = vector<vector<int>>(buildings.begin() + half, buildings.end());

        firstSkyline = recursif(firstHalf);
        secondSkyline = recursif(secondHalf);

        result = mergeSkylines(*firstSkyline, *secondSkyline);

        return result;
        delete firstSkyline;
        delete secondSkyline;
        delete result;
    }
}

vector<vector<int>>* seuil(vector<vector<int>>& buildings) {
    const int TRESHOLD = 64;

    if (buildings.size() <= TRESHOLD)
        return brute(buildings);
    else {
        vector<vector<int>>* firstSkyline = new vector<vector<int>>();
        vector<vector<int>>* secondSkyline = new vector<vector<int>>();
        vector<vector<int>>* result = new vector<vector<int>>();
        size_t const half = buildings.size() / 2;
        vector<vector<int>> firstHalf = vector<vector<int>>(buildings.begin(), buildings.begin() + half);
        vector<vector<int>> secondHalf = vector<vector<int>>(buildings.begin() + half, buildings.end());

        firstSkyline = recursif(firstHalf);
        secondSkyline = recursif(secondHalf);

        result = mergeSkylines(*firstSkyline, *secondSkyline);

        return result;
        delete firstSkyline;
        delete secondSkyline;
        delete result;
    }
}

// Mainly from guide bash
int main(int argc, char *argv[]) {
    struct {
        string algo;
        string file_path;
        bool print_res{false};
        bool print_time{false};
    } prog_args;

    // Read program arguments
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg == "-a") {
            prog_args.algo = argv[i+1]; i++;
        } else if (arg == "-e") {
            prog_args.file_path = argv[i+1]; i++;
        } else if (arg == "-p") {
            prog_args.print_res = true;
        } else if (arg == "-t") {
            prog_args.print_time = true;
        }
    }

    // Read buildings coordinates into vector
    vector<vector<int>> buildings;
    {
        fstream ex_file(prog_args.file_path);
        int i = 1, j = 0, n;

        // Don't consider first number (number of buildings)
        ex_file >> n;
        buildings.push_back(vector<int>());
        
        while (ex_file >> n) {
            buildings[j].push_back(n);
            i++;
            if (i == 4) { // 3 elements max (x1,x2,h)
                i = 1;
                buildings.push_back(vector<int>());
                j++;
            }
        }
        // Remove excedent space given at the end
        buildings.pop_back();
    }

    // Apply correct algorithm
    if (prog_args.algo == "brute")
        run(brute, buildings, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "recursif")
        run(recursif, buildings, prog_args.print_res, prog_args.print_time);
    else if(prog_args.algo == "seuil")
        run(seuil, buildings, prog_args.print_res, prog_args.print_time);

}
