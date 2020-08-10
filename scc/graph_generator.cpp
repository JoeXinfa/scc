#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <ctime>
#include <omp.h>
#include "graph_writer.hpp"

using namespace std;

// create a erdos-renyi graph with n_vertices and probability for an edge = p
vector<vector<int>> ER_graph(double p, int n_vertices) {
    vector<vector<int>> edges(n_vertices);
    double r;
#pragma omp parallel
#pragma omp for
    for (unsigned i = 0; i < n_vertices; ++i) {
        for (unsigned j = 0; j < n_vertices; ++j) {
            r = double(rand()) / RAND_MAX;
            if (r >= p && i != j) edges[i].push_back(j);
        }
    }
    return edges;
}

int main(int argc, char const* argv[]) {
    // picking parameters
    cout << "Hello, this module generates erdos-renyi graphs!\nPlease specify "
        "(n_vertices, prob_of_an_edge)\n";
    int n_vertices;
    double p;
    cin >> n_vertices >> p;
    cin.ignore();  // takes the trailing \n

    srand(time(0));  // really random
    vector<vector<int>> edges = ER_graph(p, n_vertices);

    // user chooses where to output the graph, true says to print the message
    GraphWriter::output_graph(edges, n_vertices, true);

    return 0;
}
