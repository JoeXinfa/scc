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

void fleischer(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc);

int main(int argc, char* argv[]) {
    string filename = argv[1];
    int n_vertices;
    clock_t begin_time;

    vector<unordered_set<int>> graph_edges_out = read_graph(filename, n_vertices);

    begin_time = clock();
    vector<unordered_set<int>> graph_edges_in = reverse_edges(graph_edges_out);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    vector<vector<int>> scc, scc_seq;

    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    begin_time = clock();
    vertices = trim(vertices, graph_edges_out, graph_edges_in, scc);
    std::cout << "Trim time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim nv: " << n_vertices << endl;
    std::cout << "After trim nv: " << vertices.size() << endl;

    begin_time = clock();
    fleischer(vertices, graph_edges_out, graph_edges_in, scc);
    std::cout << "SCC time ms: " << float(clock() - begin_time) << endl;

    string outfn = filename + ".scc";
    write_scc(outfn, scc);
    return 0;
}

void fleischer(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

    vector<bool> scced(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        int pivot = vertices[i];
        if (!scced[pivot]) {
            unordered_set<int> succ = reach_fw(graph_edges_out, pivot);
            unordered_set<int> pred = reach_bw(graph_edges_in, pivot);
            vector<int> s1;
            for (int i = 0; i < vertices.size(); i++) {
                if (succ.count(vertices[i]) > 0 && pred.count(vertices[i]) > 0) {
                    s1.push_back(vertices[i]);
                }
            }
            bool found = false;
            for (auto k : s1) {
                if (scced[k]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                scc.push_back(s1);
                for (auto k : s1) {
                    scced[k] = true;
                }
            }
        }
    }
}
