# Algorithm Engineering WiSe 18/19

Carsten Gregor Michael Schubert, Fynn Constantin Lohren, Leon Suchy

## Code Keywords (no colons)
    - NOTE
    - TODO
    - REVIEW

## Build & Run

To build you need CMake 3.10 or higher. Use   
``cmake --build cmake-build-debug --target <target>``  
to build a target. You can find the build output in ``cmake-build-debug/<target>/``.

### Example

To build & run the vertex cover solver you first build the target and its
dependencies using ``cmake --build cmake-build-debug --target vertex_cover``.
You can then run it with ``./cmake-build-debug/vertex_cover``.  

### Release builds

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