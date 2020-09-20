#include "body.h"

body::body(float fmass, float px, float py, float vx, float vy) :
    mass(fmass),
    x(px),
    y(py),
    xv(vx),
    yv(vy),
    surface_seed(rand())
{}

void body::update() {
    x += xv;
    y += yv;
}

void body::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
    if (mass >= 100) SDL_RenderDrawLine(renderer, x, y, x+xv*5, y+yv*5);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff);
    draw_circle(renderer, x, y, (int) radius());
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

void body::draw_circle_displaced(SDL_Renderer* renderer, int centreX, int centreY, int radius) {
    // For now
    int res = 16;
    srand(surface_seed);
    int x = centreX;
    int y = centreY + radius;
    for (int i = 1; i <= res; i++) {
        float ipi = i * 2 * 3.1415 / res;
        int x2 = centreX + sin(ipi) * radius + (rand()%(1+radius/8));
        int y2 = centreY + cos(ipi) * radius + (rand()%(1+radius/8));
        SDL_RenderDrawLine(renderer, x, y, x2, y2);
        x = x2;
        y = y2;
    }
    SDL_RenderDrawLine(renderer, centreX, centreY+radius, x, y);
}
