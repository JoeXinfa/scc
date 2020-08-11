# CMakeList.txt : CMake project for scc, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (scc "scc.cpp" "scc.h" "dfs.h")

#add_executable (scck "scc_kosaraju.hpp" "scc_kosaraju.cpp")

add_executable (sccf "scc_fleischer.cpp" "scc_com.hpp")

add_executable (sccp "scc_tasks.cpp" "scc_com.hpp")

#add_executable (scpivots "scc_pivots.cpp" "scc_com.hpp")

add_executable (test_trim1 "test_trim1.cpp" "scc_com.hpp")
add_executable (test_trim2 "test_trim2.cpp" "scc_com.hpp")
add_executable (test_wcc "test_wcc.cpp" "scc_com.hpp")
add_executable (test_enh "test_enh.cpp" "scc_com.hpp" "scc_kosaraju.hpp" "scc_kosaraju.cpp")

add_executable (fib "fibonacci.cpp")

add_executable (grapher "graph_generator.cpp" "graph_writer.hpp" "graph_writer.cpp")

add_executable (grw "dgraph2rgraph.cpp" "graph_reader.hpp" "graph_reader.cpp"
    "graph_writer.hpp" "graph_writer.cpp")

# TODO: Add tests and install targets if needed.
