#include <sim.h>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    int FPS = 60;
    unsigned int width = 512;
    unsigned int height = 512;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-fps") == 0) {
            if (i == argc-1) {
                std::cerr << "Error: bad value provided for option \"-fps\"\n";
                return -1;
            }
            FPS = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-size") == 0) {
            if (i == argc-2) {
                std::cerr << "Error: bad value provided for option \"-size\"\n";
                return -1;
            }
            width = atoi(argv[++i]);
            height = atoi(argv[++i]);
        }
    }

    Sim sim(width, height);
    while (sim.running) {
        auto f_begin = std::chrono::high_resolution_clock::now();

        sim.events();
        sim.update();
        sim.render();

        std::this_thread::sleep_until(f_begin + std::chrono::milliseconds(1000/FPS));
    }

    return 0;
}
