#ifndef GRAPH_READER_HPP
#define GRAPH_READER_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class GraphReader {
    int n_vertices;
    vector<vector<int>> edges;
    vector<string> complexId;

  public:
    int n_edges();
    int get_n();
    vector<vector<int>> get_edges();
    vector<string>* get_nodes();
    GraphReader(string filename);
};

#endif  // GRAPH_READER_HPP
