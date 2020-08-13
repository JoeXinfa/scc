# ParSCC

Parallel detection of strongly connected components in real-world graphs

## Code

* scc/scc_com.hpp  all core functions
* scc/test_\*  driver code for testing
* scc/graph_\* utilities for graph IO


## Compile

* mkdir build; cd build
* cmake .. -G "Unix Makefiles"
* make

## Run

* cd build/scc
* ./test_trim1.exe exmaple.rgraph
* ./test_trim2.exe exmaple.rgraph
* ./test_wcc.exe exmaple.rgraph
* ./test_enh.exe exmaple.rgraph

## Presentation

https://docs.google.com/presentation/d/1rNQQ4N8m2uv5IPSoA3ySq9RbUrX_cRWREOkCLChwZVw/edit?usp=sharing
