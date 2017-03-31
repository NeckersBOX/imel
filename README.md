# What's Imel

**Imel** is a library for images elaboration. It include tools to create, draw, save and modify different images types with **more than 240 functions**. It was designed to be easy to use with a good compromise between speed and memory usage.

An image will be elaborated in a ImelImage type ( see documentation for more details ). It works with RGB channels plus a level to identify an image layer or the alpha if its value is between 0 and -255.

Imel is written in **C** and released under **GNU GPL 3** license. It can be included in C and C++ projects. 

# Installation

Imel uses two libraries to read TrueType fonts and open and save in different
image formats. Before you compile it you have to install these libraries:

* *libfreetype6*  Required. To read TrueType fonts
* *libfreeimage3* Required. To open and save in different image format
* *libstdc++6*    Optional. To compile a static library.

On Debian-like system you can install these libraries with the command:
```
apt-get install libfreetype6-dev libfreeimage-dev
```

After this preliminary actions you can compile the source with _make_.
```
make
```
**Note:** For 64 bit systems you have to add _flags='-fPIC'_
 
Then you can install Imel with:
```
make install
```

To clean make generated files you can do:
```
make clean
```

To uninstall:
```
make uninstall
```

( For debug you can call _make_ with _debug=true_ option )

# Documentation

From the 3.0 version the documentation was rewritten with **Doxygen**.
The Doxygen default configuration can be found in _imel_src_ folder and
it's configured to generate HTML, LATEX and MAN output. 
**Note:** HTML output was patched with a bootstrap theme

You can generate the documentation with the command:
```
make imel-doc
```

And if you want clean it
```
make imel-doc-clean
```

Online documentation can be found following these links:
[HTML Documentation](http://www.neckersbox.eu/docs/imel-html-doc/index.html)
[PDF Documentation](http://www.neckersbox.eu/docs/Imel-3.0-Documentation.pdf)

# Quick Start

To use Imel in your project you have only to include its header with:
```
#include <imel.h>
```

And compile with
```
gcc myfile.c -o myprogram -limel
```

## Image open and save

```
#include <imel.h>

int main (int argc, char *argv[]) 
{
 ImelImage *image;
 
 image = imel_image_new_from (argv[1], 0, NULL);
 if ( !image )
      return 1;
 
 imel_image_save_png (image, "image.png", IMEL_PNG_Z_DEFAULT_COMPRESSION, NULL);
 imel_image_free (image);
 
 return 0;
}
```

Others examples can be found in some function description

# Imel Logic

Imel have, hope, an own logic to define its functions name:
```
imel_category_action ()
```

The _category_ can be:
* *image*:          Includes functions to elaborate the images with effects, filters,
                    rotations and much more.
* *image_fill*:     Includes functions to fill an image area with colors or levels.
* *image_new_from*: Includes functions to load an existed image.
* *image_save*:     Includes functions to save in different image format.
* *color*:          Includes functions to operate with a single color.
* *pixel*:          Includes functions to operate with the single pixels.
* *draw*:           Includes functions to draw in an image.
* *point*:          Includes functions to reference a point in an image.
* *font*:           Includes functions to write a string in an image.
* *value*:          Includes functions to convert from a value to another one.

Special functions are:
* _imel_printf_debug ()_:  Print a message only if imel was compiled with debug=true.
* _imel_enable_brush ()_:  Enabled the draw with a brush chosen.
* _imel_disable_brush ()_: Disable the draw with a brush. 

The functions in the category _image_new_from_ have this format:

* imel_image_new_from *format*
* imel_image_new_from *format* handle
* imel_image_new_from *format* memory

The functions in the category _image_save_ have the same format without
_memory_ action.

# Image save and open format

| Image Format | Open | Open Flags | Save | Save Flags |
|--------------|:----:|:----------:|:----:|:----------:|
| BMP	       | Yes  | No         | Yes  | Yes        |
| CUT          | Yes  | No         | No   | No         |
| DDS          | Yes  | No         | No   | No         |
| EXR          | Yes  | No         | No   | No         |
| GIF          | Yes  | Yes        | No   | No         |
| HDR          | Yes  | No         | No   | No         |
| ICO          | Yes  | Yes        | No   | No         |
| IFF          | Yes  | No         | No   | No         |
| IMEL         | Yes  | No         | Yes  | No         |
| J2K          | Yes  | No         | Yes  | Yes        |
| JNG          | Yes  | No         | No   | No         |
| JP2          | Yes  | No         | Yes  | Yes        |
| JPEG         | Yes  | Yes        | Yes  | Yes        |
| KOALA        | Yes  | No         | No   | No         |
| MNG          | Yes  | No         | No   | No         |
| PBM          | Yes  | No         | No   | No         |
| PBMRAW       | Yes  | No         | No   | No         |
| PCD          | Yes  | Yes        | No   | No         |
| PCX          | Yes  | No         | No   | No         |
| PGM          | Yes  | No         | No   | No         |
| PGMRAW       | Yes  | No         | No   | No         |
| PNG          | Yes  | Yes        | Yes  | Yes        |
| PPM          | Yes  | No         | Yes  | No         |
| PPMRAW       | Yes  | No         | Yes  | No         |
| PSD          | Yes  | No         | No   | No         |
| RAS          | Yes  | No         | No   | No         |
| RAW          | Yes  | Yes        | No   | No         |
| SGI          | Yes  | No         | No   | No         |
| TARGA        | Yes  | No         | No   | No         |
| TIFF         | Yes  | Yes        | Yes  | Yes        |
| WBMP         | Yes  | No         | Yes  | No         |
| XBM          | Yes  | No         | No   | No         |
| XPM          | Yes  | No         | Yes  | No         |

