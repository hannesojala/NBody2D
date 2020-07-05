#ifndef BODY_H
#define BODY_H

#include "SDL.h"
#include <cmath>

class body {

friend class Sim;

public:
    body(float fmass, float px, float py, float vx, float vy);
    void update();
    void render(SDL_Renderer* renderer);

private:
    float radius() { return 1 + cbrt(mass/100); } // examine. member variable instead? variable mass? on-fly impact? ctor initialization?

    float mass;
    float x;
    float y;
    float xv;
    float yv;

    // Hopefully temporary, may use SDL_gfx lib later
    // Midpoint algorithm
    // From https://stackoverflow.com/a/48291620
    void draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius);
};

#endif // BODY_H
