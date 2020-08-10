#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>
#include <stack>
#include <sstream>
#include <fstream>
#include <ctime>
#include <omp.h>
#include <algorithm>
#include <numeric>

using namespace std;

// Generate the edges for each vertex from the source file
vector<unordered_set<int>> read_graph(string filename, int& n_vertices) {
    ifstream graph_file;
    graph_file.open(filename);

    string line;
    int dest;

    graph_file >> n_vertices;
    getline(graph_file, line);
    vector<unordered_set<int>> graph_edges_out(n_vertices);

    for (int i = 0; i < n_vertices; i++) {
        getline(graph_file, line);
        stringstream estream(line);
        while (estream >> dest)
            graph_edges_out[i].insert(dest);
    }

    graph_file.close();

    return graph_edges_out;
}

// Generate the edges entering each vertex from the vector with exiting edges
vector<unordered_set<int>> reverse_edges(vector<unordered_set<int>>& graph_edges_out) {
    int n_vertices = graph_edges_out.size();
    vector<unordered_set<int>> graph_edges_in(n_vertices);

    for (int i = 0; i < n_vertices; i++) {
        for (unordered_set<int>::iterator it = graph_edges_out[i].begin(); it != graph_edges_out[i].end(); it++)
            graph_edges_in[*it].insert(i);
    }

    return graph_edges_in;
}

unordered_set<int> reach_fw_seq(vector<unordered_set<int>>& graph_edges_out, int start) {
    unordered_set<int> succ;
    queue<int> toVisit;
    int node;

    succ.insert(start);
    toVisit.push(start);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
        for (unordered_set<int>::iterator it = graph_edges_out[node].begin(); it != graph_edges_out[node].end(); it++) {
            if (succ.count(*it) == 0) {
                succ.insert(*it);
                toVisit.push(*it);
            }
        }
    }

    return succ;
}

unordered_set<int> reach_fw_par(vector<unordered_set<int>>& graph_edges_out, int start) {
    unordered_set<int> succ;
    queue<int> toVisit;
    int node;

    succ.insert(start);
    toVisit.push(start);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS in parallel
#pragma omp parallel
        for (unordered_set<int>::iterator it = graph_edges_out[node].begin(); it != graph_edges_out[node].end(); it++) {
            if (succ.count(*it) == 0) {
#pragma omp critical
                {
                    succ.insert(*it);
                    toVisit.push(*it);
                }
            }
        }
    }

    return succ;
}

unordered_set<int> reach_bw_seq(vector<unordered_set<int>>& graph_edges_in, int start) {
    unordered_set<int> pred;
    queue<int> toVisit;
    int node;

    pred.insert(start);
    toVisit.push(start);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
        for (unordered_set<int>::iterator it = graph_edges_in[node].begin(); it != graph_edges_in[node].end(); it++) {
            if (pred.count(*it) == 0) {
                pred.insert(*it);
                toVisit.push(*it);
            }
        }
    }

    return pred;
}

unordered_set<int> reach_bw_par(vector<unordered_set<int>>& graph_edges_in, int start) {
    unordered_set<int> pred;
    queue<int> toVisit;
    int node;

    pred.insert(start);
    toVisit.push(start);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
#pragma omp parallel
        for (unordered_set<int>::iterator it = graph_edges_in[node].begin(); it != graph_edges_in[node].end(); it++) {
            if (pred.count(*it) == 0) {
#pragma omp critical
                {
                    pred.insert(*it);
                    toVisit.push(*it);
                }
            }
        }
    }

    return pred;
}

vector<int> trim_seq(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc) {
    queue<int> forward_trim, backward_trim;
    unordered_set<int> toDelete;
    int vertex;

    // forward trim
    for (int i = 0; i < vertices.size(); i++) {
        vertex = vertices[i];
        if (graph_edges_in[vertex].empty()) {
            forward_trim.push(vertex);
            toDelete.insert(vertex);
        }
    }

    cout << "trimf1 size: " << toDelete.size() << endl;

    while (!forward_trim.empty()) {
        vertex = forward_trim.front();
        forward_trim.pop();

        // Add to SCC
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertex);

        // Delete out-edges
        for (unordered_set<int>::iterator it = graph_edges_out[vertex].begin(); it != graph_edges_out[vertex].end(); it++) {
            graph_edges_in[*it].erase(vertex);
            if (graph_edges_in[*it].empty()) {
                forward_trim.push(*it);
                toDelete.insert(*it);
            }
        }
        graph_edges_out[vertex].clear();
    }

    cout << "trimfr size: " << toDelete.size() << endl;

    // backward trim
    for (int i = 0; i < vertices.size(); i++) {
        vertex = vertices[i];
        if (graph_edges_out[vertex].empty() && toDelete.count(vertex) == 0) {
            backward_trim.push(vertex);
            toDelete.insert(vertex);
        }
    }

    cout << "trimb1 size: " << toDelete.size() << endl;

    while (!backward_trim.empty()) {
        vertex = backward_trim.front();
        backward_trim.pop();

        // Add to SCC
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertex);

        // Delete out-edges
        for (unordered_set<int>::iterator it = graph_edges_in[vertex].begin(); it != graph_edges_in[vertex].end(); it++) {
            graph_edges_out[*it].erase(vertex);
            if (graph_edges_out[*it].empty()) {
                backward_trim.push(*it);
                toDelete.insert(*it);
            }
        }
        graph_edges_in[vertex].clear();
    }
    
    cout << "trimbr size: " << toDelete.size() << endl;

    // update vectors
    vector<int> new_vertices;
    for (int i = 0; i < vertices.size(); i++) {
        vertex = vertices[i];
        if (toDelete.count(vertex) == 0)
            new_vertices.push_back(vertex);
    }

    return new_vertices;
}

vector<int> trim_par(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc) {
    queue<int> forward_trim, backward_trim;
    unordered_set<int> toDelete;
    int vertex;

    // forward trim
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];  // local private to thread
        if (graph_edges_in[v].empty()) {
#pragma omp critical
            {
                forward_trim.push(v);
                toDelete.insert(v);
            }
        }
    }

    cout << "trimf1 size: " << toDelete.size() << endl;

    while (!forward_trim.empty()) {
        vertex = forward_trim.front();
        forward_trim.pop();

        // Add to SCC
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertex);

        // Delete out-edges
        for (unordered_set<int>::iterator it = graph_edges_out[vertex].begin(); it != graph_edges_out[vertex].end(); it++) {
            graph_edges_in[*it].erase(vertex);
            if (graph_edges_in[*it].empty()) {
                    forward_trim.push(*it);
                    toDelete.insert(*it);
            }
        }
        graph_edges_out[vertex].clear();
    }

    cout << "trimfr size: " << toDelete.size() << endl;

    // backward trim
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];
        if (graph_edges_out[v].empty() && toDelete.count(v) == 0) {
#pragma omp critical
            {
                backward_trim.push(v);
                toDelete.insert(v);
            }
        }
    }

    cout << "trimb1 size: " << toDelete.size() << endl;

    while (!backward_trim.empty()) {
        vertex = backward_trim.front();
        backward_trim.pop();

        // Add to SCC
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertex);

        // Delete out-edges
        for (unordered_set<int>::iterator it = graph_edges_in[vertex].begin(); it != graph_edges_in[vertex].end(); it++) {
            graph_edges_out[*it].erase(vertex);
            if (graph_edges_out[*it].empty()) {
                    backward_trim.push(*it);
                    toDelete.insert(*it);
            }
        }
        graph_edges_in[vertex].clear();
    }
    cout << "trimbr size: " << toDelete.size() << endl;

    // update vectors
    vector<int> new_vertices;
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];
        if (toDelete.count(v) == 0)
#pragma omp critical
            new_vertices.push_back(v);
    }

    return new_vertices;
}

vector<int> trim2_seq(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc) {
    unordered_set<int> toDelete;
    int vertex_a, vertex_b;

    for (int i = 0; i < vertices.size(); i++) {
        vertex_a = vertices[i];
        if (graph_edges_in[vertex_a].size() == 1) {
            vertex_b = *graph_edges_in[vertex_a].begin();
            if (graph_edges_in[vertex_b].size() == 1 &&
                *graph_edges_in[vertex_b].begin() == vertex_a) {
                // Add to SCC
                if (toDelete.count(vertex_a) == 0) {
                    scc.push_back(vector<int>());
                    scc[scc.size() - 1].push_back(vertex_a);
                    scc[scc.size() - 1].push_back(vertex_b);
                }
                toDelete.insert(vertex_a);
                toDelete.insert(vertex_b);
                
            }
        }
        if (graph_edges_out[vertex_a].size() == 1) {
            vertex_b = *graph_edges_out[vertex_a].begin();
            if (graph_edges_out[vertex_b].size() == 1 &&
                *graph_edges_out[vertex_b].begin() == vertex_a) {
                // Add to SCC
                if (toDelete.count(vertex_a) == 0) {
                    scc.push_back(vector<int>());
                    scc[scc.size() - 1].push_back(vertex_a);
                    scc[scc.size() - 1].push_back(vertex_b);
                }
                toDelete.insert(vertex_a);
                toDelete.insert(vertex_b);
            }
        }
    }

    cout << "trim2 size: " << toDelete.size() << endl;
    cout << "SCC size: " << scc.size() << endl;

    // update vectors
    vector<int> new_vertices;
    for (int i = 0; i < vertices.size(); i++) {
        vertex_a = vertices[i];
        if (toDelete.count(vertex_a) == 0)
            new_vertices.push_back(vertex_a);
    }

    return new_vertices;
}

vector<int> trim2_par(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in, vector<vector<int>>& scc) {
    unordered_set<int> toDelete;

#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int vertex_a = vertices[i];
        if (graph_edges_in[vertex_a].size() == 1) {
            int vertex_b = *graph_edges_in[vertex_a].begin();
            if (graph_edges_in[vertex_b].size() == 1 &&
                *graph_edges_in[vertex_b].begin() == vertex_a) {
#pragma omp critical
                {
                    // Add to SCC
                    if (toDelete.count(vertex_a) == 0) {
                        scc.push_back(vector<int>());
                        scc[scc.size() - 1].push_back(vertex_a);
                        scc[scc.size() - 1].push_back(vertex_b);
                    }
                    toDelete.insert(vertex_a);
                    toDelete.insert(vertex_b);
                }
            }
        }
        if (graph_edges_out[vertex_a].size() == 1) {
            int vertex_b = *graph_edges_out[vertex_a].begin();
            if (graph_edges_out[vertex_b].size() == 1 &&
            *graph_edges_out[vertex_b].begin() == vertex_a) {
#pragma omp critical
                {
                    // Add to SCC
                    if (toDelete.count(vertex_a) == 0) {
                        scc.push_back(vector<int>());
                        scc[scc.size() - 1].push_back(vertex_a);
                        scc[scc.size() - 1].push_back(vertex_b);
                    }
                    toDelete.insert(vertex_a);
                    toDelete.insert(vertex_b);
                }
            }
        }
    }

    cout << "trim2 size: " << toDelete.size() << endl;
    cout << "SCC size: " << scc.size() << endl;

    // update vectors
    vector<int> new_vertices;
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];
        if (toDelete.count(v) == 0)
#pragma omp critical
            new_vertices.push_back(v);
    }

    return new_vertices;
}

void write_scc(string filename, vector<vector<int>>& scc) {
    ofstream fout(filename);
    int nv = 0;
    for (int i = 0; i < scc.size(); i++) {
        for (int j = 0; j < scc[i].size(); j++, nv++)
            fout << scc[i][j] << " ";
        fout << endl;
    }
    fout.close();
    std::cout << endl << "Vertices : " << nv << endl << "SCC : " << scc.size() << endl;
}


void print_cc(vector<vector<int>>& cc) {
    int nv = 0;
    for (int i = 0; i < cc.size(); i++) {
        for (int j = 0; j < cc[i].size(); j++, nv++)
            cout << cc[i][j] << " ";
        cout << endl;
    }
    cout << endl << "Vertices : " << nv << endl << "CC : " << cc.size() << endl;
}


// Function to print the index of an element in a vector
int get_index(vector<int>& v, int K) {
    auto it = find(v.begin(), v.end(), K);

    // If element was found 
    if (it != v.end()) {
        // calculating the index of K 
        int index = distance(v.begin(), it);
        //cout << index << endl;
        return index;
    }
    else {
        // If the element is not present in the vector 
        //cout << "-1" << endl;
        return -1;
    }
}

vector<vector<int>> wcc_reach_seq(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in) {
    vector<vector<int>> wcc;
    int nv = vertices.size();

    if (nv == 0)
        return wcc;
    if (nv == 1) {
        wcc.push_back(vector<int>());
        wcc[0].push_back(vertices[0]);
        return wcc;
    }

    vector<int> color(nv, -1);

    for (int i = 0; i < nv; i++) {
        //cout << "iter " << i << endl;
        if (color[i] != -1)
            continue;

        color[i] = i;
        int start = vertices[i];

        unordered_set<int> succ, pred;
        succ = reach_fw_seq(graph_edges_out, start);
        pred = reach_bw_seq(graph_edges_in, start);

        for (unordered_set<int>::iterator it = succ.begin(); it != succ.end(); it++) {
            int index = get_index(vertices, *it);
            if (index == -1)
                continue;
            // Adjust WCC color to minimum index in component
            if (color[index] == -1) {
                color[index] = color[i];
            }
            else if (color[index] <= color[i]) {
                color[i] = color[index];
            }
            else if (color[index] > color[i]) {
                color[index] = color[i];
                //cout << "should never be here" << endl;
            }
            //cout << "test succ " << i << " " << index << endl;
            //for (int k = 0; k < nv; k++) {
            //    cout << color[k] << " ";
            //}
            //cout << endl;
        }

        for (unordered_set<int>::iterator it = pred.begin(); it != pred.end(); it++) {
            int index = get_index(vertices, *it);
            if (index == -1)
                continue;
            // Adjust WCC color to minimum index in component
            if (color[index] == -1) {
                color[index] = color[i];
            }
            else if (color[index] <= color[i]) {
                color[i] = color[index];
            }
            else if (color[index] > color[i]) {
                color[index] = color[i];
                //cout << "should never be here" << endl;
            }
        }
    }

    //for (int i = 0; i < nv; i++) {
    //    cout << i << " " << color[i] << endl;
    //}

    int c;
    vector<int> wcc_colors;
    for (int i = 0; i < nv; i++) {
        c = color[i];
        if (std::count(wcc_colors.begin(), wcc_colors.end(), c)) {
            int index = get_index(wcc_colors, c);
            wcc[index].push_back(vertices[i]);
        }
        else {
            wcc_colors.push_back(c);
            wcc.push_back(vector<int>());
            wcc[wcc.size() - 1].push_back(vertices[i]);
        }
    }

    //print_cc(wcc);
    cout << "Input size " << vertices.size() << endl;
    cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

vector<vector<int>> wcc_edge_seq(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out) {
    vector<vector<int>> wcc;
    int nv = vertices.size();
    int old_sum, new_sum, improve;

    if (nv == 0)
        return wcc;
    if (nv == 1) {
        wcc.push_back(vector<int>());
        wcc[0].push_back(vertices[0]);
        return wcc;
    }

    vector<int> color(nv, -1);
    for (int i = 0; i < nv; i++) {
        color[i] = i;
    }

    do {
        old_sum = accumulate(color.begin(), color.end(), 0);
        for (int i = 0; i < nv; i++) {
            int node = vertices[i];
            for (unordered_set<int>::iterator it = graph_edges_out[node].begin(); it != graph_edges_out[node].end(); it++) {
                int j = get_index(vertices, *it);
                if (j == -1)
                    continue;
                // Adjust WCC color to minimum of the two at this edge
                int minc = std::min(color[i], color[j]);
                color[i] = color[j] = minc;
            }
        }
        new_sum = accumulate(color.begin(), color.end(), 0);
        improve = old_sum - new_sum;
        //cout << "iter " << old_sum << " " << new_sum << endl;
    } while (improve > 0);

    //for (int i = 0; i < nv; i++) {
    //    cout << i << " " << color[i] << endl;
    //}

    int c;
    vector<int> wcc_colors;
    for (int i = 0; i < nv; i++) {
        c = color[i];
        if (std::count(wcc_colors.begin(), wcc_colors.end(), c)) {
            int index = get_index(wcc_colors, c);
            wcc[index].push_back(vertices[i]);
        }
        else {
            wcc_colors.push_back(c);
            wcc.push_back(vector<int>());
            wcc[wcc.size() - 1].push_back(vertices[i]);
        }
    }

    //print_cc(wcc);
    std::cout << "Input size " << vertices.size() << endl;
    std::cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

vector<vector<int>> wcc_edge_par(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out) {
    vector<vector<int>> wcc;
    int nv = vertices.size();
    int old_sum, new_sum, improve;

    if (nv == 0)
        return wcc;
    if (nv == 1) {
        wcc.push_back(vector<int>());
        wcc[0].push_back(vertices[0]);
        return wcc;
    }

    vector<int> color(nv, -1);
    for (int i = 0; i < nv; i++) {
        color[i] = i;
    }

    do {
        old_sum = new_sum = 0;
        //old_sum = accumulate(color.begin(), color.end(), 0);
#pragma omp parallel for reduction (+:old_sum)
        for (int i = 0; i < nv; i++) {
            old_sum += color[i];
        }
#pragma omp parallel for
        for (int i = 0; i < nv; i++) {
            int node = vertices[i];
            for (unordered_set<int>::iterator it = graph_edges_out[node].begin(); it != graph_edges_out[node].end(); it++) {
                int j = get_index(vertices, *it);
                if (j == -1)
                    continue;
                // Adjust WCC color to minimum of the two at this edge
                int minc = std::min(color[i], color[j]);
#pragma omp critical
                color[i] = color[j] = minc;
            }
        }
        //new_sum = accumulate(color.begin(), color.end(), 0);
#pragma omp parallel for reduction (+:new_sum)
        for (int i = 0; i < nv; i++) {
            new_sum += color[i];
        }
        improve = old_sum - new_sum;
        //cout << "iter " << old_sum << " " << new_sum << endl;
    } while (improve > 0);

    //for (int i = 0; i < nv; i++) {
    //    cout << i << " " << color[i] << endl;
    //}

    int c;
    vector<int> wcc_colors;
    for (int i = 0; i < nv; i++) {
        c = color[i];
        if (std::count(wcc_colors.begin(), wcc_colors.end(), c)) {
            int index = get_index(wcc_colors, c);
            wcc[index].push_back(vertices[i]);
        }
        else {
            wcc_colors.push_back(c);
            wcc.push_back(vector<int>());
            wcc[wcc.size() - 1].push_back(vertices[i]);
        }
    }

    //print_cc(wcc);
    std::cout << "Input size " << vertices.size() << endl;
    std::cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

void test_wcc(vector<int> vertices, vector<unordered_set<int>>& graph_edges_out,
    vector<unordered_set<int>>& graph_edges_in) {
    vector<vector<int>> wcc;
    clock_t begin_time;

    begin_time = clock();
    wcc = wcc_reach_seq(vertices, graph_edges_out, graph_edges_in);
    std::cout << "WCC reach seq time ms: " << float(clock() - begin_time) << endl;

    begin_time = clock();
    wcc = wcc_edge_seq(vertices, graph_edges_out);
    std::cout << "WCC edge seq time ms: " << float(clock() - begin_time) << endl;

    begin_time = clock();
    wcc = wcc_edge_par(vertices, graph_edges_out);
    std::cout << "WCC edge par time ms: " << float(clock() - begin_time) << endl;
}
