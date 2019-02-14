#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

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

void show_help() {
  const char help_str[] =
    "Usage: mymixer [-v VOLUME] [-s VOLUME] [-p VOLUME] [-h]\n"
    "Controls Audient iD14 volume/dm settings\n"
    "\n"
    "VOLUME is a float from 0.0 to 1.0 (to 1.1 in -v when boosted)\n"
    "  -v VOLUME       set direct monitoring volume\n"
    "  -s VOLUME       set speakers volume\n"
    "  -p VOLUME       set headphones volume\n"
    "  -h              show this help\n"
    "\n"
    "Examples:\n"
    "  mymixer -v 1.06255     set DM volume with boosting\n"
    "  mymixer -p 0.7 -s 0.0  turn HP on, mute speakers\n"
    "  mymixer -s 0.7         turn speakers on\n";

  printf("%s", help_str);
}

int main(int argc, char **argv) {
  int something = 0;
  int c;
  opterr = 0;
  while ((c = getopt(argc, argv, "v:p:s:h")) != -1) {
    switch (c) {
      case '?':
      case 'h': show_help(); return 0;
      default: something = 1; break;
    }
  }
  if (!something) {
    show_help();
    return 0;
  }

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

  opterr = 0;
  optind = 1;
  while ((c = getopt(argc, argv, "v:p:s:h")) != -1) {
    switch (c) {
      case 'v': set_vinyl_dm(atof(optarg)); break;
      case 'p': set_hp_volume(atof(optarg)); break;
      case 's': set_speaker_volume(atof(optarg)); break;
    }
  }

  libusb_release_interface(devh, 0);
  libusb_close(devh);
  libusb_exit(NULL);
}
