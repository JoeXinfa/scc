#include <string>
#include "scc_com.hpp"

using namespace std;

void test_wcc(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn) {
    vector<vector<int>> wcc;
    clock_t begin_time;

    begin_time = clock();
    wcc = wcc_node_seq(vertices, edgesOut, edgesIn);
    std::cout << "WCC reach seq time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Input size " << vertices.size() << endl;
    std::cout << "WCC size: " << wcc.size() << endl;

    begin_time = clock();
    wcc = wcc_edge_seq(vertices, edgesOut);
    std::cout << "WCC edge seq time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Input size " << vertices.size() << endl;
    std::cout << "WCC size: " << wcc.size() << endl;

    begin_time = clock();
    wcc = wcc_edge_par(vertices, edgesOut);
    std::cout << "WCC edge par time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Input size " << vertices.size() << endl;
    std::cout << "WCC size: " << wcc.size() << endl;
}


int main(int argc, char* argv[]) {
    string filename = argv[1];
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);
    
    //test_wcc(vertices, edgesOut, edgesIn);
    //return 0;

    vector<vector<int>> scc;

    clock_t begin_time;
    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_seq(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim nv: " << n1 << endl;
    std::cout << "After trim nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_seq(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    int pivot = vertices[rand() % vertices.size()];
    unordered_set<int> succ = reach_fw_seq(edgesOut, pivot);
    unordered_set<int> pred = reach_bw_seq(edgesIn, pivot);

    vector<int> s1, s2, s3, s4;
    for (int i = 0; i < vertices.size(); i++) {
        if (succ.count(vertices[i]) > 0) {
            if (pred.count(vertices[i]) > 0)
                s1.push_back(vertices[i]);
            else
                s2.push_back(vertices[i]);
        }
        else if (pred.count(vertices[i]) > 0)
            s3.push_back(vertices[i]);
        else
            s4.push_back(vertices[i]);
    }

    cout << "Four sets: " << s1.size() << " " << s2.size() \
        << " " << s3.size() << " " << s4.size() << " " << endl;

    test_wcc(s2, edgesOut, edgesIn);
}

/*
$ ./test_wcc.exe twitter.rgraph
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim time ms: 68
Before trim nv: 81306
After trim nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 9
Before trim2 nv: 69571
After trim2 nv: 69048
Four sets: 68413 635 0 0
WCC reach seq time ms: 126769
Input size 635
WCC size: 200
WCC edge seq time ms: 17
Input size 635
WCC size: 200
WCC edge par time ms: 6
Input size 635
WCC size: 200


$ ./test_wcc.exe wikivote.rgraph
Without any preprocessing...
WCC reach seq time ms: 173
Input size 7115
WCC size: 24
WCC edge seq time ms: 1573
Input size 7115
WCC size: 24
WCC edge par time ms: 778
Input size 7115
WCC size: 24

*/
