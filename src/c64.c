#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "c64.h"
#include "c64_bus.h"
#include "c64_bus_helpers.h"

static uint8_t c64_port_in(void *user_data);
static void c64_port_out(uint8_t data, void *user_data);

extern SDL_Color c64_colors[16] = {
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

host_display_t *c64_init_display(void) {

  host_display_t *display = malloc(sizeof(host_display_t));
  memset(display, 0, sizeof(host_display_t));

  display->pixel_format = SDL_PIXELFORMAT_INDEX8;
  display->texture_format = SDL_PIXELFORMAT_XRGB8888;
  display->access = SDL_TEXTUREACCESS_STREAMING;
  display->filter_mode = SDL_SCALEMODE_NEAREST;

  display->colors_amount = 16;
  display->palette_colors = c64_colors;

  init_host_display(display, "Sea64", 320, 200);

  return display;
}

c64_t *c64_init(void) {
 
  c64_t *c64 = malloc(sizeof(c64_t));
  memset(c64, 0, sizeof(c64_t));
  c64->display = c64_init_display();

  c64->m6510 = m6510_init();
  // c64->m6510->port->in_extern_device = c64_port_in;
  // c64->m6510->port->out_extern_device = c64_port_out;
  // c64->m6510->port->user_data = c64;

  c64->c64_pins = c64->m6510->m6510_pins;



  return c64;
}

void c64_tick(c64_t* c64) {
  m65xx_t *c = c64->m6510;

  c->m6510_pins = c64->c64_pins;
  m6510_tick(c);

  const uint16_t main_address = m6510_get_abus(c); 

  // Manipulated by other chips that have access to the main bus as well. 
  c64->c64_pins = c->m6510_pins & ~(M6510_RDY | M6510_AEC | M6510_NMI | M6510_IRQ);

  c64->master_clock++;
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
    c64_tick(c64);
    // update_host_display(c64->display, c64->vic->video_buffer);
    render_host_display(c64->display);
  }


  return;
}


static uint8_t c64_port_in(void *user_data) {
  c64_t *c64 = (c64_t*) user_data;
  uint8_t data = 0;

  return data;
}
static void c64_port_out(uint8_t data, void *user_data) {
  c64_t *c64 = (c64_t*) user_data;
  data = 0;
}
