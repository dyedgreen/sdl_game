# Simple SDL Game

This is a personal project and in an early stage. If you still want to attempt building it,
this document is intended to outline what is required.

### Prerequisites
To build this, the following must be installed:

* SDL 2.0 [see here for details](https://www.libsdl.org/)

If there are any unprepared image assets (i.e. .png file in `/assets/`), the following tool
is required to be installed during the first build:

* ImageMagick [see here for details](https://www.imagemagick.org/)

Note that the .png versions of the assets are destroyed during the build.

### Building
Now building the game should work using the make command:
```
$ make
```

The final result will be stored in `/bin`. Note that as of now, these builds do not bundle
any dependencies and assets are simply copied next to the build output. As such, the builds
will probably not work when transferred to other systems (e.g. SDL my not be installed there).
