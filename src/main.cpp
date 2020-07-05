#include <sim.h>
#include <chrono>
#include <thread>

int main(int argc, char** argv) {

    const int FPS = 60;
    Sim sim;
    while (sim.running) {
        auto f_begin = std::chrono::high_resolution_clock::now();

        sim.events();
        sim.update();
        sim.render();

        std::this_thread::sleep_until(f_begin + std::chrono::milliseconds(1000/FPS));
    }

    return 0;
}

