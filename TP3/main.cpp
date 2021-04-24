#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

struct block {
    int netGain;
    bool isDug;
};

int extract() {

}

int main(int argc, char* argv[]) {

    // Read program argument
    string file_path(argv[2]);

    fstream ex_file(file_path);
    int n, m;

    ex_file >> n;
    ex_file >> m;

    vector<vector<block>> site;
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
        }
    }

    int result = extract();

    return 0;
}
