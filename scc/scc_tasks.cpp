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
#include <math.h>
#include "scc_com.hpp"

using namespace std;

void fleischer(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc,
    int rec_height);

int main(int argc, char* argv[]) {
    int tot_thread = 8;
    int recDepth = (int) log2f(tot_thread);
    if (((int)pow(2, recDepth)) < tot_thread) recDepth += 1;
    cout << "recDepth: " << recDepth << endl;
    omp_set_max_active_levels(recDepth);
    omp_set_nested(recDepth - 1);
    //omp_set_nested(1);
    //cout << omp_get_nested() << endl;
    omp_set_num_threads(tot_thread);

    string filename = argv[1];
    int n_vertices, n1;
    clock_t begin_time;

    vector<unordered_set<int>> edgesOut = read_graph(filename, n_vertices);

    begin_time = clock();
    vector<unordered_set<int>> edgesIn = reverse_edges(edgesOut);
    std::cout << "Flip time ms: " << float(clock() - begin_time) << endl;

    vector<int> vertices;
    vector<vector<int>> scc, scc_seq;

    for (int i = 0; i < n_vertices; i++)
        vertices.push_back(i);

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim nv: " << n1 << endl;
    std::cout << "After trim nv: " << vertices.size() << endl;

    n1 = vertices.size();
    begin_time = clock();
    vertices = trim2_par(vertices, edgesOut, edgesIn, scc);
    std::cout << "Trim2 time ms: " << float(clock() - begin_time) << endl;
    std::cout << "Before trim2 nv: " << n1 << endl;
    std::cout << "After trim2 nv: " << vertices.size() << endl;

    begin_time = clock();

#pragma omp parallel
    {
#pragma omp single
        {
            fleischer(vertices, edgesOut, edgesIn, scc, recDepth);
        }
    }

    std::cout << "SCC time ms: " << float(clock() - begin_time) << endl;

    string outfn = filename + ".scc";
    write_scc(outfn, scc);
    return 0;
}

void fleischer(vector<int> vertices, vector<unordered_set<int>>& edgesOut,
    vector<unordered_set<int>>& edgesIn, vector<vector<int>>& scc,
    int rec_height) {
    if (vertices.size() == 0)
        return;

    if (vertices.size() == 1) {
        scc.push_back(vector<int>());
        scc[scc.size() - 1].push_back(vertices[0]);
        return;
    }

    //int pivot = vertices[0];
    int pivot = vertices[rand() % vertices.size()];

    unordered_set<int> succ, pred;
#pragma omp task default(shared)
    succ = reach_fw_seq(edgesOut, pivot);
    //succ = reach_fw_par(edgesOut, pivot);  // slower than seq
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

//#pragma omp critical
    scc.push_back(s1);  // need single thread access?

    int ID = omp_get_thread_num();
    cout << "Thread ID: " << ID << " " << s1.size() << " " << s2.size() \
        << " " << s3.size() << " " << s4.size() << " " << rec_height << endl;

    //int s4old = s4.size();
    //s4 = trim(s4, edgesOut, edgesIn, scc);
    //int s4new = s4.size();
    //cout << "before/after trim: " << s4old << " " << s4new << endl;

    if (rec_height > 0) {
        rec_height -= 1;
#pragma omp task default(shared)
        fleischer(s2, edgesOut, edgesIn, scc, rec_height);
#pragma omp task default(shared)
        fleischer(s3, edgesOut, edgesIn, scc, rec_height);
#pragma omp task default(shared)
        fleischer(s4, edgesOut, edgesIn, scc, rec_height);
#pragma omp taskwait
    }
    else {
        fleischer(s2, edgesOut, edgesIn, scc, rec_height);
        fleischer(s3, edgesOut, edgesIn, scc, rec_height);
        fleischer(s4, edgesOut, edgesIn, scc, rec_height);
    }

    // More tasks for parallel by WCC
    /*
    vector<vector<int>> wcc;
    wcc = wcc_edge_par(s2, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        fleischer(wcc[i], edgesOut, edgesIn, scc, rec_height);
    }
    wcc = wcc_edge_par(s3, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        fleischer(wcc[i], edgesOut, edgesIn, scc, rec_height);
    }
    wcc = wcc_edge_par(s4, edgesOut);
#pragma omp parallel for
    for (int i = 0; i < wcc.size(); i++) {
        fleischer(wcc[i], edgesOut, edgesIn, scc, rec_height);
    }
    */

}


/*
Without WCC
$ ./sccp.exe twitter.rgraph
recDepth: 3
Flip time ms: 1586
trimf1 size: 1
trimfr size: 2
trimb1 size: 11210
trimbr size: 11735
Trim time ms: 140
Before trim nv: 81306
After trim nv: 69571
trim2 size: 523
SCC size: 11998
Trim2 time ms: 31
Before trim2 nv: 69571
After trim2 nv: 69048
Thread ID: 2 68413 635 0 0 3
Thread ID: 2 1 3 0 631 2
Thread ID: 4 3 0 0 0 1
Thread ID: 7 8 0 4 619 1
Thread ID: 7 14 0 21 584 0
Thread ID: 0 1 0 0 3 0
Thread ID: 0 1 0 0 2 0
Thread ID: 7 20 0 1 0 0
Thread ID: 0 1 0 0 1 0
Thread ID: 7 1 0 0 583 0
Thread ID: 7 1 5 0 577 0
Thread ID: 7 5 0 0 0 0
Thread ID: 7 1 3 0 573 0
Thread ID: 7 3 0 0 0 0
Thread ID: 7 53 0 2 518 0
Thread ID: 7 1 0 0 1 0
Thread ID: 7 1 0 0 517 0
Thread ID: 7 1 0 0 516 0
Thread ID: 7 3 0 0 513 0
Thread ID: 7 3 0 0 510 0
Thread ID: 7 1 0 0 509 0
Thread ID: 7 1 0 0 508 0
Thread ID: 7 1 0 0 507 0
Thread ID: 7 73 0 0 434 0
Thread ID: 7 1 0 1 432 0
Thread ID: 7 1 0 0 431 0
Thread ID: 7 3 0 0 428 0
Thread ID: 7 7 0 1 420 0
Thread ID: 7 3 0 0 417 0

*/