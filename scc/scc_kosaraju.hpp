#pragma once

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

vector<vector<int>> read_edges(int n_vertices, istream& in);

list<list<int>> kosaraju(const vector<vector<int>>& neighbors);

