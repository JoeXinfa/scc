#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>
#include <stack>
#include <sstream>
#include <fstream>
#include <ctime>
#include "scc_com.hpp"

using namespace std;

void fleischer(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc);

int main(int argc, char* argv[]) {
    string filename = argv[1];
    int n_vertices, n1;
    clock_t begin_time;

    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    vector<vector<int>> scc;

    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    /*
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
    */

    begin_time = clock();
    fleischer(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC time ms: " << float(clock() - begin_time) << endl;

    string outfn = filename + ".scc";
    write_scc(outfn, scc);
    return 0;
}

void fleischer(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

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

    scc.push_back(s1);
    //s4 = trim_seq(s4, edgesOut, edgesIn, scc);

    cout << "Thread ID: " << 0 << " " << s1.size() << " " << s2.size() \
        << " " << s3.size() << " " << s4.size() << endl;

    fleischer(s2, edgesOut, edgesIn, scc);
    fleischer(s3, edgesOut, edgesIn, scc);
    fleischer(s4, edgesOut, edgesIn, scc);

    // More tasks for parallel by WCC
    /*
    vector<vector<int>> wccs;
    wccs = wcc_edge_seq(s2, edgesOut);
    for (auto wcc : wccs) {
        fleischer(wcc, edgesOut, edgesIn, scc);
    }
    wccs = wcc_edge_seq(s3, edgesOut);
    for (auto wcc : wccs) {
        fleischer(wcc, edgesOut, edgesIn, scc);
    }
    wccs = wcc_edge_seq(s4, edgesOut);
    for (auto wcc : wccs) {
        fleischer(wcc, edgesOut, edgesIn, scc);
    }
    */
}
