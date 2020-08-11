#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>
#include <stack>
#include <ctime>
#include <omp.h>
#include <math.h>
#include "scc_com.hpp"
#include "scc_kosaraju.hpp"

using namespace std;

/*
Progressively improve
1) Divide-and-Conquer, vanilla
2) Add trim1
3) Add trim2
4) Parallel fw/bw search
5) Parallel s2/s3/s4
6) Use WCC
*/


void scc_dc1(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
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

    //cout << "Four sets: " << s1.size() << " " << s2.size() \
    //    << " " << s3.size() << " " << s4.size() << endl;

    scc_dc1(s2, edgesOut, edgesIn, scc);
    scc_dc1(s3, edgesOut, edgesIn, scc);
    scc_dc1(s4, edgesOut, edgesIn, scc);
}

void scc_dc4(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

    int pivot = vertices[rand() % vertices.size()];

    unordered_set<int> succ, pred;
#pragma omp task default(shared)
    succ = reach_fw_seq(edgesOut, pivot);
#pragma omp task default(shared)
    pred = reach_bw_seq(edgesIn, pivot);
#pragma omp taskwait

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

    scc_dc4(s2, edgesOut, edgesIn, scc);
    scc_dc4(s3, edgesOut, edgesIn, scc);
    scc_dc4(s4, edgesOut, edgesIn, scc);
}

void scc_dc5(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc,
    int rec_height) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

    int pivot = vertices[rand() % vertices.size()];

    unordered_set<int> succ, pred;
#pragma omp task default(shared)
    succ = reach_fw_seq(edgesOut, pivot);
#pragma omp task default(shared)
    pred = reach_bw_seq(edgesIn, pivot);
#pragma omp taskwait

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

    if (rec_height > 0) {
        rec_height -= 1;
#pragma omp task default(shared)
        scc_dc5(s2, edgesOut, edgesIn, scc, rec_height);
#pragma omp task default(shared)
        scc_dc5(s3, edgesOut, edgesIn, scc, rec_height);
#pragma omp task default(shared)
        scc_dc5(s4, edgesOut, edgesIn, scc, rec_height);
#pragma omp taskwait
    }
    else {
        scc_dc5(s2, edgesOut, edgesIn, scc, rec_height);
        scc_dc5(s3, edgesOut, edgesIn, scc, rec_height);
        scc_dc5(s4, edgesOut, edgesIn, scc, rec_height);
    }
}

void scc_dc6(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

    int pivot = vertices[rand() % vertices.size()];

    unordered_set<int> succ, pred;
#pragma omp task default(shared)
    succ = reach_fw_seq(edgesOut, pivot);
#pragma omp task default(shared)
    pred = reach_bw_seq(edgesIn, pivot);
#pragma omp taskwait

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

    // More tasks for parallel by WCC
    vector<vector<int>> wcc;
    wcc = wcc_edge_par(s2, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        scc_dc6(wcc[i], edgesOut, edgesIn, scc);
    }
    wcc = wcc_edge_par(s3, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        scc_dc6(wcc[i], edgesOut, edgesIn, scc);
    }
    wcc = wcc_edge_par(s4, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        scc_dc6(wcc[i], edgesOut, edgesIn, scc);
    }
}

void test_seq(string filename) {
    ifstream fin(filename);
    int n_vertices;
    fin >> n_vertices;
    fin.ignore();
    vector<vector<int>> neighbors = read_edges(n_vertices, fin);
    fin.close();

    clock_t begin_time = clock();
    list<list<int>> scc = kosaraju(neighbors);
    std::cout << "SCC0 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC0 size: " << scc.size() << endl;
}

void test_dc1(string filename) {
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    begin_time = clock();
    scc_dc1(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC1 size: " << scc.size() << endl;
}

void test_dc2(string filename) {
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim1 nv: " << n1 << endl;
    std::cout << "After trim1 nv: " << vertices.size() << endl;

    begin_time = clock();
    scc_dc1(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC2 size: " << scc.size() << endl;
}

void test_dc3(string filename) {
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim1 nv: " << n1 << endl;
    std::cout << "After trim1 nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    begin_time = clock();
    scc_dc1(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC3 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC3 size: " << scc.size() << endl;
}

void test_dc4(string filename) {
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim1 nv: " << n1 << endl;
    std::cout << "After trim1 nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    begin_time = clock();
    scc_dc4(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC4 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC4 size: " << scc.size() << endl;
}

void test_dc5(string filename) {
    int tot_thread = 8;
    int rec_depth = (int)log2f(tot_thread);
    if (((int)pow(2, rec_depth)) < tot_thread) rec_depth += 1;
    cout << "rec_depth: " << rec_depth << endl;
    omp_set_max_active_levels(rec_depth);
    omp_set_nested(rec_depth - 1);
    omp_set_num_threads(tot_thread);

    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim1 nv: " << n1 << endl;
    std::cout << "After trim1 nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    begin_time = clock();
    // is this necessary?
#pragma omp parallel
    {
#pragma omp single
        {
            scc_dc5(vertices, edgesOut, edgesIn, scc, rec_depth);
        }
    }
    std::cout << "SCC5 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC5 size: " << scc.size() << endl;
}

void test_dc6(string filename) {
    int n_vertices, n1;
    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    clock_t begin_time;
    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    vector<vector<int>> scc;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim1 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim1 nv: " << n1 << endl;
    std::cout << "After trim1 nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    begin_time = clock();
    scc_dc6(vertices, edgesOut, edgesIn, scc);
    std::cout << "SCC6 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "SCC6 size: " << scc.size() << endl;
}

int main(int argc, char* argv[]) {
    string filename = argv[1];
    test_dc6(filename);
    cout << "---------------------------------" << endl;
    test_dc5(filename);
    cout << "---------------------------------" << endl;
    test_dc4(filename);
    cout << "---------------------------------" << endl;
    test_dc3(filename);
    cout << "---------------------------------" << endl;
    test_dc2(filename);
    cout << "---------------------------------" << endl;
    // memory use goes high and may die, too many recursions?
    //test_dc1(filename);
    //cout << "---------------------------------" << endl;
    test_seq(filename);
    cout << "---------------------------------" << endl;
}

/*

xinfa@LAPTOP-71TBJKSA MINGW64 ~/Documents/uta/para/project/scc/build/scc (master)
$ ./test_enh.exe test_valid.rgraph
Flip time ms: 0
trimf1 size: 2
trimfr size: 3
trimb1 size: 3
trimbr size: 3
Trim1 time ms: 0
Before trim1 nv: 8
After trim1 nv: 5
trim2 size: 2
SCC size: 4
Trim2 time ms: 0
Before trim2 nv: 5
After trim2 nv: 3
SCC6 time ms: 1
SCC6 size: 5
---------------------------------
rec_depth: 3
Flip time ms: 0
trimf1 size: 2
trimfr size: 3
trimb1 size: 3
trimbr size: 3
Trim1 time ms: 0
Before trim1 nv: 8
After trim1 nv: 5
trim2 size: 2
SCC size: 4
Trim2 time ms: 0
Before trim2 nv: 5
After trim2 nv: 3
SCC5 time ms: 0
SCC5 size: 5
---------------------------------
Flip time ms: 0
trimf1 size: 2
trimfr size: 3
trimb1 size: 3
trimbr size: 3
Trim1 time ms: 0
Before trim1 nv: 8
After trim1 nv: 5
trim2 size: 2
SCC size: 4
Trim2 time ms: 0
Before trim2 nv: 5
After trim2 nv: 3
SCC4 time ms: 0
SCC4 size: 5
---------------------------------
Flip time ms: 0
trimf1 size: 2
trimfr size: 3
trimb1 size: 3
trimbr size: 3
Trim1 time ms: 0
Before trim1 nv: 8
After trim1 nv: 5
trim2 size: 2
SCC size: 4
Trim2 time ms: 0
Before trim2 nv: 5
After trim2 nv: 3
SCC3 time ms: 0
SCC3 size: 5
---------------------------------
Flip time ms: 0
trimf1 size: 2
trimfr size: 3
trimb1 size: 3
trimbr size: 3
Trim1 time ms: 0
Before trim1 nv: 8
After trim1 nv: 5
SCC2 time ms: 0
SCC2 size: 5
---------------------------------
SCC0 time ms: 0
SCC0 size: 5
---------------------------------

xinfa@LAPTOP-71TBJKSA MINGW64 ~/Documents/uta/para/project/scc/build/scc (master)
$ ./test_enh.exe wikivote.rgraph
Flip time ms: 39
trimf1 size: 4734
trimfr size: 4799
trimb1 size: 5748
trimbr size: 5815
Trim1 time ms: 28
Before trim1 nv: 7115
After trim1 nv: 1300
trim2 size: 0
SCC size: 5815
Trim2 time ms: 0
Before trim2 nv: 1300
After trim2 nv: 1300
SCC6 time ms: 14
SCC6 size: 5816
---------------------------------
rec_depth: 3
Flip time ms: 39
trimf1 size: 4734
trimfr size: 4799
trimb1 size: 5748
trimbr size: 5815
Trim1 time ms: 30
Before trim1 nv: 7115
After trim1 nv: 1300
trim2 size: 0
SCC size: 5815
Trim2 time ms: 0
Before trim2 nv: 1300
After trim2 nv: 1300
SCC5 time ms: 8
SCC5 size: 5816
---------------------------------
Flip time ms: 36
trimf1 size: 4734
trimfr size: 4799
trimb1 size: 5748
trimbr size: 5815
Trim1 time ms: 27
Before trim1 nv: 7115
After trim1 nv: 1300
trim2 size: 0
SCC size: 5815
Trim2 time ms: 0
Before trim2 nv: 1300
After trim2 nv: 1300
SCC4 time ms: 12
SCC4 size: 5816
---------------------------------
Flip time ms: 37
trimf1 size: 4734
trimfr size: 4799
trimb1 size: 5748
trimbr size: 5815
Trim1 time ms: 28
Before trim1 nv: 7115
After trim1 nv: 1300
trim2 size: 0
SCC size: 5815
Trim2 time ms: 0
Before trim2 nv: 1300
After trim2 nv: 1300
SCC3 time ms: 13
SCC3 size: 5816
---------------------------------
Flip time ms: 38
trimf1 size: 4734
trimfr size: 4799
trimb1 size: 5748
trimbr size: 5815
Trim1 time ms: 29
Before trim1 nv: 7115
After trim1 nv: 1300
SCC2 time ms: 13
SCC2 size: 5816
---------------------------------
SCC0 time ms: 16
SCC0 size: 5816
---------------------------------


$ ./test_enh.exe twitter.rgraph
Flip time ms: 788
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim1 time ms: 81
Before trim1 nv: 81306
After trim1 nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 20
Before trim2 nv: 69571
After trim2 nv: 69048
SCC6 time ms: 10042
SCC6 size: 12248
---------------------------------
rec_depth: 3
Flip time ms: 790
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim1 time ms: 82
Before trim1 nv: 81306
After trim1 nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 24
Before trim2 nv: 69571
After trim2 nv: 69048
SCC5 time ms: 86618
SCC5 size: 12248
---------------------------------
Flip time ms: 842
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim1 time ms: 84
Before trim1 nv: 81306
After trim1 nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 18
Before trim2 nv: 69571
After trim2 nv: 69048
SCC4 time ms: 86289
SCC4 size: 12248
---------------------------------
Flip time ms: 847
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim1 time ms: 85
Before trim1 nv: 81306
After trim1 nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 17
Before trim2 nv: 69571
After trim2 nv: 69048
SCC3 time ms: 89616
SCC3 size: 12248
---------------------------------
Flip time ms: 892
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim1 time ms: 84
Before trim1 nv: 81306
After trim1 nv: 69571
SCC2 time ms: 194704
SCC2 size: 12248
---------------------------------


SCC6 time ms: 10042  // DC + trim1 + trim2 + fwbw + wcc
SCC5 time ms: 86618  // DC + trim1 + trim2 + fwbw + s234
SCC4 time ms: 86289  // DC + trim1 + trim2 + fwbw
SCC3 time ms: 89616  // DC + trim1 + trim2
SCC2 time ms: 194704 // DC + trim1
SCC1 time ms: die    // DC
SCC0 time ms: die    // Kosaraju


*/
