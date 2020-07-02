#include "sim.h"

Sim::Sim()
{
    init_SDL();
    init_sim();
    running = true;
}

void Sim::init_SDL() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << endl;

    sim_Window = SDL_CreateWindow( "NBody2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
    if (!sim_Window) cout << "SDL could not create window. SDL_Error: " << SDL_GetError() << endl;

    sim_Renderer = SDL_CreateRenderer(sim_Window, -1, SDL_RENDERER_ACCELERATED);
    if (!sim_Renderer) cout << "SDL could not create renderer. SDL_Error: " << SDL_GetError() << endl;
}

void Sim::init_sim() {
    body* sun = new body(333000.0, WIDTH/2, HEIGHT/2, 0, 0);
    system.insert(system.begin(), sun);
    // Circle of Bodies
    make_ring(sun, 130, 16, 1000, system);
    make_ring(sun, 120, 128, 1, system);

    make_ring(sun,  80, 32, 10, system);
    make_ring(sun,  75, 32, 10, system);
    make_ring(sun,  70, 32, 10, system);
    make_ring(sun,  65, 32, 10, system);
    make_ring(sun,  60, 32, 10, system);
    make_ring(sun,  55, 32, 10, system);
    make_ring(sun,  50, 32, 10, system);
    make_ring(sun,  45, 32, 10, system);
    make_ring(sun,  40, 32, 10, system);
    make_ring(sun,  35, 32, 10, system);
    make_ring(sun,  30, 32, 10, system);
    make_ring(sun,  25, 32, 10, system);
}

Sim::~Sim()
{
    for (auto P : system)
        delete P;
    for (auto P : trash)
        delete P;
    system.clear();

    SDL_DestroyRenderer(sim_Renderer);
    SDL_DestroyWindow(sim_Window);
    SDL_Quit();
}

void Sim::events() {
    int mx, my;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            SDL_GetMouseState(&mx, &my);
            body* bod = new body(100.0, mx, my, 0, 0);
            system.insert(system.begin(), bod);
            break;
        }
    }
}

void Sim::update() {
    // grav and collisions
    for (auto itA = system.begin(); itA != system.end(); itA++) {
        for (auto itB = system.begin(); itB != system.end(); itB++) {
            auto A = *itA;
            auto B = *itB;
            if (A != B) {
                interact(A, B);
            }
        }
    }
    empty_trash();
    update_com();
    for (auto B : system) {
        B->update();
    }
}

void Sim::interact(body* A, body* B) {
    float xc = B->x - A->x;
    float yc = B->y - A->y;
    float dist = sqrt(xc*xc + yc*yc);
    if (dist < A->radius() + B->radius())
        // Collisions
        collide(A, B);
    else {
        // Gravity
        float force = G_CONST * (A->mass * B->mass) / (dist * dist);
        A->xv += force * (xc/dist) / A->mass;
        A->yv += force * (yc/dist) / A->mass;
    }
}

void Sim::collide(body* A, body* B) {
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
}

void Sim::empty_trash() {
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
}

void Sim::update_com() {
    // not impl
}

void Sim::render() {
    //SDL_RenderClear();
    // Trails. Above for no trails
    SDL_SetRenderDrawBlendMode(sim_Renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect R = {0, 0, WIDTH, HEIGHT};
    SDL_SetRenderDrawColor(sim_Renderer, 0x00, 0x00, 0x00, 0x11); // leaves a "residue"
    SDL_RenderFillRect(sim_Renderer, &R);

    for (auto B : system) {
            B->render(sim_Renderer);
    }

    // Center of mass indicator
    // SDL_SetRenderDrawColor(pRENDERER, 0xff, 0xff, 0x00, 0xff);
    // draw_circle(pRENDERER, cmx, cmy, 2.0);

    // SDL_SetWindowTitle(pWINDOW, to_string(system.size()).c_str());

    SDL_RenderPresent(sim_Renderer);
}

void Sim::make_ring(body* center, int radius, int amt, int mass, list<body*>& system) {
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

void body::draw_circle(SDL_Renderer* renderer, int centreX, int centreY, int radius) {
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
