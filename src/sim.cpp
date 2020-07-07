#include "sim.h"

Sim::Sim() : running(true)
{
    init_SDL();
    init_sim();
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

void Sim::init_SDL() {
    // Init and create window and renderer
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) std::cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << std::endl;

    sim_Window = SDL_CreateWindow( "NBody2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sim_Width, sim_Height, SDL_WINDOW_SHOWN );
    if (!sim_Window) std::cout << "SDL could not create window. SDL_Error: " << SDL_GetError() << std::endl;

    sim_Renderer = SDL_CreateRenderer(sim_Window, -1, SDL_RENDERER_ACCELERATED);
    if (!sim_Renderer) std::cout << "SDL could not create renderer. SDL_Error: " << SDL_GetError() << std::endl;
}

void Sim::init_sim() {
    // should be a script and user input
    body* sun = new body(333000.0, sim_Width/2, sim_Height/2, 0, 0);
    system.insert(system.begin(), sun);
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

void Sim::events() {
    static int mouse_down_x = 0, mouse_down_y = 0;
    static int mouse_up_x = 0, mouse_up_y = 0;
    // Event Polling loop
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
        // Window 'X'
        case SDL_QUIT:
            // Quit
            running = false;
            break;
        // Any mouse button
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                SDL_GetMouseState(&mouse_down_x, &mouse_down_y);
                break;
            }

            break;

        case SDL_MOUSEBUTTONUP:
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                // Create a small body at the old mouse cursor position
                // with velocity equal to 8 times the portion of the window traversed during mouse click
                SDL_GetMouseState(&mouse_up_x, &mouse_up_y);
                // todo: remove magic numbers
                body* bod = new body(100.0, mouse_down_x, mouse_down_y, 8 * (float)(mouse_up_x - mouse_down_x)/sim_Width, 8 * (float)(mouse_up_y - mouse_down_y)/sim_Height);
                system.insert(system.begin(), bod);
                mouse_down_x = 0;
                mouse_down_y = 0;
                mouse_up_x = 0;
                mouse_up_y = 0;
                break;
            }
            break;
        }
    }
}

void Sim::update() {
    // Calculate gravity and collisions
    for (auto itA = system.begin(); itA != system.end(); itA++) {
        for (auto itB = system.begin(); itB != system.end(); itB++) {
            if (*itA != *itB) interact(*itA, *itB);
        }
    }
    // Remove bodies that are destroyed
    empty_trash();
    // Bodies update themselves
    for (auto B : system) B->update();
}

void Sim::interact(body* A, body* B) {
    // Calculate component ratios
    float xc = B->x - A->x;
    float yc = B->y - A->y;
    // Calculate distance
    float dist = sqrt(xc*xc + yc*yc);
    // Check for collisions
    if (dist < A->radius() + B->radius())
        collide(A, B);
    // Calculate gravitational force
    else {
        // Newtonian gravity equation
        float force = G_CONST * (A->mass * B->mass) / (dist * dist);
        // Apply force component-wise
        A->xv += force * (xc/dist) / A->mass;
        A->yv += force * (yc/dist) / A->mass;
    }
}

void Sim::collide(body* A, body* B) {
    // More massive body absorbs the less massive
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
    // Empty the system of bodies in the trash list
    for (auto t = trash.begin(); t != trash.end(); t++) {
        for (auto b = system.begin(); b != system.end(); /* nothing */) {
            if (*t == *b)
                b = system.erase(b);
            else ++b;
        }
    }
    trash.clear();
}

void Sim::render() {
    // Clear last frame
    SDL_SetRenderDrawColor(sim_Renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(sim_Renderer);

    // Render bodies
    for (auto B : system)
        B->render(sim_Renderer);

    // Backbuffer->Screenbuffer
    SDL_RenderPresent(sim_Renderer);
}

void Sim::make_ring(body* center, int radius, int amt, int mass, std::list<body*>& system) {
    for (int i = 0; i < amt; i++) {
        float ipi = i * 2 * PI / amt;
        body* pbod = new body
        (
            mass,
            center->x + sin(ipi) * radius,
            center->y + cos(ipi) * radius,
            -cos(ipi) * sqrt(G_CONST * center->mass/radius),
            sin(ipi) * sqrt(G_CONST * center->mass/radius)
        );
        system.insert(system.begin(), pbod);
    }
}
