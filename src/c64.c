#include <string.h>
#include <stdbool.h>

#include "c64_bus.h"

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

void c64_init_display(c64_t *c64) {
  
  c64->display->pixel_format = SDL_PIXELFORMAT_INDEX8;
  c64->display->texture_format = SDL_PIXELFORMAT_XRGB8888;
  c64->display->access = SDL_TEXTUREACCESS_STREAMING;
  c64->display->filter_mode = SDL_SCALEMODE_NEAREST;

  c64->display->colors_amount = 16;
  c64->display->palette_colors = c64_colors;

  init_host_display(c64->display, "Sea64", 320, 200);
}

void c64_init(c64_t *c64) {

  m65xx_init(c64->m6510);
  vic_ii_init(c64->vic);

  c64_init_display(c64);

  memset(c64->ram, 0, 0x10000);
  memset(c64->kernal_rom, 0, 0x1FFF);
  memset(c64->basic_rom, 0, 0x1FFF); 
  memset(c64->char_rom, 0, 0xFFF);

  c64->main_clock = 0;

  // color_ram
}

void c64_run(c64_t* c64) {

  bool running = true;
  SDL_Event event;

  // Clear memory for SDL3, in order to prevent garbage value reads.
  SDL_zero(event);

  while(running) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) { running = false; }
    }
  }

  return;
}
