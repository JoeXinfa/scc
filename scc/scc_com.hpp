#pragma once

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
    vector<unordered_set<int>> edgesOut(n_vertices);

    for (int i = 0; i < n_vertices; i++) {
        getline(graph_file, line);
        stringstream estream(line);
        while (estream >> dest)
            edgesOut[i].insert(dest);
    }

    graph_file.close();

    return edgesOut;
}

// Generate the edges entering each vertex from the vector with exiting edges
vector<unordered_set<int>> reverse_edges(vector<unordered_set<int>>& edgesOut) {
    int n_vertices = edgesOut.size();
    vector<unordered_set<int>> edgesIn(n_vertices);

    for (int i = 0; i < n_vertices; i++) {
        for (unordered_set<int>::iterator it = edgesOut[i].begin(); it != edgesOut[i].end(); it++)
            edgesIn[*it].insert(i);
    }

    return edgesIn;
}

unordered_set<int> reach_fw_seq(vector<unordered_set<int>>& edgesOut, int pivot) {
    unordered_set<int> succ;
    queue<int> toVisit;
    int node;

    succ.insert(pivot);
    toVisit.push(pivot);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
        for (unordered_set<int>::iterator it = edgesOut[node].begin(); it != edgesOut[node].end(); it++) {
            if (succ.count(*it) == 0) {
                succ.insert(*it);
                toVisit.push(*it);
            }
        }
    }

    return succ;
}

unordered_set<int> reach_fw_par(vector<unordered_set<int>>& edgesOut, int pivot) {
    unordered_set<int> succ;
    queue<int> toVisit;
    int node;

    succ.insert(pivot);
    toVisit.push(pivot);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS in parallel
#pragma omp parallel
        for (unordered_set<int>::iterator it = edgesOut[node].begin(); it != edgesOut[node].end(); it++) {
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

unordered_set<int> reach_bw_seq(vector<unordered_set<int>>& edgesIn, int pivot) {
    unordered_set<int> pred;
    queue<int> toVisit;
    int node;

    pred.insert(pivot);
    toVisit.push(pivot);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
        for (unordered_set<int>::iterator it = edgesIn[node].begin(); it != edgesIn[node].end(); it++) {
            if (pred.count(*it) == 0) {
                pred.insert(*it);
                toVisit.push(*it);
            }
        }
    }

    return pred;
}

unordered_set<int> reach_bw_par(vector<unordered_set<int>>& edgesIn, int pivot) {
    unordered_set<int> pred;
    queue<int> toVisit;
    int node;

    pred.insert(pivot);
    toVisit.push(pivot);

    while (!toVisit.empty()) {
        node = toVisit.front();
        toVisit.pop();

        // BFS
#pragma omp parallel
        for (unordered_set<int>::iterator it = edgesIn[node].begin(); it != edgesIn[node].end(); it++) {
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

vector<int> trim_seq(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    queue<int> forward_trim, backward_trim;
    unordered_set<int> toDelete;
    int vertex;

    // forward trim
    for (int i = 0; i < vertices.size(); i++) {
        vertex = vertices[i];
        if (edgesIn[vertex].empty()) {
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
        for (unordered_set<int>::iterator it = edgesOut[vertex].begin(); it != edgesOut[vertex].end(); it++) {
            edgesIn[*it].erase(vertex);
            if (edgesIn[*it].empty()) {
                forward_trim.push(*it);
                toDelete.insert(*it);
            }
        }
        edgesOut[vertex].clear();
    }

    cout << "trimfr size: " << toDelete.size() << endl;

    // backward trim
    for (int i = 0; i < vertices.size(); i++) {
        vertex = vertices[i];
        if (edgesOut[vertex].empty() && toDelete.count(vertex) == 0) {
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
        for (unordered_set<int>::iterator it = edgesIn[vertex].begin(); it != edgesIn[vertex].end(); it++) {
            edgesOut[*it].erase(vertex);
            if (edgesOut[*it].empty()) {
                backward_trim.push(*it);
                toDelete.insert(*it);
            }
        }
        edgesIn[vertex].clear();
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

vector<int> trim_par(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    queue<int> forward_trim, backward_trim;
    unordered_set<int> toDelete;
    int vertex;

    // forward trim
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];  // local private to thread
        if (edgesIn[v].empty()) {
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
        for (unordered_set<int>::iterator it = edgesOut[vertex].begin(); it != edgesOut[vertex].end(); it++) {
            edgesIn[*it].erase(vertex);
            if (edgesIn[*it].empty()) {
                    forward_trim.push(*it);
                    toDelete.insert(*it);
            }
        }
        edgesOut[vertex].clear();
    }

    cout << "trimfr size: " << toDelete.size() << endl;

    // backward trim
#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int v = vertices[i];
        if (edgesOut[v].empty() && toDelete.count(v) == 0) {
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
        for (unordered_set<int>::iterator it = edgesIn[vertex].begin(); it != edgesIn[vertex].end(); it++) {
            edgesOut[*it].erase(vertex);
            if (edgesOut[*it].empty()) {
                    backward_trim.push(*it);
                    toDelete.insert(*it);
            }
        }
        edgesIn[vertex].clear();
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

vector<int> trim2_seq(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    unordered_set<int> toDelete;
    int vertex_a, vertex_b;

    for (int i = 0; i < vertices.size(); i++) {
        vertex_a = vertices[i];
        if (edgesIn[vertex_a].size() == 1) {
            vertex_b = *edgesIn[vertex_a].begin();
            if (edgesIn[vertex_b].size() == 1 &&
                *edgesIn[vertex_b].begin() == vertex_a) {
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
        if (edgesOut[vertex_a].size() == 1) {
            vertex_b = *edgesOut[vertex_a].begin();
            if (edgesOut[vertex_b].size() == 1 &&
                *edgesOut[vertex_b].begin() == vertex_a) {
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

vector<int> trim2_par(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    unordered_set<int> toDelete;

#pragma omp parallel for
    for (int i = 0; i < vertices.size(); i++) {
        int vertex_a = vertices[i];
        if (edgesIn[vertex_a].size() == 1) {
            int vertex_b = *edgesIn[vertex_a].begin();
            if (edgesIn[vertex_b].size() == 1 &&
                *edgesIn[vertex_b].begin() == vertex_a) {
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
        if (edgesOut[vertex_a].size() == 1) {
            int vertex_b = *edgesOut[vertex_a].begin();
            if (edgesOut[vertex_b].size() == 1 &&
            *edgesOut[vertex_b].begin() == vertex_a) {
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

vector<vector<int>> wcc_node_seq(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn) {
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
        int pivot = vertices[i];

        unordered_set<int> succ, pred;
        succ = reach_fw_seq(edgesOut, pivot);
        pred = reach_bw_seq(edgesIn, pivot);

        int j, minc;
        for (unordered_set<int>::iterator it = succ.begin(); it != succ.end(); it++) {
            j = get_index(vertices, *it);
            if (j == -1)
                continue;
            // Adjust WCC color to minimum index in component
            if (color[j] == -1) {
                color[j] = color[i];
            }
            else {
                minc = std::min(color[i], color[j]);
                color[i] = color[j] = minc;
            }
            //cout << "test succ " << i << " " << j << endl;
            //for (int k = 0; k < nv; k++) {
            //    cout << color[k] << " ";
            //}
            //cout << endl;
        }

        for (unordered_set<int>::iterator it = pred.begin(); it != pred.end(); it++) {
            j = get_index(vertices, *it);
            if (j == -1)
                continue;
            // Adjust WCC color to minimum index in component
            if (color[j] == -1) {
                color[j] = color[i];
            }
            else {
                minc = std::min(color[i], color[j]);
                color[i] = color[j] = minc;
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
    //cout << "Input size " << vertices.size() << endl;
    //cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

vector<vector<int>> wcc_edge_seq(vector<int> vertices, vector<unordered_set<int>>& edgesOut) {
    vector<vector<int>> wcc;
    int nv = vertices.size();
    int oldSum = 0, newSum = 0;

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

    oldSum = accumulate(color.begin(), color.end(), 0);
    int node, j, minc;
    int improve = 1;
    while (improve > 0) {
        for (int i = 0; i < nv; i++) {
            node = vertices[i];
            for (unordered_set<int>::iterator it = edgesOut[node].begin(); it != edgesOut[node].end(); it++) {
                j = get_index(vertices, *it);
                if (j == -1)
                    continue;
                // Adjust WCC color to minimum of the two at this edge
                minc = std::min(color[i], color[j]);
                color[i] = color[j] = minc;
            }
        }
        newSum = accumulate(color.begin(), color.end(), 0);
        improve = oldSum - newSum;
        //cout << "iter " << old_sum << " " << new_sum << endl;
        oldSum = newSum;
        newSum = 0;
    }

    //for (int i = 0; i < nv; i++) {
    //    cout << i << " " << color[i] << endl;
    //}

    int c;
    vector<int> wcc_colors;
    for (int i = 0; i < nv; i++) {
        c = color[i];
        if (std::count(wcc_colors.begin(), wcc_colors.end(), c)) {
            j = get_index(wcc_colors, c);
            wcc[j].push_back(vertices[i]);
        }
        else {
            wcc_colors.push_back(c);
            wcc.push_back(vector<int>());
            wcc[wcc.size() - 1].push_back(vertices[i]);
        }
    }

    //print_cc(wcc);
    //std::cout << "Input size " << vertices.size() << endl;
    //std::cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

vector<vector<int>> wcc_edge_par(vector<int> vertices, vector<unordered_set<int>>& edgesOut) {
    vector<vector<int>> wcc;
    int nv = vertices.size();
    int oldSum = 0, newSum = 0;

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

#pragma omp parallel for reduction (+:oldSum)
    for (int i = 0; i < nv; i++) {
        oldSum += color[i];
    }

    int improve = 1;
    while (improve > 0) {
#pragma omp parallel for
        for (int i = 0; i < nv; i++) {
            int node = vertices[i];
            for (unordered_set<int>::iterator it = edgesOut[node].begin(); it != edgesOut[node].end(); it++) {
                int j = get_index(vertices, *it);
                if (j == -1)
                    continue;
                // Adjust WCC color to minimum of the two at this edge
                int minc = std::min(color[i], color[j]);
#pragma omp critical
                color[i] = color[j] = minc;
            }
        }
#pragma omp parallel for reduction (+:newSum)
        for (int i = 0; i < nv; i++) {
            newSum += color[i];
        }
        improve = oldSum - newSum;
        //cout << "iter " << old_sum << " " << new_sum << endl;
        oldSum = newSum;
        newSum = 0;
    }

    //for (int i = 0; i < nv; i++) {
    //    cout << i << " " << color[i] << endl;
    //}

    int c;
    vector<int> wcc_colors;
    for (int i = 0; i < nv; i++) {
        c = color[i];
        if (std::count(wcc_colors.begin(), wcc_colors.end(), c)) {
            int j = get_index(wcc_colors, c);
            wcc[j].push_back(vertices[i]);
        }
        else {
            wcc_colors.push_back(c);
            wcc.push_back(vector<int>());
            wcc[wcc.size() - 1].push_back(vertices[i]);
        }
    }

    //print_cc(wcc);
    //std::cout << "Input size " << vertices.size() << endl;
    //std::cout << "WCC size: " << wcc.size() << endl;
    return wcc;
}

