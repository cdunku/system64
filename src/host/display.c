#include <stdio.h>
#include <stdbool.h>

#include "display.h"

SDL_Color c64_colors[16] = {
  { 0x00, 0x00, 0x00, 0xFF }, // Black
  { 0xFF, 0xFF, 0xFF, 0xFF }, // White
  { 0x88, 0x00, 0x00, 0xFF }, // Red
  { 0xAA, 0xFF, 0xEE, 0xFF }, // Cyan
  { 0xCC, 0x44, 0xCC, 0xFF }, // Violet/purple
  { 0x00, 0xCC, 0x55, 0xFF }, // Green
  { 0x00, 0x00, 0xAA, 0xFF }, // Blue
  { 0xEE, 0xEE, 0x77, 0xFF }, // Yellow
  { 0xDD, 0x88, 0x55, 0xFF }, // Orange
  { 0x66, 0x44, 0x00, 0xFF }, // Brown
  { 0xFF, 0x77, 0x77, 0xFF }, // Light red 
  { 0x33, 0x33, 0x33, 0xFF }, // Dark grey/grey 1
  { 0x77, 0x77, 0x77, 0xFF }, // Grey 2
  { 0xAA, 0xFF, 0x66, 0xFF }, // Light green 
  { 0x00, 0x88, 0xFF, 0xFF }, // Light blue 
  { 0xBB, 0xBB, 0xBB, 0xFF }, // Light grey/grey 3
};

bool host_create_window_renderer(host_display_t *disp, int width, int height) {

  disp->window = SDL_CreateWindow("Sea64", width, height, 0);
  if(disp->window == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 window (%s)\n", SDL_GetError());
    return false;
  }

  disp->renderer = SDL_CreateRenderer(disp->window, NULL);
  if(disp->renderer == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 renderer (%s)\n", SDL_GetError());
    return false;
  }

  if(!SDL_SetRenderLogicalPresentation(disp->renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
    fprintf(stderr, "Error: failed to set SDL3 logical presentation (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}


bool host_create_palette(host_display_t *disp) {

  disp->palette = SDL_CreatePalette(16);

  if(disp->palette == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 palette (%s)\n", SDL_GetError());
    return false;
  }

  if(!SDL_SetPaletteColors(disp->palette, c64_colors, 0, 16)) {
    fprintf(stderr, "Error: failed to set SDL3 palette (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}



bool host_create_surface(host_display_t *disp, int width, int height) {

  disp->surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_INDEX8);

  if(disp->surface == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 surface (%s)\n", SDL_GetError());
    return false;
  }
  
  if(host_create_palette(disp) == false) { return false; }

  if(!SDL_SetSurfacePalette(disp->surface, disp->palette)) {
    fprintf(stderr, "Error: failed to set SDL3 surface (%s)\n", SDL_GetError());
    return false;
  }

  return true;
}

bool host_texture_create(host_display_t* disp, int width, int height) {
  
  disp->texture = SDL_CreateTexture(disp->renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

  if(disp->texture == NULL) {
    fprintf(stderr, "Error: failed to create SDL3 texture (%s)\n", SDL_GetError());
    return false;
  } 

  if(!SDL_SetTextureScaleMode(disp->texture, SDL_SCALEMODE_NEAREST)) { 
    fprintf(stderr, "Error: failed to set SDL3 textures to scale mode (%s)\n", SDL_GetError());
    return false; 
  };

  return true;
}


// Initializes the host display, and checks the creation of each component step-by-step.
// This can be crucial for debugging later, 
// each if-statement for checking the state has a special error message with SDL_GetError() called.
bool init_host_display(host_display_t *disp, int width, int height) {

  if(!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Error: failed to initialise SDL3 display (%s)\n", SDL_GetError());
    return false;
  }

  if(host_create_window_renderer(disp, width, height) == false) { return false; }
  if(host_create_surface(disp, width, height) == false) { return false; };
  if(host_texture_create(disp, width, height) == false) { return false; }

  return true;
}


void render_host_display(host_display_t *disp);
void update_host_display(host_display_t *disp, void *disp_buf, SDL_PixelFormat pix_form) {

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
