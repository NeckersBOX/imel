/*
 * "image_new_from.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
*/

#define _BSD_SOURCE
#define _GNU_SOURCE
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <FreeImage.h>
#include "header.h"
/**
 * @file image_new_from.c
 * @author Davide Francesco Merico
 * @brief This file contains function to load different image format.
 */
 
#ifndef DOXYGEN_IGNORE_DOC

extern ImelImage *imel_image_new (ImelSize width, ImelSize height);
extern void imel_image_free (ImelImage *image);

#endif

static ImelImage *imel_image_new_from_core (FIBITMAP *bitmap, long int level)
{
 ImelImage *image;
 RGBQUAD value;
 ImelSize y, x;
 FREE_IMAGE_COLOR_TYPE color_type;
 bool has_alpha;
 FIBITMAP *_bmp;
 
 return_var_if_fail (bitmap, NULL);
 
 color_type = FreeImage_GetColorType (bitmap);
 has_alpha = FreeImage_IsTransparent (bitmap);
 
 if ( color_type != FIC_RGB && color_type != FIC_RGBALPHA && color_type != FIC_CMYK ) 
      _bmp = FreeImage_ConvertTo24Bits (bitmap);
      
 image = imel_image_new (FreeImage_GetWidth (bitmap), FreeImage_GetHeight (bitmap));
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) { 
             if ( color_type != FIC_RGB && color_type != FIC_RGBALPHA && color_type != FIC_CMYK ) 
                  FreeImage_GetPixelColor (_bmp, x, image->height - (y + 1), &value);
             else FreeImage_GetPixelColor (bitmap, x, image->height - (y + 1), &value);
             
             switch ( color_type ) {
               case FIC_MINISBLACK:
                  image->pixel[y][x].red   = /* see below */
                  image->pixel[y][x].green = /* see below */
                  image->pixel[y][x].blue  = value.rgbRed;
                  break;
               case FIC_MINISWHITE:
                  image->pixel[y][x].red   = /* see below */
                  image->pixel[y][x].green = /* see below */
                  image->pixel[y][x].blue  = 255 - value.rgbRed;
                  break;
               case FIC_PALETTE:
               case FIC_RGB:
               case FIC_RGBALPHA:
                  image->pixel[y][x].red   = value.rgbRed;
                  image->pixel[y][x].green = value.rgbGreen;
                  image->pixel[y][x].blue  = value.rgbBlue;
                  break;
               case FIC_CMYK:
                  image->pixel[y][x].red   = (ImelColor) (((double) (255 - value.rgbReserved)) * 
                                                         (((double) (255 - value.rgbRed)) / 255.f));
                  image->pixel[y][x].green = (ImelColor) (((double) (255 - value.rgbReserved)) * 
                                                         (((double) (255 - value.rgbGreen)) / 255.f));
                  image->pixel[y][x].blue  = (ImelColor) (((double) (255 - value.rgbReserved)) * 
                                                         (((double) (255 - value.rgbBlue)) / 255.f));
                  break;
             }
              
             if ( has_alpha ) {
                  if ( value.rgbReserved == 0xff ) 
                       image->pixel[y][x].level = level;
                  else image->pixel[y][x].level = -255 + value.rgbReserved;
             }
             else image->pixel[y][x].level = level;
       }
 }
  
 if ( color_type != FIC_RGB && color_type != FIC_RGBALPHA && color_type != FIC_CMYK ) 
      FreeImage_Unload (_bmp);
      
 return image;
}

/**
 * @brief Load an IMEL image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_imel_handle
 */
ImelImage *imel_image_new_from_imel (const char *filename, ImelError *error)
{
 ImelImage *l_image;
 FILE *of;
 ImelSize y, x;
 char sign_confirm[16];
 const char sign[16] = "\x00\x01Imel\xff\xffSign\x00\x00\x00\x00";


 return_var_if_fail (filename, NULL);

 if ( !(of = fopen(filename, "rb")) ) {
      imel_printf_debug ("imel_image_new_from_imel", filename, "warning", 
                         strerror (errno));

      if ( error ) {
           error->code = errno;
           error->description = strdup (strerror (errno));
      }

      return NULL;
 }

 fread (sign_confirm, sizeof (char), 16, of);
 if ( memcmp (sign_confirm, sign, 16) ) {
      imel_printf_debug ("imel_image_new_from_imel", filename, "warning",
                         "backward compatibility enabled");

      rewind (of);
 }

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 fread (&(l_image->width), sizeof (ImelSize), 1, of);
 fread (&(l_image->height), sizeof (ImelSize), 1, of);
 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *)
                                         + __memory_buffer);
 for ( y = 0; y < l_image->height; y++ ) {
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel)
                                                 + __memory_buffer);
       for ( x = 0; x < l_image->width; x++ )
             fread (&(l_image->pixel[y][x]), sizeof (ImelPixel), 1, of);
 }
 fclose (of);

 return l_image;
}

/**
 * @brief Load an IMEL image from an already open file.
 * 
 * @param of Initialized FILE type.
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_imel
 */
ImelImage *imel_image_new_from_imel_handle (FILE *of, ImelError *error)
{
 ImelImage *l_image;
 ImelSize y, x;
 char sign_confirm[16];
 const char sign[16] = "\x00\x01Imel\xff\xffSign\x00\x00\x00\x00";


 return_var_if_fail (of, NULL);

 fread (sign_confirm, sizeof (char), 16, of);
 if ( memcmp (sign_confirm, sign, 16) ) {
      imel_printf_debug ("imel_image_new_from_imel_handle", NULL, "warning",
                         "backward compatibility enabled");
      rewind (of);
 }

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 fread (&(l_image->width), sizeof (ImelSize), 1, of);
 fread (&(l_image->height), sizeof (ImelSize), 1, of);
 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *)
                                         + __memory_buffer);
 for ( y = 0; y < l_image->height; y++ ) {
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel)
                                                 + __memory_buffer);
       for ( x = 0; x < l_image->width; x++ )
             fread (&(l_image->pixel[y][x]), sizeof (ImelPixel), 1, of);
 }
 fclose (of);

 return l_image;
}

/**
 * @brief Load a raw image from file name
 * 
 * This function can load a raw image with no header and different bits value
 * to rappresent color channels.
 * 
 * @param filename Name of the image with extension
 * @param width Image width
 * @param height Image height
 * @param bits_red Bits of each red channel or 0 if it doesn't exist.
 * @param bits_green Bits of each green channel or 0 if it doesn't exist.
 * @param bits_blue Bits of each blue channel or 0 if it doesn't exist.
 * @param bits_level Bits of each level channel or 0 if it doesn't exist.
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 */
ImelImage *imel_image_new_from_raw (const char *filename, ImelSize width, ImelSize height, 
                                    int bits_red, int bits_green, int bits_blue, 
                                    int bits_level, ImelError *error)
{
 FILE *of;
 ImelSize y, x;
 static int32_t var24, var32;
 static int16_t var16;
 static int8_t var8;
 ImelImage *l_image;
 static int total_bits = 0, mask[4], shift[2][4];
  
#define __imel_valid_bit_width(val) (((val) > 0) ? (val) : 0)
#define __current_int(bits) ((bits == 32) ? var32 : \
                             (bits == 24) ? var24 : \
                             (bits == 16) ? var16 : var8)

 return_var_if_fail (filename && width && height, NULL);
 return_var_if_fail (__imel_valid_bit_width (bits_red) < 9 &&
                     __imel_valid_bit_width (bits_green) < 9 &&
                     __imel_valid_bit_width (bits_blue) < 9 &&
                     __imel_valid_bit_width (bits_level) < 9, NULL);
 
 total_bits = __imel_valid_bit_width (bits_red) + __imel_valid_bit_width (bits_green) 
            + __imel_valid_bit_width (bits_blue) + __imel_valid_bit_width (bits_level);
 
 if ( total_bits > 0x20) {
      imel_printf_debug ("imel_image_new_from_raw", filename, "warning",
                         "%d bpp are too big for Imel.", total_bits);
      
      if ( error ) {
           error->description = strdup ("The bpp are too big for Imel.");
           error->code = IMEL_ERR_RAW_BPP;
      }
      
      return NULL;
 }
 
 if ( !total_bits ) {
      imel_printf_debug ("imel_image_new_from_raw", filename, "warning", 
                         "bpp equal to zero ( not valid )");
      
      if ( error ) {
           error->description = strdup ("bpp equal to zero ( not valid ).");
           error->code = IMEL_ERR_RAW_BPP2;
      }
      
      return NULL;
 }
 
 if ( total_bits & 7 ) {
      imel_printf_debug ("imel_image_new_from_raw", filename, "warning", 
                         "%d bpp aren't multiples of 8.", total_bits);
      
      if ( error ) {
           error->description = strdup ("The bpp aren't multiples of 8.");
           error->code = IMEL_ERR_RAW_LENGTH;
      }
      
      return NULL;
 }
 
 if ( !(of = fopen (filename, "rb")) ) { 
      imel_printf_debug ("imel_image_new_from_raw", filename, "warning", 
                         strerror (errno));
                         
      if ( error ) {
           error->code = errno;
           error->description = strdup (strerror (errno));
      }
      
      return NULL;
 }
 fseek (of, 0, SEEK_END);
 
 if ( ftell (of) % (total_bits >> 3) ) {
      imel_printf_debug ("imel_image_new_from_raw", filename, "warning", 
                         "the file length isn't valid.");
      
      if ( error ) {
           error->description = strdup ("The file length isn't valid.");
           error->code = IMEL_ERR_RAW_FILE_LENGTH;
      }
      
      return NULL;
 }
 else rewind (of);
 
 l_image = imel_image_new (width, height);
 
 if ( bits_red > 0 ) {
      mask[0] = pow (2, bits_red) - 1;
      shift[0][0] = total_bits - bits_red;
      shift[1][0] = 8 - bits_red;
 }
 if ( bits_green > 0 ) {
      mask[1] = pow (2, bits_green) - 1;
      shift[0][1] = total_bits - (bits_green + __imel_valid_bit_width (bits_red));
      shift[1][1] = 8 - bits_green;
 }
 if ( bits_blue > 0 ) {
      mask[2] = pow (2, bits_blue) - 1;
      shift[0][2] = total_bits - (bits_blue + __imel_valid_bit_width (bits_green) 
               + __imel_valid_bit_width (bits_red));
      shift[1][2] = 8 - bits_blue;
 } 
 if ( bits_level > 0 ) {
      mask[3] = pow (2, bits_level) - 1;
      shift[1][3] = 8 - bits_level;
 }
    
 for ( y = 0; y < height; y++ ) {
       for ( x = 0; x < width; x++ ) {
                  if ( total_bits == 32 )
                       fread (&var32, sizeof (int32_t), 1, of);
             else if ( total_bits == 24 ) {
             	       fread (&var16, sizeof (int16_t), 1, of);
             	       fread (&var8, sizeof (int8_t), 1, of);
             	       
             	       var24 = (var16 << 8) | var8;
			 }
             else if ( total_bits == 16 )
                       fread (&var16, sizeof (int16_t), 1, of);
             else      fread (&var8, sizeof (int8_t), 1, of);
             
             if ( bits_red > 0 )
                  l_image->pixel[y][x].red = (ImelColor) (((__current_int (total_bits) >> shift[0][0]) 
                                           & mask[0]) << shift[1][0]);
             else l_image->pixel[y][x].red = (ImelColor) abs (bits_red);
             
             if ( bits_green > 0 ) 
                  l_image->pixel[y][x].green = (ImelColor) (((__current_int (total_bits) >> shift[0][1]) 
                                             & mask[1]) << shift[1][1]);
             else l_image->pixel[y][x].green = (ImelColor) abs (bits_green);
             
             if ( bits_blue > 0 )
                  l_image->pixel[y][x].blue = (ImelColor) (((__current_int (total_bits) >> shift[0][2]) 
                                            & mask[2]) << shift[1][2]);
             else l_image->pixel[y][x].blue = (ImelColor) abs (bits_blue);
             
             if ( bits_level > 0 )
                  l_image->pixel[y][x].level = (long int) ((__current_int (total_bits) 
                                             & mask[3]) << shift[1][3]);
             else l_image->pixel[y][x].level = (long int) abs (bits_level);
       }
 }
 
 return l_image;
}

/**
 * @brief Load a JPEG image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelJpegLoadFlags
 * @see imel_image_new_from_jpeg_handle
 * @see imel_image_new_from_jpeg_memory
 */
ImelImage *imel_image_new_from_jpeg (const char *filename, long int level, ImelJpegLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_JPEG, filename, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_jpeg", filename, 
                         "error", "Error while loading the jpeg image");

      if ( error ) {
           error->code = IMEL_ERR_JPEG_LOAD;
           error->description = strdup ("Error while loading the jpeg image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JPEG image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelJpegLoadFlags
 * @see imel_image_new_from_jpeg
 * @see imel_image_new_from_jpeg_memory
 */
ImelImage *imel_image_new_from_jpeg_handle (FILE *file, long int level, ImelJpegLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_JPEG, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_jpeg_handle", NULL, 
                         "error", "Error while loading the jpeg image");

      if ( error ) {
           error->code = IMEL_ERR_JPEG_LOAD;
           error->description = strdup ("Error while loading the jpeg image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JPEG image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelJpegLoadFlags
 * @see imel_image_new_from_jpeg
 * @see imel_image_new_from_jpeg_handle
 */
ImelImage *imel_image_new_from_jpeg_memory (uint8_t *memory, uint32_t length, long int level, 
                                            ImelJpegLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_JPEG, image_memory, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_jpeg_memory", NULL, 
                         "error", "Error while loading the jpeg image");

      if ( error ) {
           error->code = IMEL_ERR_JPEG_LOAD;
           error->description = strdup ("Error while loading the jpeg image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}
/**
 * @brief Load a PNG image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPngLoadFlags
 * @see imel_image_new_from_png_handle
 * @see imel_image_new_from_png_memory
 */
ImelImage *imel_image_new_from_png (const char *filename, long int level, ImelPngLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PNG, filename, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_png", filename, "error", 
                         "Error while loading the png image");

      if ( error ) {
           error->code = IMEL_ERR_PNG_LOAD;
           error->description = strdup ("Error while loading the png image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PNG image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPngLoadFlags
 * @see imel_image_new_from_png
 * @see imel_image_new_from_png_memory
 */
ImelImage *imel_image_new_from_png_handle (FILE *file, long int level, ImelPngLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PNG, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_png_handle", NULL, "error", 
                         "Error while loading the png image");

      if ( error ) {
           error->code = IMEL_ERR_PNG_LOAD;
           error->description = strdup ("Error while loading the png image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PNG image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPngLoadFlags
 * @see imel_image_new_from_png
 * @see imel_image_new_from_png_handle
 */
ImelImage *imel_image_new_from_png_memory (uint8_t *memory, uint32_t length, long int level, 
                                           ImelPngLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PNG, image_memory, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_png_memory", NULL, "error", 
                         "Error while loading the png image");

      if ( error ) {
           error->code = IMEL_ERR_PNG_LOAD;
           error->description = strdup ("Error while loading the png image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TIFF image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelTiffLoadFlags
 * @see imel_image_new_from_tiff_handle
 * @see imel_image_new_from_tiff_memory
 */
ImelImage *imel_image_new_from_tiff (const char *filename, long int level, ImelTiffLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_TIFF, filename, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_tiff", filename, "error", 
                         "Error while loading the tiff image");

      if ( error ) {
           error->code = IMEL_ERR_TIFF_LOAD;
           error->description = strdup ("Error while loading the tiff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TIFF image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelTiffLoadFlags
 * @see imel_image_new_from_tiff
 * @see imel_image_new_from_tiff_memory
 */
ImelImage *imel_image_new_from_tiff_handle (FILE *file, long int level, ImelTiffLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_TIFF, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_tiff_handle", NULL, "error", 
                         "Error while loading the tiff image");

      if ( error ) {
           error->code = IMEL_ERR_TIFF_LOAD;
           error->description = strdup ("Error while loading the tiff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TIFF image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelTiffLoadFlags
 * @see imel_image_new_from_tiff
 * @see imel_image_new_from_tiff_handle
 */
ImelImage *imel_image_new_from_tiff_memory (uint8_t *memory, uint32_t length, long int level, 
                                           ImelTiffLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_TIFF, image_memory, load_flags);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_tiff_memory", NULL, "error", 
                         "Error while loading the tiff image");

      if ( error ) {
           error->code = IMEL_ERR_TIFF_LOAD;
           error->description = strdup ("Error while loading the tiff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a BMP image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_bmp_handle
 * @see imel_image_new_from_bmp_memory
 */
ImelImage *imel_image_new_from_bmp (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_BMP, filename, BMP_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_bmp", filename, "error", 
                         "Error while loading the bmp image");

      if ( error ) {
           error->code = IMEL_ERR_BMP_LOAD;
           error->description = strdup ("Error while loading the bmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a BMP image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_bmp
 * @see imel_image_new_from_bmp_memory
 */
ImelImage *imel_image_new_from_bmp_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_BMP, &io, (fi_handle) file, BMP_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_bmp_handle", NULL, "error", 
                         "Error while loading the bmp image");

      if ( error ) {
           error->code = IMEL_ERR_BMP_LOAD;
           error->description = strdup ("Error while loading the bmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a BMP image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_bmp
 * @see imel_image_new_from_bmp_handle
 */
ImelImage *imel_image_new_from_bmp_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_BMP, image_memory, BMP_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_bmp_memory", NULL, "error", 
                         "Error while loading the bmp image");

      if ( error ) {
           error->code = IMEL_ERR_BMP_LOAD;
           error->description = strdup ("Error while loading the bmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a CUT image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_cut_handle
 * @see imel_image_new_from_cut_memory
 */
ImelImage *imel_image_new_from_cut (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_CUT, filename, CUT_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_cut", filename, "error",
                         "Error while loading the cut image");

      if ( error ) {
           error->code = IMEL_ERR_CUT_LOAD;
           error->description = strdup ("Error while loading the cut image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a CUT image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_cut
 * @see imel_image_new_from_cut_memory
 */
ImelImage *imel_image_new_from_cut_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_CUT, &io, (fi_handle) file, CUT_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_cut_handle", NULL, "error",
                         "Error while loading the cut image");

      if ( error ) {
           error->code = IMEL_ERR_CUT_LOAD;
           error->description = strdup ("Error while loading the cut image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a CUT image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_cut
 * @see imel_image_new_from_cut_handle
 */
ImelImage *imel_image_new_from_cut_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_CUT, image_memory, CUT_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_cut_memory", NULL, "error",
                         "Error while loading the cut image");

      if ( error ) {
           error->code = IMEL_ERR_CUT_LOAD;
           error->description = strdup ("Error while loading the cut image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a DDS image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_dds_handle
 * @see imel_image_new_from_dds_memory
 */
ImelImage *imel_image_new_from_dds (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_DDS, filename, DDS_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_dds", filename, "error",
                         "Error while loading the dds image");

      if ( error ) {
           error->code = IMEL_ERR_DDS_LOAD;
           error->description = strdup ("Error while loading the dds image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a DDS image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_dds
 * @see imel_image_new_from_dds_memory
 */
ImelImage *imel_image_new_from_dds_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_DDS, &io, (fi_handle) file, DDS_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_dds_handle", NULL, "error",
                         "Error while loading the dds image");

      if ( error ) {
           error->code = IMEL_ERR_DDS_LOAD;
           error->description = strdup ("Error while loading the dds image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a DDS image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_dds
 * @see imel_image_new_from_dds_handle
 */
ImelImage *imel_image_new_from_dds_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_DDS, image_memory, DDS_DEFAULT);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from_dds_memory", NULL, "error",
                         "Error while loading the dds image");

      if ( error ) {
           error->code = IMEL_ERR_DDS_LOAD;
           error->description = strdup ("Error while loading the dds image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XPM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xpm_handle
 * @see imel_image_new_from_xpm_memory
 */
ImelImage *imel_image_new_from_xpm (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_XPM, filename, XPM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xpm: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the xpm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XPM_LOAD;
           error->description = strdup ("Error while loading the xpm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XPM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xpm
 * @see imel_image_new_from_xpm_memory
 */
ImelImage *imel_image_new_from_xpm_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_XPM, &io, (fi_handle) file, XPM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xpm_handle: "
                       "error: %s\n", getpid (), "Error while loading the xpm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XPM_LOAD;
           error->description = strdup ("Error while loading the xpm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XPM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xpm
 * @see imel_image_new_from_xpm_handle
 */
ImelImage *imel_image_new_from_xpm_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_XPM, image_memory, XPM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xpm_memory: "
                       "error: %s\n", getpid (), "Error while loading the xpm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XPM_LOAD;
           error->description = strdup ("Error while loading the xpm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XBM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xbm_handle
 * @see imel_image_new_from_xbm_memory
 */
ImelImage *imel_image_new_from_xbm (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_XBM, filename, XBM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xbm: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the xbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XBM_LOAD;
           error->description = strdup ("Error while loading the xbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XBM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xbm
 * @see imel_image_new_from_xbm_memory
 */
ImelImage *imel_image_new_from_xbm_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_XBM, &io, (fi_handle) file, XBM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xbm_handle: "
                       "error: %s\n", getpid (), "Error while loading the xbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XBM_LOAD;
           error->description = strdup ("Error while loading the xbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a XBM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_xbm
 * @see imel_image_new_from_xbm_handle
 */
ImelImage *imel_image_new_from_xbm_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_XBM, image_memory, XBM_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_xbm_memory: "
                       "error: %s\n", getpid (), "Error while loading the xbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_XBM_LOAD;
           error->description = strdup ("Error while loading the xbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a SGI image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_sgi_handle
 * @see imel_image_new_from_sgi_memory
 */
ImelImage *imel_image_new_from_sgi (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_SGI, filename, SGI_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_sgi: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the sgi image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_SGI_LOAD;
           error->description = strdup ("Error while loading the sgi image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a SGI image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_sgi
 * @see imel_image_new_from_sgi_memory
 */
ImelImage *imel_image_new_from_sgi_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_SGI, &io, (fi_handle) file, SGI_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_sgi_handle: "
                       "error: %s\n", getpid (), "Error while loading the sgi image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_SGI_LOAD;
           error->description = strdup ("Error while loading the sgi image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a SGI image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_sgi
 * @see imel_image_new_from_sgi_handle
 */
ImelImage *imel_image_new_from_sgi_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_SGI, image_memory, SGI_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_sgi_memory: "
                       "error: %s\n", getpid (), "Error while loading the sgi image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_SGI_LOAD;
           error->description = strdup ("Error while loading the sgi image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a WBMP image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_wbmp_handle
 * @see imel_image_new_from_wbmp_memory
 */
ImelImage *imel_image_new_from_wbmp (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_WBMP, filename, WBMP_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_wbmp: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the wbmp image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_WBMP_LOAD;
           error->description = strdup ("Error while loading the wbmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a WBMP image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_wbmp
 * @see imel_image_new_from_wbmp_memory
 */
ImelImage *imel_image_new_from_wbmp_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_WBMP, &io, (fi_handle) file, WBMP_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_wbmp_handle: "
                       "error: %s\n", getpid (), "Error while loading the wbmp image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_WBMP_LOAD;
           error->description = strdup ("Error while loading the wbmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a WBMP image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_wbmp
 * @see imel_image_new_from_wbmp_handle
 */
ImelImage *imel_image_new_from_wbmp_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_WBMP, image_memory, WBMP_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_wbmp_memory: "
                       "error: %s\n", getpid (), "Error while loading the wbmp image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_WBMP_LOAD;
           error->description = strdup ("Error while loading the wbmp image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a HDR image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_hdr_handle
 * @see imel_image_new_from_hdr_memory
 */
ImelImage *imel_image_new_from_hdr (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_HDR, filename, HDR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_hdr: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the hdr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_HDR_LOAD;
           error->description = strdup ("Error while loading the hdr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a HDR image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_hdr
 * @see imel_image_new_from_hdr_memory
 */
ImelImage *imel_image_new_from_hdr_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_HDR, &io, (fi_handle) file, HDR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_hdr_handle: "
                       "error: %s\n", getpid (), "Error while loading the hdr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_HDR_LOAD;
           error->description = strdup ("Error while loading the hdr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a HDR image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_hdr
 * @see imel_image_new_from_hdr_handle
 */
ImelImage *imel_image_new_from_hdr_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_HDR, image_memory, HDR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_hdr_memory: "
                       "error: %s\n", getpid (), "Error while loading the hdr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_HDR_LOAD;
           error->description = strdup ("Error while loading the hdr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PSD image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_psd_handle
 * @see imel_image_new_from_psd_memory
 */
ImelImage *imel_image_new_from_psd (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PSD, filename, PSD_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_psd: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the psd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PSD_LOAD;
           error->description = strdup ("Error while loading the psd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PSD image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_psd
 * @see imel_image_new_from_psd_memory
 */
ImelImage *imel_image_new_from_psd_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PSD, &io, (fi_handle) file, PSD_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_psd_handle: "
                       "error: %s\n", getpid (), "Error while loading the psd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PSD_LOAD;
           error->description = strdup ("Error while loading the psd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PSD image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_psd
 * @see imel_image_new_from_psd_handle
 */
ImelImage *imel_image_new_from_psd_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PSD, image_memory, PSD_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_psd_memory: "
                       "error: %s\n", getpid (), "Error while loading the psd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PSD_LOAD;
           error->description = strdup ("Error while loading the psd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a IFF image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_iff_handle
 * @see imel_image_new_from_iff_memory
 */
ImelImage *imel_image_new_from_iff (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_IFF, filename, IFF_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_iff: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the iff image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_IFF_LOAD;
           error->description = strdup ("Error while loading the iff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a IFF image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_iff
 * @see imel_image_new_from_iff_memory
 */
ImelImage *imel_image_new_from_iff_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_IFF, &io, (fi_handle) file, IFF_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_iff_handle: "
                       "error: %s\n", getpid (), "Error while loading the iff image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_IFF_LOAD;
           error->description = strdup ("Error while loading the iff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a IFF image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_iff
 * @see imel_image_new_from_iff_handle
 */
ImelImage *imel_image_new_from_iff_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_IFF, image_memory, IFF_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_iff_memory: "
                       "error: %s\n", getpid (), "Error while loading the iff image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_IFF_LOAD;
           error->description = strdup ("Error while loading the iff image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JNG image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jng_handle
 * @see imel_image_new_from_jng_memory
 */
ImelImage *imel_image_new_from_jng (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_JNG, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jng: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the jng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JNG_LOAD;
           error->description = strdup ("Error while loading the jng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JNG image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jng
 * @see imel_image_new_from_jng_memory
 */
ImelImage *imel_image_new_from_jng_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_JNG, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jng_handle: "
                       "error: %s\n", getpid (), "Error while loading the jng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JNG_LOAD;
           error->description = strdup ("Error while loading the jng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JNG image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jng
 * @see imel_image_new_from_jng_handle
 */
ImelImage *imel_image_new_from_jng_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_JNG, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jng_memory: "
                       "error: %s\n", getpid (), "Error while loading the jng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JNG_LOAD;
           error->description = strdup ("Error while loading the jng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a KOALA image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_koala_handle
 * @see imel_image_new_from_koala_memory
 */
ImelImage *imel_image_new_from_koala (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_KOALA, filename, KOALA_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_koala: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the koala image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_KOALA_LOAD;
           error->description = strdup ("Error while loading the koala image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a KOALA image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_koala
 * @see imel_image_new_from_koala_memory
 */
ImelImage *imel_image_new_from_koala_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_KOALA, &io, (fi_handle) file, KOALA_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_koala_handle: "
                       "error: %s\n", getpid (), "Error while loading the koala image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_KOALA_LOAD;
           error->description = strdup ("Error while loading the koala image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a KOALA image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_koala
 * @see imel_image_new_from_koala_handle
 */
ImelImage *imel_image_new_from_koala_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_KOALA, image_memory, KOALA_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_koala_memory: "
                       "error: %s\n", getpid (), "Error while loading the koala image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_KOALA_LOAD;
           error->description = strdup ("Error while loading the koala image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a MNG image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_mng_handle
 * @see imel_image_new_from_mng_memory
 */
ImelImage *imel_image_new_from_mng (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_MNG, filename, MNG_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_mng: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the mng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_MNG_LOAD;
           error->description = strdup ("Error while loading the mng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a MNG image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_mng
 * @see imel_image_new_from_mng_memory
 */
ImelImage *imel_image_new_from_mng_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_MNG, &io, (fi_handle) file, MNG_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_mng_handle: "
                       "error: %s\n", getpid (), "Error while loading the mng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_MNG_LOAD;
           error->description = strdup ("Error while loading the mng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a MNG image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_mng
 * @see imel_image_new_from_mng_handle
 */
ImelImage *imel_image_new_from_mng_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_MNG, image_memory, MNG_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_mng_memory: "
                       "error: %s\n", getpid (), "Error while loading the mng image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_MNG_LOAD;
           error->description = strdup ("Error while loading the mng image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCX image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pcx_handle
 * @see imel_image_new_from_pcx_memory
 */
ImelImage *imel_image_new_from_pcx (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PCX, filename, PCX_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcx: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pcx image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCX_LOAD;
           error->description = strdup ("Error while loading the pcx image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCX image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pcx
 * @see imel_image_new_from_pcx_memory
 */
ImelImage *imel_image_new_from_pcx_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PCX, &io, (fi_handle) file, PCX_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcx_handle: "
                       "error: %s\n", getpid (), "Error while loading the pcx image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCX_LOAD;
           error->description = strdup ("Error while loading the pcx image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCX image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pcx
 * @see imel_image_new_from_pcx_handle
 */
ImelImage *imel_image_new_from_pcx_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PCX, image_memory, PCX_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcx_memory: "
                       "error: %s\n", getpid (), "Error while loading the pcx image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCX_LOAD;
           error->description = strdup ("Error while loading the pcx image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PGM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgm_handle
 * @see imel_image_new_from_pgm_memory
 */
ImelImage *imel_image_new_from_pgm (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PGM, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgm: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pgm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGM_LOAD;
           error->description = strdup ("Error while loading the pgm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PGM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgm
 * @see imel_image_new_from_pgm_memory
 */
ImelImage *imel_image_new_from_pgm_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PGM, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgm_handle: "
                       "error: %s\n", getpid (), "Error while loading the pgm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGM_LOAD;
           error->description = strdup ("Error while loading the pgm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PGM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgm
 * @see imel_image_new_from_pgm_handle
 */
ImelImage *imel_image_new_from_pgm_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PGM, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgm_memory: "
                       "error: %s\n", getpid (), "Error while loading the pgm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGM_LOAD;
           error->description = strdup ("Error while loading the pgm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PGM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgmraw_handle
 * @see imel_image_new_from_pgmraw_memory
 */
ImelImage *imel_image_new_from_pgmraw (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PGMRAW, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgmraw: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pgmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGMRAW_LOAD;
           error->description = strdup ("Error while loading the pgmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PGM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgmraw
 * @see imel_image_new_from_pgmraw_memory
 */
ImelImage *imel_image_new_from_pgmraw_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PGMRAW, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgmraw_handle: "
                       "error: %s\n", getpid (), "Error while loading the pgmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGMRAW_LOAD;
           error->description = strdup ("Error while loading the pgmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PGM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pgmraw
 * @see imel_image_new_from_pgmraw_handle
 */
ImelImage *imel_image_new_from_pgmraw_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PGMRAW, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pgmraw_memory: "
                       "error: %s\n", getpid (), "Error while loading the pgmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PGMRAW_LOAD;
           error->description = strdup ("Error while loading the pgmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a RAS image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ras_handle
 * @see imel_image_new_from_ras_memory
 */
ImelImage *imel_image_new_from_ras (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_RAS, filename, RAS_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ras: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the ras image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_RAS_LOAD;
           error->description = strdup ("Error while loading the ras image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a RAS image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ras
 * @see imel_image_new_from_ras_memory
 */
ImelImage *imel_image_new_from_ras_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_RAS, &io, (fi_handle) file, RAS_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ras_handle: "
                       "error: %s\n", getpid (), "Error while loading the ras image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_RAS_LOAD;
           error->description = strdup ("Error while loading the ras image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a RAS image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ras
 * @see imel_image_new_from_ras_handle
 */
ImelImage *imel_image_new_from_ras_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_RAS, image_memory, RAS_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ras_memory: "
                       "error: %s\n", getpid (), "Error while loading the ras image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_RAS_LOAD;
           error->description = strdup ("Error while loading the ras image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a EXR image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_exr_handle
 * @see imel_image_new_from_exr_memory
 */
ImelImage *imel_image_new_from_exr (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_EXR, filename, EXR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_exr: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the exr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_EXR_LOAD;
           error->description = strdup ("Error while loading the exr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a EXR image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_exr
 * @see imel_image_new_from_exr_memory
 */
ImelImage *imel_image_new_from_exr_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_EXR, &io, (fi_handle) file, EXR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_exr_handle: "
                       "error: %s\n", getpid (), "Error while loading the exr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_EXR_LOAD;
           error->description = strdup ("Error while loading the exr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a EXR image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_exr
 * @see imel_image_new_from_exr_handle
 */
ImelImage *imel_image_new_from_exr_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_EXR, image_memory, EXR_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_exr_memory: "
                       "error: %s\n", getpid (), "Error while loading the exr image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_EXR_LOAD;
           error->description = strdup ("Error while loading the exr image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a J2K image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_j2k_handle
 * @see imel_image_new_from_j2k_memory
 */
ImelImage *imel_image_new_from_j2k (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_J2K, filename, J2K_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_j2k: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the j2k image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_J2K_LOAD;
           error->description = strdup ("Error while loading the j2k image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a J2K image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_j2k
 * @see imel_image_new_from_j2k_memory
 */
ImelImage *imel_image_new_from_j2k_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_J2K, &io, (fi_handle) file, J2K_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_j2k_handle: "
                       "error: %s\n", getpid (), "Error while loading the j2k image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_J2K_LOAD;
           error->description = strdup ("Error while loading the j2k image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a J2K image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_j2k
 * @see imel_image_new_from_j2k_handle
 */
ImelImage *imel_image_new_from_j2k_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_J2K, image_memory, J2K_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_j2k_memory: "
                       "error: %s\n", getpid (), "Error while loading the j2k image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_J2K_LOAD;
           error->description = strdup ("Error while loading the j2k image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PPM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppm_handle
 * @see imel_image_new_from_ppm_memory
 */
ImelImage *imel_image_new_from_ppm (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PPM, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppm: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the ppm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPM_LOAD;
           error->description = strdup ("Error while loading the ppm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PPM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppm
 * @see imel_image_new_from_ppm_memory
 */
ImelImage *imel_image_new_from_ppm_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PPM, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppm_handle: "
                       "error: %s\n", getpid (), "Error while loading the ppm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPM_LOAD;
           error->description = strdup ("Error while loading the ppm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PPM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppm
 * @see imel_image_new_from_ppm_handle
 */
ImelImage *imel_image_new_from_ppm_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PPM, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppm_memory: "
                       "error: %s\n", getpid (), "Error while loading the ppm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPM_LOAD;
           error->description = strdup ("Error while loading the ppm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PPM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppmraw_handle
 * @see imel_image_new_from_ppmraw_memory
 */
ImelImage *imel_image_new_from_ppmraw (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PPMRAW, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppmraw: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the ppmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPMRAW_LOAD;
           error->description = strdup ("Error while loading the ppmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PPM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppmraw
 * @see imel_image_new_from_ppmraw_memory
 */
ImelImage *imel_image_new_from_ppmraw_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PPMRAW, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppmraw_handle: "
                       "error: %s\n", getpid (), "Error while loading the ppmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPMRAW_LOAD;
           error->description = strdup ("Error while loading the ppmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PPM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_ppmraw
 * @see imel_image_new_from_ppmraw_handle
 */
ImelImage *imel_image_new_from_ppmraw_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PPMRAW, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ppmraw_memory: "
                       "error: %s\n", getpid (), "Error while loading the ppmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PPMRAW_LOAD;
           error->description = strdup ("Error while loading the ppmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PBM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbm_handle
 * @see imel_image_new_from_pbm_memory
 */
ImelImage *imel_image_new_from_pbm (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PBM, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbm: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBM_LOAD;
           error->description = strdup ("Error while loading the pbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PBM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbm
 * @see imel_image_new_from_pbm_memory
 */
ImelImage *imel_image_new_from_pbm_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PBM, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbm_handle: "
                       "error: %s\n", getpid (), "Error while loading the pbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBM_LOAD;
           error->description = strdup ("Error while loading the pbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PBM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbm
 * @see imel_image_new_from_pbm_handle
 */
ImelImage *imel_image_new_from_pbm_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PBM, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbm_memory: "
                       "error: %s\n", getpid (), "Error while loading the pbm image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBM_LOAD;
           error->description = strdup ("Error while loading the pbm image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PBM image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbmraw_handle
 * @see imel_image_new_from_pbmraw_memory
 */
ImelImage *imel_image_new_from_pbmraw (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PBMRAW, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbmraw: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pbmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBMRAW_LOAD;
           error->description = strdup ("Error while loading the pbmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PBM image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbmraw
 * @see imel_image_new_from_pbmraw_memory
 */
ImelImage *imel_image_new_from_pbmraw_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PBMRAW, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbmraw_handle: "
                       "error: %s\n", getpid (), "Error while loading the pbmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBMRAW_LOAD;
           error->description = strdup ("Error while loading the pbmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a raw PBM image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_pbmraw
 * @see imel_image_new_from_pbmraw_handle
 */
ImelImage *imel_image_new_from_pbmraw_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PBMRAW, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pbmraw_memory: "
                       "error: %s\n", getpid (), "Error while loading the pbmraw image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PBMRAW_LOAD;
           error->description = strdup ("Error while loading the pbmraw image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TARGA image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_targa_handle
 * @see imel_image_new_from_targa_memory
 */
ImelImage *imel_image_new_from_targa (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_TARGA, filename, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_targa: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the targa image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_TARGA_LOAD;
           error->description = strdup ("Error while loading the targa image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TARGA image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_targa
 * @see imel_image_new_from_targa_memory
 */
ImelImage *imel_image_new_from_targa_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_TARGA, &io, (fi_handle) file, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_targa_handle: "
                       "error: %s\n", getpid (), "Error while loading the targa image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_TARGA_LOAD;
           error->description = strdup ("Error while loading the targa image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a TARGA image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_targa
 * @see imel_image_new_from_targa_handle
 */
ImelImage *imel_image_new_from_targa_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_TARGA, image_memory, 0);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_targa_memory: "
                       "error: %s\n", getpid (), "Error while loading the targa image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_TARGA_LOAD;
           error->description = strdup ("Error while loading the targa image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JP2 image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jp2_handle
 * @see imel_image_new_from_jp2_memory
 */
ImelImage *imel_image_new_from_jp2 (const char *filename, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_JP2, filename, JP2_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jp2: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the jp2 image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JP2_LOAD;
           error->description = strdup ("Error while loading the jp2 image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JP2 image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jp2
 * @see imel_image_new_from_jp2_memory
 */
ImelImage *imel_image_new_from_jp2_handle (FILE *file, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_JP2, &io, (fi_handle) file, JP2_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jp2_handle: "
                       "error: %s\n", getpid (), "Error while loading the jp2 image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JP2_LOAD;
           error->description = strdup ("Error while loading the jp2 image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a JP2 image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new_from_jp2
 * @see imel_image_new_from_jp2_handle
 */
ImelImage *imel_image_new_from_jp2_memory (uint8_t *memory, uint32_t length, long int level, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_JP2, image_memory, JP2_DEFAULT);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_jp2_memory: "
                       "error: %s\n", getpid (), "Error while loading the jp2 image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_JP2_LOAD;
           error->description = strdup ("Error while loading the jp2 image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a ICO image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelIcoLoadFlags
 * @see imel_image_new_from_ico_handle
 * @see imel_image_new_from_ico_memory
 */
ImelImage *imel_image_new_from_ico (const char *filename, long int level, ImelIcoLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_ICO, filename, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ico: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the ico image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_ICO_LOAD;
           error->description = strdup ("Error while loading the ico image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a ICO image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelIcoLoadFlags
 * @see imel_image_new_from_ico
 * @see imel_image_new_from_ico_memory
 */
ImelImage *imel_image_new_from_ico_handle (FILE *file, long int level, ImelIcoLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_ICO, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ico_handle: "
                       "error: %s\n", getpid (), "Error while loading the ico image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_ICO_LOAD;
           error->description = strdup ("Error while loading the ico image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a ICO image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelIcoLoadFlags
 * @see imel_image_new_from_ico
 * @see imel_image_new_from_ico_handle
 */
ImelImage *imel_image_new_from_ico_memory (uint8_t *memory, uint32_t length, long int level, 
                                           ImelIcoLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_ICO, image_memory, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_ico_memory: "
                       "error: %s\n", getpid (), "Error while loading the ico image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_ICO_LOAD;
           error->description = strdup ("Error while loading the ico image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCD image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPcdLoadFlags
 * @see imel_image_new_from_pcd_handle
 * @see imel_image_new_from_pcd_memory
 */
ImelImage *imel_image_new_from_pcd (const char *filename, long int level, ImelPcdLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_PCD, filename, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcd: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the pcd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCD_LOAD;
           error->description = strdup ("Error while loading the pcd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCD image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPcdLoadFlags
 * @see imel_image_new_from_pcd
 * @see imel_image_new_from_pcd_memory
 */
ImelImage *imel_image_new_from_pcd_handle (FILE *file, long int level, ImelPcdLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_PCD, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcd_handle: "
                       "error: %s\n", getpid (), "Error while loading the pcd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCD_LOAD;
           error->description = strdup ("Error while loading the pcd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a PCD image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelPcdLoadFlags
 * @see imel_image_new_from_pcd
 * @see imel_image_new_from_pcd_handle
 */
ImelImage *imel_image_new_from_pcd_memory (uint8_t *memory, uint32_t length, long int level, 
                                            ImelPcdLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_PCD, image_memory, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_pcd_memory: "
                       "error: %s\n", getpid (), "Error while loading the pcd image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_PCD_LOAD;
           error->description = strdup ("Error while loading the pcd image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a GIF image from a file name.
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelGifLoadFlags
 * @see imel_image_new_from_gif_handle
 * @see imel_image_new_from_gif_memory
 */
ImelImage *imel_image_new_from_gif (const char *filename, long int level, ImelGifLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 bitmap = FreeImage_Load (FIF_GIF, filename, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_gif: %s: "
                       "error: %s\n", getpid (), filename, 
                       "Error while loading the gif image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_GIF_LOAD;
           error->description = strdup ("Error while loading the gif image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a GIF image from an already open file.
 * 
 * @param file Initialized FILE type.
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelGifLoadFlags
 * @see imel_image_new_from_gif
 * @see imel_image_new_from_gif_memory
 */
ImelImage *imel_image_new_from_gif_handle (FILE *file, long int level, ImelGifLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 ImelImage *l_image;
 FreeImageIO io = { (FI_ReadProc) fread, NULL, (FI_SeekProc) fseek, (FI_TellProc) ftell };

 return_var_if_fail (file, NULL);
 
 bitmap = FreeImage_LoadFromHandle (FIF_GIF, &io, (fi_handle) file, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_gif_handle: "
                       "error: %s\n", getpid (), "Error while loading the gif image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_GIF_LOAD;
           error->description = strdup ("Error while loading the gif image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load a GIF image from an one loaded in memory.
 * 
 * @param memory Content of the image
 * @param length @p memory length
 * @param level Image level
 * @param load_flags Load options
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see ImelGifLoadFlags
 * @see imel_image_new_from_gif
 * @see imel_image_new_from_gif_handle
 */
ImelImage *imel_image_new_from_gif_memory (uint8_t *memory, uint32_t length, long int level, 
                                            ImelGifLoadFlags load_flags, ImelError *error)
{
 FIBITMAP *bitmap;
 FIMEMORY *image_memory;
 ImelImage *l_image;

 return_var_if_fail (memory, NULL);
 
 image_memory = FreeImage_OpenMemory (memory, length);
 bitmap = FreeImage_LoadFromMemory (FIF_GIF, image_memory, load_flags);
 if ( !bitmap ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_image_new_from_gif_memory: "
                       "error: %s\n", getpid (), "Error while loading the gif image");
#endif

      if ( error ) {
           error->code = IMEL_ERR_GIF_LOAD;
           error->description = strdup ("Error while loading the gif image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}

/**
 * @brief Load an image identify by it's extension
 * 
 * @param filename Name of the image with extension.
 * @param level Image level
 * @param error Error variable if you want handle the errors or NULL.
 * @return Image loaded in #ImelImage type on success or NULL on error.
 * 
 * @see imel_image_new
 */
ImelImage *imel_image_new_from (const char *filename, long int level, ImelError *error)
{
 FREE_IMAGE_FORMAT fif;
 FIBITMAP *bitmap;
 ImelImage *l_image;
 
 return_var_if_fail (filename, NULL);
 
 fif = FreeImage_GetFIFFromFilename (filename);
 
 bitmap = FreeImage_Load (fif, filename, 0);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_new_from", filename, "error", "Error while loading the image");

      if ( error ) {
           error->code = IMEL_ERR_LOAD;
           error->description = strdup ("Error while loading the image");
      }
      
      FreeImage_Unload (bitmap);
      return NULL;
 }
 
 l_image = imel_image_new_from_core (bitmap, level);
 FreeImage_Unload (bitmap);
 
 return l_image;
}
