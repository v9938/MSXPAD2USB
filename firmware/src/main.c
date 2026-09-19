////////////////////////////////////////////////////////////////////////
//
// MSXPAD2 to Switch Joystick
// Copyright 2026 @V9938
//	
//	26/09/19 V0.1		1st version
//
//	GPIOの配置は下記の通りです。(外部にPullupが必要です)
//  GPIO1 = HAT_UP    = DSUB 1pin
//  GPIO2 = HAT_DOWN  = DSUB 2pin
//  GPIO3 = HAT_LEFT  = DSUB 3pin
//  GPIO4 = HAT_RIGHT = DSUB 4pin
//  +5V   =             DSUB 5pin
//  GPIO5 = BUTTON_0  = DSUB 6pin
//  GPIO6 = BUTTON_1  = DSUB 7pin
//  GPIO7 = COM_OUT   = DSUB 8pin
//  GND   =             DSUB 9pin
//
////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"

enum {
  GPIO_HAT_UP = 1,
  GPIO_HAT_DOWN = 2,
  GPIO_HAT_LEFT = 3,
  GPIO_HAT_RIGHT = 4,
  GPIO_BUTTON_0 = 5,
  GPIO_BUTTON_1 = 6,
  GPIO_OUTPUT_LOW = 7
};

static void gpio_setup(void) {
  static uint const input_pins[] = {
    GPIO_HAT_UP, GPIO_HAT_DOWN, GPIO_HAT_LEFT, GPIO_HAT_RIGHT,
    GPIO_BUTTON_0, GPIO_BUTTON_1
  };

  for (size_t i = 0; i < count_of(input_pins); ++i) {
    gpio_init(input_pins[i]);
    gpio_set_dir(input_pins[i], GPIO_IN);
    gpio_disable_pulls(input_pins[i]);
  }

  gpio_init(GPIO_OUTPUT_LOW);
  gpio_put(GPIO_OUTPUT_LOW, false);
  gpio_set_dir(GPIO_OUTPUT_LOW, GPIO_OUT);
}

static uint8_t read_hat(void) {
  bool up = !gpio_get(GPIO_HAT_UP);
  bool down = !gpio_get(GPIO_HAT_DOWN);
  bool left = !gpio_get(GPIO_HAT_LEFT);
  bool right = !gpio_get(GPIO_HAT_RIGHT);

  if (up == down) up = down = false;
  if (left == right) left = right = false;

  if (up && right) return HORI_HAT_UP_RIGHT;
  if (down && right) return HORI_HAT_DOWN_RIGHT;
  if (down && left) return HORI_HAT_DOWN_LEFT;
  if (up && left) return HORI_HAT_UP_LEFT;
  if (up) return HORI_HAT_UP;
  if (right) return HORI_HAT_RIGHT;
  if (down) return HORI_HAT_DOWN;
  if (left) return HORI_HAT_LEFT;
  return HORI_HAT_CENTERED;
}

static gamepad_report_t read_gamepad(void) {
  gamepad_report_t report = {
    .buttons = 0,
    .hat = read_hat(),
    .x = 0x80,
    .y = 0x80,
    .z = 0x80,
    .rz = 0x80,
    .vendor = 0
  };

  if (!gpio_get(GPIO_BUTTON_0)) report.buttons |= 1u << 0;
  if (!gpio_get(GPIO_BUTTON_1)) report.buttons |= 1u << 1;
  return report;
}

static void gamepad_task(void) {
  static absolute_time_t next_report;

  if (!time_reached(next_report)) return;
  next_report = make_timeout_time_ms(5);

  gamepad_report_t const report = read_gamepad();
  if (tud_mounted() && tud_hid_ready()) {
    (void)tud_hid_report(0, &report, sizeof(report));
  }
}

int main(void) {
  gpio_setup();
  tusb_init();

  while (true) {
    tud_task();
    gamepad_task();
  }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t requested_len) {
  (void)instance;
  (void)report_id;
  (void)report_type;

  gamepad_report_t const report = read_gamepad();
  uint16_t const len = requested_len < sizeof(report) ? requested_len : sizeof(report);
  memcpy(buffer, &report, len);
  return len;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}
