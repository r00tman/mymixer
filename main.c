#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

libusb_device_handle *devh = NULL;

uint16_t float_to_u16(float volume) {
  uint16_t res = -32768+32767*volume;
  return res;
}

void set_vinyl_dm(float volume) {
  uint16_t one = float_to_u16(volume);
  uint16_t zero = float_to_u16(0);
  int err = 0;
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0100, 0x3c00, (uint8_t*)&one, 2, 0);
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0101, 0x3c00, (uint8_t*)&zero, 2, 0);
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0104, 0x3c00, (uint8_t*)&zero, 2, 0);
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0105, 0x3c00, (uint8_t*)&one, 2, 0);
  if (err < 0) {
    printf("libusb_control_transfer failed: %s\n", libusb_error_name(err));
    exit(1);
  }
}

void set_hp_volume(float volume) {
  assert(volume>=0 && volume<=1);
  uint16_t vol = float_to_u16(volume);
  int err = 0;
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0203, 0x0a00, (uint8_t*)&vol, 2, 0);
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x0204, 0x0a00, (uint8_t*)&vol, 2, 0);
  if (err < 0) {
    printf("libusb_control_transfer failed: %s\n", libusb_error_name(err));
    exit(1);
  }
}

void set_speaker_volume(float volume) {
  assert(volume>=0 && volume<=1);
  uint16_t vol = float_to_u16(volume);
  int err = 0;
  err = libusb_control_transfer(devh, 0x21, 0x1, 0x1200, 0x3600, (uint8_t*)&vol, 2, 0);
  if (err < 0) {
    printf("libusb_control_transfer failed: %s\n", libusb_error_name(err));
    exit(1);
  }
}

int main() {
  int err;
  err = libusb_init(NULL);
  assert(err == LIBUSB_SUCCESS);

  devh = libusb_open_device_with_vid_pid(NULL, 0x2708, 0x0002);
  assert(devh != NULL);
  err = libusb_set_auto_detach_kernel_driver(devh, 1);
  if (err < 0) {
    printf("libusb_set_auto_detach_kernel_driver failed: %s\n", libusb_error_name(err));
    return 1;
  }
  err = libusb_claim_interface(devh, 0);
  if (err < 0) {
    printf("libusb_claim_interface failed: %s\n", libusb_error_name(err));
    return 1;
  }

  set_vinyl_dm(1.06255);
  set_hp_volume(0.7);
  set_speaker_volume(0.0);

  libusb_release_interface(devh, 0);
  libusb_close(devh);
  libusb_exit(NULL);
}
