#include "graph_writer.hpp"

using namespace std;

const string GraphWriter::EXTENSION = ".rgraph";
// utils to write a .rgraph file in the format specified by "format_graph.txt"
void GraphWriter::write_graph(vector<vector<int>> edges, int n_vertices) {
    (*out_text) << n_vertices << endl;
    for (unsigned vert = 0; vert < n_vertices; ++vert) {
        for (vector<int>::iterator dest = edges[vert].begin();
            dest != edges[vert].end(); ++dest) {
            (*out_text) << *dest << " ";
        }
        (*out_text) << endl;
    }
    return;
}

static void ID2node(vector<string> dict, string dictfilename) {
    ofstream* fout = new ofstream(dictfilename);
    for (std::vector<string>::iterator i = dict.begin(); i != dict.end(); ++i) {
        (*fout) << (*i) << endl;
    }
    fout->close();
}

// if you want to output the graph to a file use this
GraphWriter::GraphWriter(string out_base_name) {
    ofstream* fout = new ofstream((out_base_name + EXTENSION).c_str());
    out_text = fout;
}

GraphWriter::GraphWriter(ostream& outs) { out_text = &outs; }

GraphWriter::~GraphWriter() {
    if (out_text != &cout) {
        ofstream* derfrncr = dynamic_cast<ofstream*>(out_text);
        derfrncr->close();
        delete derfrncr;
    }
}

void GraphWriter::output_graph(vector<vector<int>> edges, int n_vertices,
    bool should_print) {
    if (should_print)
        cout
        << "\nIf you want to output the graph in a file type the name you want "
        "(no extension).\nOtherwise press Enter\n";
    string out_base_name;
    getline(cin, out_base_name);  // attention! buffer must be clear
    GraphWriter* gen_g;
    if (out_base_name == "") {
        gen_g = new GraphWriter(cout);
    }
    else {
        gen_g = new GraphWriter(out_base_name);
    }
    gen_g->write_graph(edges, n_vertices);
    delete gen_g;
}
