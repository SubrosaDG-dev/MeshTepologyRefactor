#include "tqdm.hpp"

#include <chrono>
#include <omp.h>
#include <thread>

int main()
{
    using namespace std::chrono_literals;
    std::size_t N = 1000;
    Tqdm::ProgressBar bar(N);

#pragma omp parallel for default(none) schedule(dynamic) shared(bar, N) num_threads(4)
    for (std::size_t i = 0; i < N; i++)
    {
        std::this_thread::sleep_for(10ms);
#pragma omp critical
        bar.update();
    }

    return 0;
}