# mymixer

`mymixer` controls various settings of Audient iD14 that were available only with official iD app for Windows/Mac.

This includes setting separate headphone, speakers, and direct monitoring volumes.

## Getting Started

```shell
$ git clone https://github.com/r00tman/mymixer
$ cd mymixer && make install
$ sudo mymixer -h
```

Now, `mymixer` is installed to `~/.local/bin` folder and is ready to use.

Notice that it needs superuser rights or a corresponding udev rule to work. Also, since it claims usb interface, snd_usb_audio might need to be restarted:
```shell
$ sudo modprobe -rv snd_usb_audio; sudo modprobe -v snd_usb_audio
```
