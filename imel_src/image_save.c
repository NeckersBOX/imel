/*
 * "image_save.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <FreeImage.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "header.h"

/**
 * @file image_save.c
 * @author Davide Francesco Merico
 * @brief This file contains function to save in different image format.
 */
 
#define __SAVE_N  0
#define __SAVE_H  1

#ifndef DOXYGEN_IGNORE_DOC

extern ImelColor *imel_color_get_from_pixel (ImelPixel pixel);
extern uint32_t imel_pixel_get_rgba (ImelPixel pxl);

#endif

static bool imel_image_save_core (ImelImage *image, FREE_IMAGE_FORMAT format, int bpp,
                                  int flags, uint8_t save_mode, ImelError *error, ...)
{
 FIBITMAP *bitmap;
 ImelSize x, y;
 RGBQUAD dst_pixel;
 BYTE dst_byte;
 uint32_t rgba;
 va_list list;
 FreeImageIO io = { NULL, (FI_WriteProc) fwrite, (FI_SeekProc) fseek, (FI_TellProc) ftell };
 
 return_var_if_fail (image, false);
 
 bitmap = FreeImage_Allocate (image->width, image->height, bpp, 0, 0, 0);
 if ( !bitmap ) {
      imel_printf_debug ("imel_image_save_core", NULL, "warning", "Unknown Error");

      if ( error ) {
           error->code = -1;
           error->description = strdup ("Unknown Error");
      }

      return false;
 }
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             rgba = imel_pixel_get_rgba (image->pixel[y][x]);
             
             if ( bpp >= 16 ) {
                  dst_pixel.rgbRed   = (BYTE) RGBA_R_MASK (rgba);
                  dst_pixel.rgbGreen = (BYTE) RGBA_G_MASK (rgba);
                  dst_pixel.rgbBlue  = (BYTE) RGBA_B_MASK (rgba);
                  if ( bpp == 32 )
                       dst_pixel.rgbReserved = 255 - (BYTE) RGBA_A_MASK (rgba);
                  
                  FreeImage_SetPixelColor (bitmap, x, image->height - (y + 1), &dst_pixel);
                  continue;
             }
             
             dst_byte = (BYTE) (((RGBA_R_MASK (rgba) + RGBA_G_MASK (rgba) + RGBA_B_MASK (rgba)) / 3) > 127);             
             FreeImage_SetPixelIndex (bitmap, x, image->height - (y + 1), &dst_byte);
       }
 }
 
 va_start (list, error);
 switch ( save_mode ) {
     case __SAVE_N:
        if ( !FreeImage_Save (format, bitmap, va_arg (list, const char *), flags) ) {
             FreeImage_Unload (bitmap);
             return false;
        }
        break;
     case __SAVE_H:
        if ( !FreeImage_SaveToHandle (format, bitmap, &io, va_arg (list, fi_handle), flags) ) {
             FreeImage_Unload (bitmap);
             return false;
        }
        break;
 }
 va_end (list);
 FreeImage_Unload (bitmap);
 
 return true;
}

/**
 * @brief Save image in PPM format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_ppm_handle
 */
bool imel_image_save_ppm (ImelImage *image, const char *filename, ImelError *error)
{
 return imel_image_save_core (image, FIF_PPM, 24, PNM_SAVE_ASCII, __SAVE_N, error, filename);
}

/**
 * @brief Save image in PPM format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_ppm
 */
bool imel_image_save_ppm_handle (ImelImage *image, FILE *of, ImelError *error)
{
 return imel_image_save_core (image, FIF_PPM, 24, PNM_SAVE_ASCII, __SAVE_H, error, of);
}

/**
 * @brief Save image in raw PPM format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_ppmraw_handle
 */
bool imel_image_save_ppmraw (ImelImage *image, const char *filename, ImelError *error)
{
 return imel_image_save_core (image, FIF_PPM, 24, PNM_SAVE_RAW, __SAVE_N, error, filename);
}

/**
 * @brief Save image in raw PPM format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_ppmraw
 */
bool imel_image_save_ppmraw_handle (ImelImage *image, FILE *of, ImelError *error)
{
 return imel_image_save_core (image, FIF_PPM, 24, PNM_SAVE_RAW, __SAVE_H, error, of);
}

/**
 * @brief Save image in JPEG format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param quality Save quality. Values between 0 and 100.
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_jpeg_handle
 */
bool imel_image_save_jpeg (ImelImage *image, const char *filename, int quality, ImelError *error)
{
 return imel_image_save_core (image, FIF_JPEG, 24, (quality < 0) ? 75 : 
                              (quality > 100) ? 100 : quality, __SAVE_N, error, filename);
}

/**
 * @brief Save image in JPEG format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param quality Save quality. Values between 0 and 100.
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_jpeg
 */
bool imel_image_save_jpeg_handle (ImelImage *image, FILE *of, int quality, ImelError *error)
{
 return imel_image_save_core (image, FIF_JPEG, 24, (quality < 0) ? 75 : 
                              (quality > 100) ? 100 : quality, __SAVE_H, error, of);
}

/**
 * @brief Save image in PNG format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param png_flags Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelPngFlags
 * @see imel_image_save_png_handle
 */
bool imel_image_save_png (ImelImage *image, const char *filename, 
                          ImelPngFlags png_flags, ImelError *error)
{
 return imel_image_save_core (image, FIF_PNG, 32, png_flags, __SAVE_N, error, filename);
}

/**
 * @brief Save image in PNG format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param png_flags Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelPngFlags
 * @see imel_image_save_png
 */
bool imel_image_save_png_handle (ImelImage *image, FILE *of, ImelPngFlags png_flags, ImelError *error)
{
 return imel_image_save_core (image, FIF_PNG, 32, png_flags, __SAVE_H, error, of);
}

/**
 * @brief Save image in TIFF format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param tiff_flags Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelTiffFlags
 * @see imel_image_save_tiff_handle
 */
bool imel_image_save_tiff (ImelImage *image, const char *filename,
                           ImelTiffFlags tiff_flags, ImelError *error)
{
 return imel_image_save_core (image, FIF_TIFF, 32, tiff_flags, __SAVE_N, error, filename);
}

/**
 * @brief Save image in TIFF format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param tiff_flags Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelTiffFlags
 * @see imel_image_save_tiff
 */
bool imel_image_save_tiff_handle (ImelImage *image, FILE *of, ImelTiffFlags tiff_flags, ImelError *error)
{
 return imel_image_save_core (image, FIF_TIFF, 32, tiff_flags, __SAVE_H, error, of);
}

/**
 * @brief Save image in WBMP format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_wbmp_handle
 */
bool imel_image_save_wbmp (ImelImage *image, const char *filename, ImelError *error)
{
 return imel_image_save_core (image, FIF_WBMP, 1, 0, __SAVE_N, error, filename);
}

/**
 * @brief Save image in WBMP format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_wbmp
 */
bool imel_image_save_wbmp_handle (ImelImage *image, FILE *of, ImelError *error)
{
 return imel_image_save_core (image, FIF_WBMP, 1, 0, __SAVE_H, error, of);
}

/**
 * @brief Save image in BMP format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelBmpBits
 * @see imel_image_save_bmp_handle
 */
bool imel_image_save_bmp (ImelImage *image, const char *filename, 
                          ImelBmpBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_BMP, bits_per_pixel, 0, __SAVE_N, error, filename);
}

/**
 * @brief Save image in BMP format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelBmpBits
 * @see imel_image_save_bmp
 */
bool imel_image_save_bmp_handle (ImelImage *image, FILE *of, ImelBmpBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_BMP, bits_per_pixel, 0, __SAVE_H, error, of);
}

/**
 * @brief Save image in J2K format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelJ2kBits
 * @see imel_image_save_j2k_handle
 */
bool imel_image_save_j2k (ImelImage *image, const char *filename,
                          ImelJ2kBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_J2K, bits_per_pixel, 0, __SAVE_N, error, filename);
}

/**
 * @brief Save image in J2K format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelJ2kBits
 * @see imel_image_save_j2k
 */
bool imel_image_save_j2k_handle (ImelImage *image, FILE *of, ImelJ2kBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_J2K, bits_per_pixel, 0, __SAVE_H, error, of);
}

/**
 * @brief Save image in JP2 format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelJ2kBits
 * @see imel_image_save_jp2_handle
 */
bool imel_image_save_jp2 (ImelImage *image, const char *filename,
                          ImelJ2kBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_JP2, bits_per_pixel, 0, __SAVE_N, error, filename);
}

/**
 * @brief Save image in JP2 format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param bits_per_pixel Save options
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see ImelJ2kBits
 * @see imel_image_save_jp2
 */
bool imel_image_save_jp2_handle (ImelImage *image, FILE *of, ImelJ2kBits bits_per_pixel, ImelError *error)
{
 return imel_image_save_core (image, FIF_JP2, bits_per_pixel, 0, __SAVE_H, error, of);
}

/**
 * @brief Save image in XPM format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_xpm_handle
 */
bool imel_image_save_xpm (ImelImage *image, const char *filename, ImelError *error)
{
 return imel_image_save_core (image, FIF_XPM, 24, 0, __SAVE_N, error, filename);
}

/**
 * @brief Save image in XPM format in an already open file
 * 
 * @param image Image to save
 * @param of Output FILE
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 * 
 * @see imel_image_save_xpm
 */
bool imel_image_save_xpm_handle (ImelImage *image, FILE *of, ImelError *error)
{
 return imel_image_save_core (image, FIF_XPM, 24, 0, __SAVE_H, error, of);
}

/**
 * @brief Save image in IMEL format
 * 
 * @param image Image to save
 * @param filename Output file name
 * @param error Error variable if you want handle the errors or NULL.
 * @return TRUE on success or FALSE on error.
 */
bool imel_image_save_imel (ImelImage *image, const char *filename, ImelError *error)
{
 FILE *of;
 ImelSize x, y;
 const char sign[16] = "\x00\x01Imel\xff\xffSign\x00\x00\x00\x00";

 return_var_if_fail (image && filename, false);

 if ( !(of = fopen(filename, "wb")) ) {
      imel_printf_debug ("imel_image_save_imel", filename, "warning", strerror (errno));

      if ( error ) {
           error->code = errno;
           error->description = strdup (strerror (errno));
      }

      return false;
 }

 fwrite (sign, sizeof (char), 16, of);
 fwrite (&(image->width), sizeof (ImelSize), 1, of);
 fwrite (&(image->height), sizeof (ImelSize), 1, of);
 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             fwrite (&(image->pixel[y][x]), sizeof (ImelPixel), 1, of);
 fclose (of);

 return true;
}
