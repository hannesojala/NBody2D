#ifndef SIM_H
#define SIM_H

#include <SDL.h>
#include <list>
#include <cmath>
#include <iostream>

// put these defines into variables
#define WIDTH 512
#define HEIGHT 512
#define PI 3.1415
#define FPS 60
#define G_CONST 0.00025

using namespace std;

class Sim;

class body { // fix... stuff
    friend class Sim;
public:
    body(float fmass, float px, float py, float vx, float vy) {
        mass = fmass;
        x = px;
        y = py;
        xv = vx;
        yv = vy;
    }
    void update() {
        // move
        x += xv;
        y += yv;
    }
    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
        if (mass >= 100) SDL_RenderDrawLine(renderer, x, y, x+xv*5, y+yv*5);
        SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff);
        draw_circle(renderer, x, y, (int) radius());
    }
    float radius() {
        return 1 + cbrt(mass/100);
    }
    float x;
    float y;
    float xv;
    float yv;
    float mass;
    void draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius);
};

class Sim
{
friend class body;
public:
    // ctor/dtor
    Sim();
    virtual ~Sim();

    // engine functions
    void events();
    void update();
    void render();

    bool running;

private:
    SDL_Window* sim_Window;
    SDL_Renderer* sim_Renderer;
    SDL_Event event;

    list<body*> system;
    list<body*> trash;

    void init_SDL();
    void init_sim();

    // update() helpers
    void interact(body* A, body* B);
    void collide(body* A, body* B);
    void empty_trash();
    void update_com();

    void make_ring(body* center, int radius, int amt, int mass, list<body*>& system);

    // Hopefully temporary, may use SDL_gfx lib later
    // Midpoint algorithm
    // From https://stackoverflow.com/a/48291620
};

#endif // SIM_H
