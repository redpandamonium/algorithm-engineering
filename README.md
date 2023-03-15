# Algorithm Engineering WiSe 18/19

This is a solver for the vertex cover problem ([wiki](https://en.wikipedia.org/wiki/Vertex_cover)).
It is also a test bench to quickly implement new data reductions and solver optimizations from the literature.
It also includes a benchmarking testbench. 

Credit: Carsten Gregor Michael Schubert, Fynn Constantin Lohren, Leon Suchy

## Build & Run

To build you need CMake 3.10 or higher. Don't forget to recursively init the project's submodules.
Use `cmake -B cmake-build-<debug/release> -S . -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake` to generate the cmake build files.
After that you can run the main solver with `cmake --build cmake-build-debug --target VertexCover` and run it from there, or use the `--run` option.

## Example

The solver needs two configuration files to run. The pre-config is the preprocessing config before the actual solver starts.
The regular config is for the main solver phase. Here are two examples from our test repository:

* `./cmake-build-debug/vertex_cover/VertexCover --config vertex_cover/res/config_fast.json --pre-config vertex_cover/res/pre_config_fast.json --no-solution-dump --file vertex_cover/test/res/ninja_star.input`
* `./cmake-build-debug/vertex_cover/VertexCover --config vertex_cover/res/config_fast.json --pre-config vertex_cover/res/pre_config_fast.json --no-solution-dump --file vertex_cover/test/res/adjnoun.graph.input`

## Release builds

To build the release use ``cmake -DCMAKE_BUILD_TYPE=Release [...]`` and replace 
``cmake-build-debug`` with ``cmake-build-release``.

## Papers
* Case Study
https://arxiv.org/abs/1411.2680
* Crown decomposition
https://www.sciencedirect.com/science/article/pii/S0304397518303104
http://csm.lau.edu.lb/fabukhzam/pubs/cr_paper.pdf
* IS Measure & Conquer
https://dl.acm.org/citation.cfm?id=1552286
* Techniques for FPT algorithms
https://www.semanticscholar.org/paper/Techniques-for-Practical-Fixed-Parameter-Algorithms-H%C3%BCffner-Niedermeier/72cd038006863c19ced75eb06188c3284ae3e123

## Useful links

* Tool to analyse graphs
http://fpt.akt.tu-berlin.de/graphana/version_3_0/index.html
* CLion profiler  
https://www.jetbrains.com/help/clion/2018.3/cpu-profiler.html
* GMock cheat sheet  
https://github.com/google/googletest/blob/master/googlemock/docs/CheatSheet.md
* GTest documentation
  - https://github.com/google/googletest/blob/master/googletest/docs/primer.md
  - https://github.com/google/googletest/blob/master/googletest/docs/advanced.md