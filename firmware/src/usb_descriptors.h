#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

#include <stdint.h>

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
