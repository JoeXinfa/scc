#ifndef GRAPH_WRITER_HPP
#define GRAPH_WRITER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class GraphWriter {
public:
    ostream* out_text;
    static const string EXTENSION;
    GraphWriter(ostream& outs = cout);
    GraphWriter(string out_base_name);
    ~GraphWriter();
    void write_graph(vector<vector<int>> edges, int n_vertices);
    static void output_graph(vector<vector<int>>, int,
        bool should_print = false);
    static void ID2node(vector<string>, string);
};

#endif  // GRAPH_WRITER_HPP
