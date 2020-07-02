#include <sim.h>
#include <chrono>
#include <thread>

// TODO
/*

REEEEEEE-FACTOR

. Change vector of ptr to vector of smart ptr
. High energy collisions result in mergers, low energy are elastic
. Quadtree data struct
. Make camera follow COM

EXPENSIVE THINGS
. Collision Check - Use a faster way to check it
. Gravity Force Calulation - Use a faster way to calculate it
. Drawing - Unlikely to be fruitful.
. cbrt(); Yikes
. List may not be the best for anything but trashing old bodies

*/

int main(int argv, char** args) {
    Sim sim;
    while (sim.running) {
        auto f_begin = chrono::high_resolution_clock::now();

        sim.events();
        sim.update();
        sim.render();

        this_thread::sleep_until(f_begin + chrono::milliseconds(1000/FPS));
    }

    return 0;
}

