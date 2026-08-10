#pragma once

#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

typedef struct host_display_t {
  
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Surface *surface;
  SDL_Palette *palette;
  SDL_Renderer *renderer;
  SDL_Color *palette_colors;

  SDL_PixelFormat pixel_format;
  SDL_PixelFormat texture_format;
  SDL_TextureAccess access;
  SDL_ScaleMode filter_mode;

  int colors_amount;

  int internal_width;
  int internal_height;

  int window_width;
  int window_height;

} host_display_t;

bool init_host_display(host_display_t *disp, const char *title, int width, int height);
void update_host_display(host_display_t *disp, void *disp_buf);
void render_host_display(host_display_t *disp);
void quit_host_display(host_display_t *disp);
