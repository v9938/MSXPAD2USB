#include <stddef.h>
#include <string.h>

#include "tusb.h"

#define USB_VID 0x0f0d
#define USB_PID 0x0092
#define USB_BCD 0x0200

static tusb_desc_device_t const desc_device = {
  .bLength = sizeof(tusb_desc_device_t),
  .bDescriptorType = TUSB_DESC_DEVICE,
  .bcdUSB = USB_BCD,
  .bDeviceClass = 0,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
  .idVendor = USB_VID,
  .idProduct = USB_PID,
  .bcdDevice = 0x0100,
  .iManufacturer = 1,
  .iProduct = 2,
  .iSerialNumber = 0,
  .bNumConfigurations = 1
};

uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&desc_device;
}

// Exact 90-byte report descriptor captured from HORI 0f0d:0092.
// Input report layout (8 bytes, no Report ID):
//   bytes 0-1: buttons 1-13 followed by three constant padding bits
//   byte 2:    low nibble is Hat Switch (0-7 directions, 8-15 neutral),
//              high nibble is constant padding
//   bytes 3-6: unsigned 8-bit X, Y, Z, and Rz axes (0-255)
//   byte 7:    vendor-defined usage 0xff00/0x20
// Output report layout (8 bytes, no Report ID):
//   bytes 0-7: vendor-defined usage 0xff00/0x2621
static uint8_t const desc_hid_report[] = {
  // Generic Desktop / Game Pad application collection.
  0x05, 0x01, 0x09, 0x05, 0xa1, 0x01,
  // Boolean range and 13 one-bit Button usages.
  0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45, 0x01,
  0x75, 0x01, 0x95, 0x0d, 0x05, 0x09, 0x19, 0x01,
  0x29, 0x0d, 0x81, 0x02, 0x95, 0x03, 0x81, 0x01,
  // Four-bit Hat Switch with null state, plus four constant padding bits.
  0x05, 0x01, 0x25, 0x07, 0x46, 0x3b, 0x01, 0x75,
  0x04, 0x95, 0x01, 0x65, 0x14, 0x09, 0x39, 0x81,
  0x42, 0x65, 0x00, 0x95, 0x01, 0x81, 0x01, 0x26,
  // Four unsigned 8-bit axes: X, Y, Z, and Rz.
  0xff, 0x00, 0x46, 0xff, 0x00, 0x09, 0x30, 0x09,
  0x31, 0x09, 0x32, 0x09, 0x35, 0x75, 0x08, 0x95,
  // One vendor-defined input byte and eight vendor-defined output bytes.
  0x04, 0x81, 0x02, 0x06, 0x00, 0xff, 0x09, 0x20,
  0x95, 0x01, 0x81, 0x02, 0x0a, 0x21, 0x26, 0x95,
  // End application collection.
  0x08, 0x91, 0x02, 0xc0
};

_Static_assert(sizeof(desc_hid_report) == 90, "HORI report descriptor must be 90 bytes");

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  (void)instance;
  return desc_hid_report;
}

enum {
  ITF_NUM_HID,
  ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN)

static uint8_t const desc_configuration[] = {
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                        0, 500),
  TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                           sizeof(desc_hid_report), 0x02, 0x81, 64, 5)
};

_Static_assert(sizeof(desc_configuration) == 41, "HORI configuration descriptor must be 41 bytes");

uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;
  return desc_configuration;
}

static char const *const string_desc_arr[] = {
  (char const[]){0x09, 0x04},
  "HORI CO.,LTD.",
  "POKKEN CONTROLLER"
};

static uint16_t desc_string[32];

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;
  size_t count;

  if (index == 0) {
    memcpy(&desc_string[1], string_desc_arr[0], 2);
    count = 1;
  } else {
    if (index >= TU_ARRAY_SIZE(string_desc_arr)) return NULL;
    char const *str = string_desc_arr[index];
    count = strlen(str);
    if (count > TU_ARRAY_SIZE(desc_string) - 1) count = TU_ARRAY_SIZE(desc_string) - 1;
    for (size_t i = 0; i < count; ++i) desc_string[1 + i] = (uint8_t)str[i];
  }

  desc_string[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * count + 2));
  return desc_string;
}
