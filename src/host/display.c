#include <stdio.h>
#include <stdbool.h>

#include "display.h"


bool host_create_window_renderer(host_display_t *disp, const char *title, int width, int height) {

  SDL_Rect disp_bounds;

  if(!SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &disp_bounds)) {
    fprintf(stderr, "Error: failed to get SDL3 display bounds (%s)\n", SDL_GetError());
    return false;
  }

  disp->window_width = disp_bounds.w * 0.5;
  disp->window_height = disp_bounds.h * 0.5;

  disp->internal_width = width;
  disp->internal_height = height;


  disp->window = SDL_CreateWindow(title, disp->window_width, disp->window_height, SDL_WINDOW_RESIZABLE);
  if(disp->window == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 window (%s)\n", SDL_GetError());
    return false;
  }

  disp->renderer = SDL_CreateRenderer(disp->window, NULL);
  if(disp->renderer == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 renderer (%s)\n", SDL_GetError());
    return false;
  }

  if(!SDL_SetRenderLogicalPresentation(disp->renderer, disp->internal_width, disp->internal_height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
    fprintf(stderr, "Error: failed to set SDL3 logical presentation (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}


bool host_create_palette(host_display_t *disp) {

  disp->palette = SDL_CreatePalette(disp->colors_amount);

  if(disp->palette == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 palette (%s)\n", SDL_GetError());
    return false;
  }

  if(!SDL_SetPaletteColors(disp->palette, disp->palette_colors, 0, disp->colors_amount)) {
    fprintf(stderr, "Error: failed to set SDL3 palette (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}



bool host_create_surface(host_display_t *disp) {

  disp->surface = SDL_CreateSurface(disp->internal_width, disp->internal_height, disp->pixel_format);

  if(disp->surface == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 surface (%s)\n", SDL_GetError());
    return false;
  }
  
  if(!host_create_palette(disp)) { return false; }

  if(!SDL_SetSurfacePalette(disp->surface, disp->palette)) {
    fprintf(stderr, "Error: failed to set SDL3 surface (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}

bool host_texture_create(host_display_t* disp) {
  
  disp->texture = SDL_CreateTexture(disp->renderer, disp->texture_format, disp->access, disp->internal_width, disp->internal_height);

  if(disp->texture == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 texture (%s)\n", SDL_GetError());
    return false;
  } 

  if(!SDL_SetTextureScaleMode(disp->texture, disp->filter_mode)) { 
    fprintf(stderr, "Error: failed to set SDL3 textures to scale mode (%s)\n", SDL_GetError());
    return false; 
  };

  return true;
}


// Initializes the host display, and checks the creation of each component step-by-step.
// This can be crucial for debugging later, 
// each if-statement for checking the state has a special error message with SDL_GetError() called.
bool init_host_display(host_display_t *disp, const char *title, int width, int height) {

  if(SDL_Init(SDL_INIT_VIDEO) == false) {
    fprintf(stderr, "Error: failed to initialise SDL3 display (%s)\n", SDL_GetError());
    return false;
  }

  if(host_create_window_renderer(disp, title, width, height) == false) { return false; }
  if(host_create_surface(disp) == false) { return false; };
  if(host_texture_create(disp) == false) { return false; }

  return true;
}


void render_host_display(host_display_t *disp) {
  
  SDL_RenderClear(disp->renderer);
  SDL_RenderTexture(disp->renderer, disp->texture, NULL, NULL);
  SDL_RenderPresent(disp->renderer);
}
void update_host_display(host_display_t *disp, void *disp_buf) {

  int bytes_per_pixel = SDL_BYTESPERPIXEL(disp->pixel_format);

  memcpy(disp->surface->pixels, disp_buf, disp->internal_width * disp->internal_height * bytes_per_pixel);

  SDL_Surface *locked = NULL; 

  SDL_LockTextureToSurface(disp->texture, NULL, &locked);
  SDL_BlitSurface(disp->surface, NULL, locked, NULL);
  SDL_UnlockTexture(disp->texture);
}

// Destroy all objects related to the display
void quit_host_display(host_display_t *disp) {
  
  if(disp->window != NULL) { 
    SDL_DestroyWindow(disp->window);   
    disp->window = NULL;
  }
  if(disp->renderer != NULL) { 
    SDL_DestroyRenderer(disp->renderer);
    disp->renderer = NULL;
  }
  if(disp->surface != NULL) { 
    SDL_DestroySurface(disp->surface);
    disp->surface = NULL;
  }
  if(disp->texture != NULL) {
    SDL_DestroyTexture(disp->texture);
    disp->texture = NULL;
  }
  if(disp->palette != NULL) {
    SDL_DestroyPalette(disp->palette);
    disp->palette = NULL;
  }

}
