//
// Created by Leon Suchy on 21.10.18.
//

#include <time.h>   // high performance timer
#include <errno.h>  // error handling
#include <iostream>
#include <cstddef>  // size_t
#include <graph.hpp>
#include <cstring>  // strerror


constexpr int   GETTIME_FAILED          = -1;
constexpr int   NANOSECONDS_PER_SECOND  = 1000000000;
constexpr int   NUM_TEST_VERTICES       = 7000;
constexpr float BUCKET_LOAD_FACTOR      = 1.2f;

// Get the current CPU time for this process
inline timespec get_cpu_time()
{
    timespec ts {};
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == GETTIME_FAILED) {
        throw std::runtime_error(strerror(errno)); // exceptional error, should never happen
    }
    return ts;
}

// Calculate the difference between two timepoints in seconds
inline double delta_time_in_seconds(const timespec& a, const timespec& b)
{
    // calculate delta timestamp
    timespec result {};
    result.tv_sec = std::abs(a.tv_sec - b.tv_sec);
    result.tv_nsec = std::abs(a.tv_nsec - b.tv_nsec);

    // convert to real number
    return result.tv_sec + result.tv_nsec / (double) NANOSECONDS_PER_SECOND;
}


void test_id_graph()
{
    graph g { static_cast<size_t>(NUM_TEST_VERTICES * BUCKET_LOAD_FACTOR) };

    // NODE INSERT
    {
        auto begin = get_cpu_time();
        for (auto i = 0; i < NUM_TEST_VERTICES; i++)
        {
            g.add_vertex();
        }

        auto end = get_cpu_time();
        std::cout << "Vertex insert done. Took " << delta_time_in_seconds(begin, end) << "s" << std::endl;
    }

    // EDGE INSERT
    {
        auto begin = get_cpu_time();
        for (auto i = 0; i < NUM_TEST_VERTICES; i++)
        {
            for (auto j = 0; j < NUM_TEST_VERTICES; j++)
            {
                if (i == j)
                    continue;
                g.add_edge(*g.get_vertex(static_cast<vertex::id_type>(i)),
                           *g.get_vertex(static_cast<vertex::id_type>(j)));
            }
        }

        auto end = get_cpu_time();
        std::cout << "Edge insert done. Took " << delta_time_in_seconds(begin, end) << "s" << std::endl;
    }

    // EDGE REMOVAL
    {
        auto begin = get_cpu_time();
        for (auto i = 0; i < NUM_TEST_VERTICES; i++)
        {
            for (auto j = 0; j < NUM_TEST_VERTICES; j++)
            {
                if (i == j)
                    continue;
                g.remove_edge(*g.get_vertex(static_cast<vertex::id_type>(i)),
                              *g.get_vertex(static_cast<vertex::id_type>(j)));
            }
        }
        auto end = get_cpu_time();
        std::cout << "Edge removal done. Took " << delta_time_in_seconds(begin, end) << "s" << std::endl;
    }

    // VERTEX REMOVAL
    {
        auto begin = get_cpu_time();
        for (auto i = 0; i < NUM_TEST_VERTICES; i++)
        {
            g.invalidate(*g.get_vertex(static_cast<vertex::id_type>(i)));
        }

        auto end = get_cpu_time();
        std::cout << "Vertex removal done. Took " << delta_time_in_seconds(begin, end) << "s" << std::endl;
    }
}

int main()
{
    std::cout << "--------- ID GRAPH ---------" << std::endl;
    test_id_graph();
    std::cout << "----------------------------" << std::endl;

    std::cout << "-------- HASH GRAPH --------" << std::endl;
    test_hash_graph();
    std::cout << "----------------------------" << std::endl;

    return 0;
}
