#include <vector>
#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

struct column {
    stack<int> blocks;
    int remainingValue;
};

vector<pair<int, int>> result;
int bestScore = 0;
int profit = 0;
int n, m;
const int NUM_REPLAYS = 200; // Arbitrary value to prevent force stop after 3 min
const int NUM_ITERATIONS = 20000; // Treshold after which we juge nothing better can be done

bool canDig(vector<column>& area, int xPos) {
    if (xPos < 0 || xPos > area.size() - 1 || area[xPos].blocks.empty())
        return false;
    int currentColumHeight = area[xPos].blocks.size();
    return
        (currentColumHeight == n) ||
        (xPos == 0 && area[xPos + 1].blocks.size() <= currentColumHeight) ||
        (xPos == area.size() - 1 && area[xPos - 1].blocks.size() <= currentColumHeight) ||
        (xPos > 0 && xPos < area.size() - 1 && area[xPos - 1].blocks.size() <= currentColumHeight && area[xPos + 1].blocks.size() <= currentColumHeight);
}

bool isProfitable(vector<column>& area, int i) {
    // A profit of 0 can free other blocks -> must be considered
    return
        (i == 0 && area[i].remainingValue + area[i + 1].remainingValue >= 0) ||
        (i == area.size() - 1 && area[i].remainingValue + area[i - 1].remainingValue >= 0) ||
        (i > 0 && i < area.size() - 1 && area[i - 1].remainingValue + area[i].remainingValue + area[i + 1].remainingValue >= 0);
}

// Looks at adjacent columns to see if it's worth digging them in order to reach the center column's deep block
int findProfitableColumn(vector<column>& area) {
    srand(time(NULL));
    for (int i = 0; i < area.size(); i++) {
        int index = rand() % area.size();
        if (area[index].blocks.empty())
            continue;

        if (isProfitable(area, index))
            return index;
    }
    return -1;
}

// Performs a block extraction on top of a column
void digColumn(vector<column>& area, int pos) {
    area[pos].remainingValue -= area[pos].blocks.top();
    profit += area[pos].blocks.top();
    result.push_back(make_pair(n - area[pos].blocks.size(), pos));
    area[pos].blocks.pop();
}

void digPositiveBlocksOnTop(vector<column>& area) {
    for (int i = 0; i < area.size(); i++) {
        if (area[i].blocks.top() >= 0 && canDig(area, i))
            digColumn(area, i);
    }
}

void extract(vector<column> area) {
    int attempts = 0;
    while (attempts < NUM_ITERATIONS) { 
        digPositiveBlocksOnTop(area);

        int col = findProfitableColumn(area);
        if (col == -1 || !(canDig(area, col) || canDig(area, col - 1) || canDig(area, col + 1))) {
            attempts++;
            continue;
        }
        while ((canDig(area, col - 1) || canDig(area, col) || canDig(area, col + 1)) && isProfitable(area, col)) {
            if (canDig(area, col - 1))
                digColumn(area, col - 1);
            if (canDig(area, col))
                digColumn(area, col);
            if (canDig(area, col + 1))
                digColumn(area, col + 1);
        }
    }
}

int main(int argc, char* argv[]) {

    // Read program argument
    string file_path;
    string arg(argv[1]);
    if (arg == "-e" && argc == 3)
        file_path = argv[2];
    else
        cout << "[ERREUR] Rappel: ./tp.sh -e CHEMIN_EXEMPLAIRE" << endl;

    fstream ex_file(file_path);

    ex_file >> n;
    ex_file >> m;

    vector<vector<int>> site; //     Only used to fill
    vector<column> area;      //  <- this variable

    int data;
    // Fill with gold values
    for (int i = 0; i < n; i++) {
        site.push_back(vector<int>());
        for (int j = 0; j < m; j++) {
            ex_file >> data;
            site[i].push_back(data);
        }
    }
    // Deduce costs
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ex_file >> data;
            site[i][j] -= data;
        }
    }
    // Create a vector of columns (stacks)
    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < m; j++) {
            if (i == n - 1) {
                stack<int> newStack;
                area.push_back({ newStack, 0 });
            }
            area[j].blocks.push(site[i][j]);
            area[j].remainingValue += site[i][j];
        }
    }
    
    int i = 0;
    while (i < NUM_REPLAYS) {
        profit = 0;
        extract(area);
        if (profit > bestScore) {
            bestScore = profit;
            cout << endl;
            for (auto p : result)
                cout << p.first << " " << p.second << endl;
        }
        result.clear();
        i++;
    }

    return 0;
}
