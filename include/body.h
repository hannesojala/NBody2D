#ifndef BODY_H
#define BODY_H

#include "SDL2/SDL.h"
#include <cmath>
#include <cstdlib>

class body {

friend class Sim;

public:
    body(float fmass, float px, float py, float vx, float vy);
    void update();
    void render(SDL_Renderer* renderer);

private:
    float radius() { return 1 + cbrt(abs(mass)/100); }
    float mass;
    float x;
    float y;
    float xv;
    float yv;

    int surface_seed;

    // Temporary
    // Midpoint algorithm
    // From https://stackoverflow.com/a/48291620
    void draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius);

    // Broken
    void draw_circle_displaced(SDL_Renderer* renderer, int centerX, int centerY, int radius);
};

#endif // BODY_H
