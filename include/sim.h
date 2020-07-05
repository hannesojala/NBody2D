#ifndef SIM_H
#define SIM_H

#include <SDL.h>
#include <list>
#include <cmath>
#include <iostream>
#include "body.h"

class Sim
{

public:
    Sim();
    // Forbid Copy and Assignment
    Sim(const Sim&) = delete;
    operator=(const Sim&) = delete;

    virtual ~Sim();

    void events();
    void update();
    void render();

    const int sim_Width = 512;
    const int sim_Height = 512;

    bool running;

private:

    const float PI = 3.1415;
    const double G_CONST = 0.00025;


    SDL_Window* sim_Window;
    SDL_Renderer* sim_Renderer;
    SDL_Event event;

    std::list<body*> system;
    std::list<body*> trash;

    void init_SDL();
    void init_sim();

    void interact(body* A, body* B);
    void collide(body* A, body* B);
    void empty_trash();

    void make_ring(body* center, int radius, int amt, int mass, std::list<body*>& system);
};

#endif // SIM_H
