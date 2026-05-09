#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct host_display_t {
  
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Surface *surface;
  SDL_Palette *palette;
  SDL_Renderer *renderer;
  SDL_PixelFormat format;

} host_display_t;

bool init_host_display(host_display_t *disp, int width, int height);
void update_host_display(host_display_t *disp, void *disp_buf, SDL_PixelFormat pix_form);
void render_host_display(host_display_t *disp);
void quit_host_display(host_display_t *disp);
