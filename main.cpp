#include <memory>
#include <iostream>
#include <list>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <string>
#include <SDL.h>

#define WIDTH 512
#define HEIGHT 512
#define PI 3.1415
#define FPS 144
#define G_CONST 0.0005

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

using namespace std;

SDL_Window* pWINDOW;
SDL_Renderer* pRENDERER;

void draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius);

class body {
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
        SDL_SetRenderDrawColor(pRENDERER, 0xff, 0x00, 0xff, 0xff);
        if (mass > 9) SDL_RenderDrawLine(pRENDERER, x, y, x+xv*5, y+yv*5);
        SDL_SetRenderDrawColor(pRENDERER, 0x00, 0xff, 0xff, 0xff);
        draw_circle(renderer, x, y, (int) radius());
    }
    float radius() { // logarithmic lookup table - cbrt is ideal but slow
        if (mass <= 1) // = earth
            return 1;
        else if (mass < 10 )
            return 1;   // 10 E
        else if (mass < 100)
            return 2;
        else if (mass < 1000)
            return 3;
        else if (mass < 10000)
            return 4;
        else if (mass < 100000)
            return 5;
        else if (mass < 1000000)
            return 6;
        else return log10(mass);
    }
    float x;
    float y;
    float xv;
    float yv;
    float mass;
};

void make_ring(body* center, int radius, int amt, int mass, list<body*>& system);

int main(int argv, char** args) {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << endl;
    pWINDOW = SDL_CreateWindow( "NBody2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
    if (!pWINDOW) cout << "SDL could not create window. SDL_Error: " << SDL_GetError() << endl;
    pRENDERER = SDL_CreateRenderer(pWINDOW, -1, SDL_RENDERER_ACCELERATED);
    if (!pRENDERER) cout << "SDL could not create renderer. SDL_Error: " << SDL_GetError() << endl;

    list<body*> system;
    list<body*> trash;

    body* sun = new body(1000000.0, WIDTH/2, HEIGHT/2, 0, 0);
    system.insert(system.begin(), sun);
    // Circle of Bodies
    make_ring(sun,  80, 128, 1, system);
    make_ring(sun,  75, 128, 1, system);
    make_ring(sun,  70, 128, 1, system);
    make_ring(sun,  65, 128, 1, system);
    make_ring(sun,  60, 128, 1, system);
    make_ring(sun,  55, 128, 1, system);
    make_ring(sun,  50, 128, 1, system);
    make_ring(sun,  45, 128, 1, system);
    make_ring(sun,  40,  64, 1, system);
    make_ring(sun,  35,  64, 1, system);
    make_ring(sun,  30,  64, 1, system);
    make_ring(sun,  25,  64, 1, system);

    // LOOP
    SDL_Event event;
    bool running = true;
    while (running) {
        auto f_begin = chrono::high_resolution_clock::now();
        int mx, my;

        // Handle Events
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                SDL_GetMouseState(&mx, &my);
                body* bod = new body(10.0, mx, my, 0, 0);
                system.insert(system.begin(), bod);
                break;
            }
        }

        // Update

        // grav and collisions
        for (auto itA = system.begin(); itA != system.end(); itA++) {
            for (auto itB = system.begin(); itB != system.end(); itB++) {
                auto A = *itA;
                auto B = *itB;
                if (A != B) {
                    float xc = B->x - A->x;
                    float yc = B->y - A->y;
                    float dist = sqrt(xc*xc + yc*yc);
                    if (dist < A->radius() + B->radius()) {
                        if (A->mass > B->mass) {
                            A->xv = (A->mass * A->xv + B->mass * B->xv)/(A->mass + B->mass);
                            A->yv = (A->mass * A->yv + B->mass * B->yv)/(A->mass + B->mass);
                            A->mass += B->mass;
                            trash.insert(trash.begin(), B);
                        } else {
                            B->xv = (A->mass * A->xv + B->mass * B->xv)/(A->mass + B->mass);
                            B->yv = (A->mass * A->yv + B->mass * B->yv)/(A->mass + B->mass);
                            B->mass += A->mass;
                            trash.insert(trash.begin(), A);
                        }
                    } else {
                        float force = G_CONST * (A->mass * B->mass) / (dist * dist);
                        A->xv += force * (xc/dist) / A->mass;
                        A->yv += force * (yc/dist) / A->mass;
                    }
                }
            }
        }

        // take out da trash
        for (auto t = trash.begin(); t != trash.end(); t++) {
            for (auto b = system.begin(); b != system.end(); /* nothing */) {
                if (*t == *b) {
                    b = system.erase(b);
                }
                else {
                    ++b;
                }
            }
        }
        trash.clear();

        // move the things
        for (auto B : system) {
            B->update();
        }

        // center of mass
        int cmx = 0;
        int cmy = 0;
        float total_mass = 0;
        for (auto b : system) {
            total_mass += b->mass;
            cmx += b->x * b->mass;
            cmy += b->y * b->mass;
        }
        cmx/=total_mass;
        cmy/=total_mass;



        // clear
        SDL_SetRenderDrawColor(pRENDERER, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderClear( pRENDERER );

        // Render
        // system
        for (auto B : system) {
            B->render(pRENDERER);
        }

        // Center of mass indicator
        SDL_SetRenderDrawColor(pRENDERER, 0xff, 0xff, 0x00, 0xff);
        draw_circle(pRENDERER, cmx, cmy, 2.0);

        SDL_SetWindowTitle(pWINDOW, to_string(system.size()).c_str());

        SDL_RenderPresent(pRENDERER);

        this_thread::sleep_until(f_begin + chrono::milliseconds(1000/FPS));

    }

    // EXIT STUFF
    for (auto P : system)
        delete P;
    system.clear();
    SDL_DestroyRenderer(pRENDERER);
    SDL_DestroyWindow(pWINDOW);
    SDL_Quit();
    return 0;
}

// Hopefully temporary, may use SDL_gfx lib later
// Midpoint algorithm
// From https://stackoverflow.com/a/48291620
void draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius) {
   const int diameter = (radius * 2);
   int x = (radius - 1);
   int y = 0;
   int tx = 1;
   int ty = 1;
   int error = (tx - diameter);
   while (x >= y) {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
      if (error <= 0) {
         ++y;
         error += ty;
         ty += 2;
      }
      if (error > 0) {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void make_ring(body* center, int radius, int amt, int mass, list<body*>& system) {
    for (int i = 0; i < amt; i++) {
        float ipi = i * 2 * PI / amt;
        body* pbod = new body(mass,
                              center->x + sin(ipi) * radius,
                              center->y + cos(ipi) * radius,
                              -cos(ipi) * sqrt(G_CONST * center->mass/radius),
                              sin(ipi) * sqrt(G_CONST * center->mass/radius));
        system.insert(system.begin(), pbod);
    }
}

