#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include <stdint.h>

// Switch button masks for HORI 0f0d:0092; bits 13-15 are descriptor padding.
enum {
  HORI_BUTTON_Y = 1u << 0,
  HORI_BUTTON_B = 1u << 1,
  HORI_BUTTON_A = 1u << 2,
  HORI_BUTTON_X = 1u << 3,
  HORI_BUTTON_L = 1u << 4,
  HORI_BUTTON_R = 1u << 5,
  HORI_BUTTON_ZL = 1u << 6,
  HORI_BUTTON_ZR = 1u << 7,
  HORI_BUTTON_MINUS = 1u << 8,
  HORI_BUTTON_PLUS = 1u << 9,
  HORI_BUTTON_L_STICK = 1u << 10,
  HORI_BUTTON_R_STICK = 1u << 11,
  HORI_BUTTON_HOME = 1u << 12,
  HORI_BUTTON_RESERVED_13 = 1u << 13,
  HORI_BUTTON_RESERVED_14 = 1u << 14,
  HORI_BUTTON_RESERVED_15 = 1u << 15
};

enum {
  HORI_HAT_UP = 0,
  HORI_HAT_UP_RIGHT,
  HORI_HAT_RIGHT,
  HORI_HAT_DOWN_RIGHT,
  HORI_HAT_DOWN,
  HORI_HAT_DOWN_LEFT,
  HORI_HAT_LEFT,
  HORI_HAT_UP_LEFT,
  HORI_HAT_CENTERED
};

typedef struct __attribute__((packed)) {
  uint16_t buttons;
  uint8_t hat;
  uint8_t x;
  uint8_t y;
  uint8_t z;
  uint8_t rz;
  uint8_t vendor;
} gamepad_report_t;

_Static_assert(sizeof(gamepad_report_t) == 8, "HORI input report must be 8 bytes");

#endif
