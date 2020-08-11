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
        vertices = trim_seq(vertices, edgesOut, edgesIn, scc);
    }
    else {
        vertices = trim_par(vertices, edgesOut, edgesIn, scc);
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
Parallel only on the first iteration

$ ./test_trim1.exe citpatents.rgraph
trimf1 size: 515794
trimfr size: 3774763
trimb1 size: 3774782
trimbr size: 3774789
Use parallel? 0
Trim time ms: 18579
Before trim nv: 3774790
After trim nv: 1
trimf1 size: 515794
trimfr size: 3774763
trimb1 size: 3774782
trimbr size: 3774789
Use parallel? 1
Trim time ms: 17214
Before trim nv: 3774790
After trim nv: 1

*/
