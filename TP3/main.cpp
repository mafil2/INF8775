#include <vector>
#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

struct block {
    int netGain;
    bool isDug;
};

struct column {
    stack<block> blocks;
    int remainingValue;
};

int profit = 0;
int n, m;

//bool canDig(vector<vector<block>>& site, int i, int j) {
//    if (site[i][j].isDug)
//        return false;
//
//    return (i == 0) || 
//           (j == 0 && site[i - 1][j].isDug && site[i - 1][j + 1].isDug) || 
//           (j == site[i].size() - 1 && site[i - 1][j].isDug && site[i - 1][j - 1].isDug) || 
//           (site[i - 1][j - 1].isDug && site[i - 1][j].isDug && site[i - 1][j + 1].isDug);
//}

bool canDig2(vector<column>& area, int xPos) {
    if (xPos < 0 || xPos > area.size() - 1)
        return false;

    return
        (area[xPos].blocks.size() == n) ||
        (xPos == 0 && area[xPos + 1].blocks.size() == area[xPos].blocks.size()) ||
        (xPos == area.size() - 1 && area[xPos - 1].blocks.size() == area[xPos].blocks.size()) ||
        (xPos > 0 && xPos < area.size() - 1 && area[xPos - 1].blocks.size() == area[xPos].blocks.size() && area[xPos + 1].blocks.size() == area[xPos].blocks.size());
}

bool isProfitable(vector<column>& area, int i) {
    return
        (i == 0 && area[i].remainingValue + area[i + 1].remainingValue >= 0) ||
        (i == area.size() - 1 && area[i].remainingValue + area[i - 1].remainingValue >= 0) ||
        (i > 0 && i < area.size() - 1 && area[i - 1].remainingValue + area[i].remainingValue + area[i + 1].remainingValue >= 0);
}

// Look at adjacent columns to see if its worth digging them in order to reach the center column's deep block
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

void digColumn(vector<column>& area, int pos) {
    area[pos].remainingValue -= area[pos].blocks.top().netGain;
    profit += area[pos].blocks.top().netGain;
    cout << area[pos].blocks.size() << " " << pos << endl;
    area[pos].blocks.pop();
}

bool isOnlyNegativeGainAvailable(vector<column>& area) {
    for (int i = 0; i < area.size(); i++) {
        if (!area[i].blocks.empty() && area[i].blocks.top().netGain >= 0 && canDig2(area, i))
            return false;
    }
    return true;
}

void extract(vector<column>& area) {
    int attempts = 0;
    while (true) {
        int col = findProfitableColumn(area);
        if (col != -1 && !canDig2(area, col)) {
            attempts++;
            if (attempts < 100) continue;
            return;
        }
        else if (col == -1) return;

        attempts = 0;
        for (int i = 0; i < area.size(); i++) {
            while (!area[i].blocks.empty() && area[i].blocks.top().netGain >= 0 && canDig2(area, i))
                digColumn(area, i);
        }

        if (isOnlyNegativeGainAvailable(area)) {
            int profitableColumn = col;
            if (profitableColumn == 0) {
                while ((!area[profitableColumn].blocks.empty() || !area[profitableColumn + 1].blocks.empty()) && isProfitable(area, profitableColumn)) {
                    if (canDig2(area, profitableColumn)) 
                        digColumn(area, profitableColumn);
                    if (canDig2(area, profitableColumn + 1))
                        digColumn(area, profitableColumn + 1);
                    if (!(canDig2(area, profitableColumn) || canDig2(area, profitableColumn + 1)))
                        break;
                    cout << "im while 1" << endl;
                }
            } else if (profitableColumn == area.size() - 1) {
                while ((!area[profitableColumn].blocks.empty() || !area[profitableColumn - 1].blocks.empty()) && isProfitable(area, profitableColumn)) {
                    if (canDig2(area, profitableColumn))
                        digColumn(area, profitableColumn);
                    if (canDig2(area, profitableColumn - 1))
                        digColumn(area, profitableColumn - 1);
                    if (!(canDig2(area, profitableColumn - 1) || canDig2(area, profitableColumn)))
                        break;
                    cout << "im while 2" << endl;
                }
            } else {
                while ((!area[profitableColumn - 1].blocks.empty() || !area[profitableColumn].blocks.empty() || !area[profitableColumn + 1].blocks.empty()) && isProfitable(area, profitableColumn)) {
                    if (canDig2(area, profitableColumn))
                        digColumn(area, profitableColumn);
                    if (canDig2(area, profitableColumn - 1))
                        digColumn(area, profitableColumn - 1);
                    if (canDig2(area, profitableColumn + 1))
                        digColumn(area, profitableColumn + 1);

                    if (!(canDig2(area, profitableColumn - 1) || canDig2(area, profitableColumn) || canDig2(area, profitableColumn + 1)))
                        break;
                    cout << "im while 3" << endl;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {

    // Read program argument
    string file_path;
    string arg(argv[1]);
    if (arg == "-e" && argc == 3) {
        file_path = argv[2];
    }
    else {
        cout << "[ERREUR] Rappel: ./tp.sh -e CHEMIN_EXEMPLAIRE" << endl;
    }

    fstream ex_file(file_path);

    ex_file >> n;
    ex_file >> m;

    vector<vector<block>> site;
    vector<column> area;

    int data;
    // Fill with gold values
    for (int i = 0; i < n; i++) {
        site.push_back(vector<block>());
        for (int j = 0; j < m; j++) {
            ex_file >> data;
            site[i].push_back({ data, false });
        }
    }
    // Deduce costs
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            ex_file >> data;
            site[i][j].netGain -= data;
            //remainingGains += data;
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < m; j++) {
            if (i == n - 1) {
                stack<block> newStack;
                area.push_back({newStack, 0});
            }
            area[j].blocks.push(site[i][j]);
            area[j].remainingValue += site[i][j].netGain;
        }
    }
    
    extract(area);
    cout << "Total profit : " << profit << endl;

    return 0;
}
