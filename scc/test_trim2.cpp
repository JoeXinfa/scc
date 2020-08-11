#include <string>
#include "scc_com.hpp"

using namespace std;

void test_trim(string filename, bool parallel) {
    int n_vertices;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);

    clock_t begin_time;
    vector<int> vertices;
    vector<vector<int>> scc;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    int n1 = vertices.size();
    begin_time = clock();
    if (!parallel) {
        vertices = trim2_seq(vertices, edgesOut, edgesIn, scc);
    }
    else {
        vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    }
    cout << "Use parallel? " << parallel << endl;
    cout << "Trim time ms: " << float(clock() - begin_time) << endl;
    cout << "Before trim nv: " << n1 << endl;
    cout << "After trim nv: " << vertices.size() << endl;
}

int main(int argc, char* argv[]) {
    string filename = argv[1];
    bool parallel;
    parallel = false;
    test_trim(filename, parallel);
    parallel = true;
    test_trim(filename, parallel);
}

/*
$ ./test_trim2.exe twitter.rgraph
trim2 size: 446
SCC size: 224
Use parallel? 0
Trim time ms: 10
Before trim nv: 81306
After trim nv: 80860
trim2 size: 446
SCC size: 224
Use parallel? 1
Trim time ms: 22
Before trim nv: 81306
After trim nv: 80860

*/
