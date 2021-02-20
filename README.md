NanoVG for Deko3D
==========

NanoVG is small antialiased vector graphics rendering library. This project is a custom fork of [nanovg-deko3d](https://github.com/Adubbz/nanovg-deko3d), which ports [nanovg](https://github.com/memononen/nanovg) to [deko3d](https://github.com/devkitPro/deko3d).

## Building
The following packages are required for building:
* `devkitA64`
* `libnx`
* `deko3d`
* `switch-glm`

After installing required packages, run the following command to build library:
> `make -j`

## Example
An example of using this library can be found [here](https://github.com/eXhumer/hello-nanovg).

## License
The library is licensed under [zlib license](LICENSE).

### Dependencies
* fincs' deko3d framework is licensed under [zlib license](source/framework/LICENSE).

## Links
* [`stb_truetype` & `std_image`](https://github.com/nothings/stb).