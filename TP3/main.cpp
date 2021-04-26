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

bool canDig(vector<column>& area, int xPos) {
    if (xPos < 0 || xPos > area.size() - 1)
        return false;

    return
        (area[xPos].blocks.size() == n) ||
        (xPos == 0 && area[xPos + 1].blocks.size() <= area[xPos].blocks.size()) ||
        (xPos == area.size() - 1 && area[xPos - 1].blocks.size() <= area[xPos].blocks.size()) ||
        (xPos > 0 && xPos < area.size() - 1 && area[xPos - 1].blocks.size() <= area[xPos].blocks.size() && area[xPos + 1].blocks.size() <= area[xPos].blocks.size());
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

// Checks if all blocks that can be digged return a negative profit
bool isOnlyNegativeGainAvailable(vector<column>& area) {
    for (int i = 0; i < area.size(); i++) {
        if (!area[i].blocks.empty() && area[i].blocks.top() >= 0 && canDig(area, i))
            return false;
    }
    return true;
}

void extract(vector<column> area) {
    int attempts = 0;
    while (true) {
        int col = findProfitableColumn(area);
        if (!canDig(area, col)) {
            attempts++;
            if (attempts < 100000) continue; // Treshold after which we juge nothing better can be done
            return;
        }

        attempts = 0;
        for (int i = 0; i < area.size(); i++) {
            while (!area[i].blocks.empty() && area[i].blocks.top() >= 0 && canDig(area, i))
                digColumn(area, i);
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
    
    const int numReplays = 200; // Arbitrary value to prevent force stop after 3 min
    int i = 0;
    while (i < numReplays) {
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
