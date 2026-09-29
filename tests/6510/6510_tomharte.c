/*
 *
 * NOTE: This particular tests needs a parser, this emulator uses the jansson JSON-parser. Please find a guide on how to actually install it onto your system.
 *
 */

#include <stdio.h>
#include <string.h>

#include <jansson.h>

#include "6510_test_helpers.h"


// Structure stores final values of each test in the opcodes.
typedef struct {
  uint16_t pc_;
  uint8_t a_, x_, y_, s_, p_;
} registers_json_t;

// Holds the address and data bus values, as well as whether if a READ or WRITE have occurred.
typedef struct {
  uint16_t addr;
  uint8_t data;
  char rw[6];
} cycles_json_t;


int m6502_v1_harte_tests(m65xx_t* m, char *file) {

  // Stores the error message.
  json_error_t error;

  // Loads a specific test file and checks whether if it was loaded.
  json_t *root = json_load_file(file, 0, &error);
  if(!root) {
    fprintf(stderr, "[Tom Harte 6502 v1]: failed to parse %s (%s at line %d)\n", file, error.text, error.line);
    return 0;
  }

  size_t passed = 0, failed = 0;

  size_t ntests = json_array_size(root);

  for(size_t i = 0; i < ntests; i++) {
    
    // Fetches the exact test from the file.
    json_t *test = json_array_get(root, i);

    if(!json_is_object(test)) {
      fprintf(stderr, "[Tom Harte 6502 v1]: test case at %zu is not valid\n", i);
      failed++;
    }

    bool match = 1;


    // Gets the tests name, and initialises "initial" and "final" states
    const char *name = json_string_value(json_object_get(test, "name"));
    json_t *initial = json_object_get(test, "initial");
    json_t* final = json_object_get(test, "final");

    m6510_test_init(m);

    // Fetches the starting values of the test
    m->pc = json_integer_value(json_object_get(initial, "pc"));
    m->a = json_integer_value(json_object_get(initial, "a"));
    m->x = json_integer_value(json_object_get(initial, "x"));
    m->y = json_integer_value(json_object_get(initial, "y"));
    m->s = json_integer_value(json_object_get(initial, "s"));
    m->p = json_integer_value(json_object_get(initial, "p"));

    // Initialises the RAM entries with the appropriate data
    json_t *ram = json_object_get(initial, "ram");
    if (json_is_array(ram)) {
      size_t size = json_array_size(ram);
      for (size_t j = 0; j < size; j++) {
        json_t *entry = json_array_get(ram, j);
        if (json_is_array(entry) && json_array_size(entry) == 2) {
          uint16_t address = json_integer_value(json_array_get(entry, 0));
          uint8_t data = json_integer_value(json_array_get(entry, 1));
          wb(address, data);
        }
      }
    }

    // Sets the address bus with the starting value of the PC.
    m6510_set_abus(m, m->pc);

    // Writes the value of PC in the starting address
    m6510_set_dbus(m, rb(json_integer_value(json_object_get(initial, "pc"))));

    json_t *cycles = json_object_get(test, "cycles");
    size_t cycles_total = json_array_size(cycles);
   
    cycles_json_t cycle[cycles_total];
    // printf("\nTest: %s\n", name);
    for(size_t k = 0; k < cycles_total; k++) {
      json_t *cyclei = json_array_get(cycles, k);

      // Each cycle, address, data and the M6510_RW_PIN signal is retrieved (the expected output) and compared with the actual output.
      cycle[k].addr = json_integer_value(json_array_get(cyclei, 0));
      cycle[k].data = json_integer_value(json_array_get(cyclei, 1));
      strncpy(cycle[k].rw, json_string_value(json_array_get(cyclei, 2)), sizeof(cycle[k].rw) - 1);
      cycle[k].rw[sizeof(cycle[k].rw) - 1] = '\0';
      
      uint16_t addr = m6510_get_abus(m);
      uint8_t data = m6510_get_dbus(m);

      // In 6502-style bus timing, PHI2 is the external bus phase:
      //  - M6510_RW_PIN=0: CPU drives data bus -> memory write happens during this cycle's bus phase
      //  - M6510_RW_PIN=1: memory must drive data bus -> CPU will read during this cycle's bus phase
      //
      // This harness models that as:
      //  - Before tick: commit the *current* cycle's write (if M6510_RW_PIN=0)
      //  - After  tick: present data for the *next* cycle's read (if M6510_RW_PIN=1)

      // Handle bus side-effects of the current cycle.
      // If M6510_RW_PIN is low, the CPU is driving the data bus during PHI2,
      // so we must commit the write now. When a new cycle is reached, it enters the PHI1, where it READS. 
      if(!m6510_check_pin(m, M6510_RW_PIN)) {
        wb(addr, data);
      }

      // Advance the CPU to the NEXT cycle.
      // Internally this transitions through PHI1 and PHI2,
      // and updates address, M6510_RW, and internal state.

      m6510_test_tick(m);

      strncpy(cycle[k].rw, m6510_check_pin(m, M6510_RW_PIN) ? "read" : "write", 6);

      // Does the checking whether if the actual output is equal to the expected output.
      if(addr != cycle[k].addr) {
        printf("[Tom Harte 6502 v1]: invalid address at cycle: %zu | Actual: [0x%04X] | Expected: [0x%04X] \n",
               k+1, addr, cycle[k].addr); 
        match = 0;
      }
      if(data != cycle[k].data) {
        printf("[Tom Harte 6502 v1]: invalid data at cycle: %zu | Actual: [0x%02X] | Expected: [0x%02X] \n", 
               k+1, data, cycle[k].data);
        match = 0;
      }
      if(strncmp(cycle[k].rw, m6510_check_pin(m, M6510_RW_PIN) ? "read" : "write", 6)) {
        printf("[Tom Harte 6502 v1]: invalid action at cycle: %zu | Actual: [%s] | Expected: [%s] \n", 
               k+1, m6510_check_pin(m, M6510_RW_PIN) ? "read" : "write", cycle[k].rw);
        match = 0;
      }

      // Retrieves the new data from the data bus and address bus, when we reach a new cycle.
      addr = m6510_get_abus(m);
      data = m6510_get_dbus(m);
      // Prepare memory response for the NEXT cycle.
      // If M6510_RW_PIN is high, memory must drive the data bus
      // ("memory must drive" in this case refers to actively forcing and electrical value to the bus.)
      // so the CPU can sample it during this cycle's PHI2.
      if(m6510_check_pin(m, M6510_RW_PIN)) {
        m6510_set_dbus(m, rb(addr));
      }
    }

    registers_json_t t;

    t.pc_ = json_integer_value(json_object_get(final, "pc"));
    t.a_ = json_integer_value(json_object_get(final, "a"));
    t.x_ = json_integer_value(json_object_get(final, "x"));
    t.y_ = json_integer_value(json_object_get(final, "y"));
    t.s_ = json_integer_value(json_object_get(final, "s"));
    t.p_ = json_integer_value(json_object_get(final, "p"));

    if (m->pc != t.pc_) {
      printf("[Tom Harte 6502 v1]: PC mismatch in test %s | Actual: [%04X] | Expected: [%04X]\n", name, m->pc, t.pc_);
      match = 0;
    }
    if (m->a != t.a_) {
      printf("[Tom Harte 6502 v1]: A mismatch in test %s | Actual: [%02X] | Expected: [%02X]\n", name, m->a, t.a_);
      match = 0;
    }
    if (m->x != t.x_) {
      printf("[Tom Harte 6502 v1]: X mismatch in test %s | Actual: [%02X] | Expected: [%02X]\n", name, m->x, t.x_);
      match = 0;
    }
    if (m->y != t.y_) {
      printf("[Tom Harte 6502 v1]: Y mismatch in test %s | Actual: [%02X] | Expected: [%02X]\n", name, m->y, t.y_);
      match = 0;
    }
    if (m->s != t.s_) {
      printf("[Tom Harte 6502 v1]: S mismatch in test %s | Actual: [%02X] | Expected: [%02X]\n", name, m->s, t.s_);
      match = 0;
    }
    if (m->p != t.p_) {
      printf("[Tom Harte 6502 v1]: P mismatch in test %s | Actual: [%02X] | Expected: [%02X]\n", name, m->p, t.p_);
      match = 0;
    }

    json_t *fram = json_object_get(final, "ram");
    if (json_is_array(fram)) {
      size_t fsize = json_array_size(fram);
      for (size_t k = 0; k < fsize; k++) {
        json_t *entry = json_array_get(fram, k);
        if (json_is_array(entry) && json_array_size(entry) == 2) {
          uint16_t expected_addr = json_integer_value(json_array_get(entry, 0));
          uint8_t expected_data = json_integer_value(json_array_get(entry, 1));
          uint8_t actual_data = rb(expected_addr);
          if (actual_data != expected_data) {
            printf("[Tom Harte 6502 v1]: RAM mismatch at [0x%04X] | Actual: [%02X] | Expected: [%02X]\n", 
                    expected_addr, actual_data, expected_data);
          }
        }
      }
    }
    if (match) { passed++; } else { failed++; }
  }
  printf("Opcode: [0x%02X] | Tests passed: %zu, Tests failed: %zu\n", m->ir, passed, failed);
  json_decref(root);

  if((passed % 10000) == 0) { return 1; } else { return 0; }
}
