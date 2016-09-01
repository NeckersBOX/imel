/*
 * "image_enum.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#ifndef IMEL_IMAGE_ENUM_H
#define IMEL_IMAGE_ENUM_H
/**
 * @file image_enum.h
 * @author Davide Francesco Merico
 * @brief This file contains enums with options when open and save image file.
 */
 
/**
 * Options when opens JPEG images.
 * 
 * @see imel_image_new_from_jpeg
 * @see imel_image_new_from_jpeg_handle
 * @see imel_image_new_from_jpeg_memory
 */
typedef enum _imel_jpeg_load_flags { 
          IMEL_JPEG_DEFAULT = 0, /**< Equal to IMEL_JPEG_FAST **/
          IMEL_JPEG_FAST,        /**< Loads the JPEG image fast but with less quality */
          IMEL_JPEG_ACCURATE,    /**< Loads the JPEG image with quality but with less velocity */
          /**
           *  Loads the JPEG image with separed CMYK channels. 
           *  @warning Imel convert CMYK channels to RGB channels automatically 
           */ 
          IMEL_JPEG_CMYK         
        } ImelJpegLoadFlags;

/**
 * Options when opens PNG images.
 * 
 * @see imel_image_new_from_png
 * @see imel_image_new_from_png_handle
 * @see imel_image_new_from_png_memory
 */        
typedef enum _imel_png_load_flags { 
          IMEL_PNG_DEFAULT = 0, /**< Loads the PNG image normally */
          IMEL_PNG_IGNOREGAMMA  /**< Loads the PNG image withour gamma correction */
        } ImelPngLoadFlags;

/**
 * Options when opens TIFF images.
 * 
 * @see imel_image_new_from_tiff
 * @see imel_image_new_from_tiff_handle
 * @see imel_image_new_from_tiff_memory
 */
typedef enum _imel_tiff_load_flags {
          IMEL_TIFF_DEFAULT = 0, /**< Loads the TIFF image normally */
          /**
           *  Loads the TIFF image with separed CMYK channels. 
           *  @warning Imel convert CMYK channels to RGB channels automatically 
           */ 
          IMEL_TIFF_CMYK
        } ImelTiffLoadFlags;

/**
 * Options when opens ICO images.
 * 
 * @see imel_image_new_from_ico
 * @see imel_image_new_from_ico_handle
 * @see imel_image_new_from_ico_memory
 */
typedef enum _imel_ico_load_flags {
          IMEL_ICO_DEFAULT = 0, /**< Loads the ICO image normally */
          /**
           * Loads the ICO image converting it to 32 bit and make
           * an alpha channel from an AND mask while loading.
           */ 
          IMEL_ICO_MAKEALPHA
        } ImelIcoLoadFlags;

/**
 * Options when opens PCD images
 * 
 * @see imel_image_new_from_pcd
 * @see imel_image_new_from_pcd_handle
 * @see imel_image_new_from_pcd_memory
 */
typedef enum _imel_pcd_load_flags { 
          IMEL_PCD_DEFAULT = 0, /**< Equal to IMEL_PCD_BASE */
          IMEL_PCD_BASE,        /**< Loads the PCD image at resolution of 768x512 pixel */
          IMEL_PCD_BASEDIV4,    /**< Loads the PCD image at resolution of 384x256 pixel */
          IMEL_PCD_BASEDIV16    /**< Loads the PCD image at resolution of 192x128 pixel */
        } ImelPcdLoadFlags;

/**
 * Options when opens GIF images
 * 
 * @see imel_image_new_from_gif
 * @see imel_image_new_from_gif_handle
 * @see imel_image_new_from_gif_memory
 */
typedef enum _imel_gif_load_flags {
          IMEL_GIF_DEFAULT = 0, /**< Loads the GIF image normally */
          IMEL_GIF_LOAD256      /**< Loads the GIF image with only 256 colors */
        } ImelGifLoadFlags;

/** 
 * Options when saves TIFF images
 * 
 * @see imel_image_save_tiff
 * @see imel_image_save_tiff_handle
 */
typedef enum _imel_tiff_flags {
             IMEL_TIFF_PACKBITS      = 0x0100, /**< Saves the TIFF image using PACKBITS compression */
             IMEL_TIFF_DEFLATE       = 0x0200, /**< Saves the TIFF image using DEFLATE compression ( Zlib )*/
             IMEL_TIFF_ADOBE_DEFLATE = 0x0400, /**< Saves the TIFF image using ADOBE DEFLATE compression */
             IMEL_TIFF_NONE          = 0x0800, /**< Saves the TIFF image using no compression */
             IMEL_TIFF_CCITTFAX3     = 0x1000, /**< Saves the TIFF image using CCITT Group 3 fax codify */
             IMEL_TIFF_CCITTFAX4     = 0x2000, /**< Saves the TIFF image using CCITT Group 3 fax codify */
             IMEL_TIFF_LZW           = 0x4000  /**< Saves the TIFF image using LZW compression */
} ImelTiffFlags;

/**
 * Options when saves PNG images.
 * 
 * @see imel_image_save_png
 * @see imel_image_save_png_handle
 */
typedef enum _imel_png_flags {
             IMEL_PNG_Z_BEST_SPEED          = 0x0001, /**< Saves the PNG image using Zlib library with compression value of 1 */
             IMEL_PNG_Z_DEFAULT_COMPRESSION = 0x0006, /**< Saves the PNG image using Zlib library with compression value of 6 */
             IMEL_PNG_Z_BEST_COMPRESSION    = 0x0009, /**< Saves the PNG image using Zlib library with compression value of 9 */
             IMEL_PNG_Z_NO_COMPRESSION      = 0x0100, /**< Saves the PNG image using Zlib library with no compression */
             IMEL_PNG_INTERLACED            = 0x0200  /**< Saves the PNG image with Adam7 interlacing */
} ImelPngFlags;

/**
 * Options when saves BMP images.
 * 
 * @see imel_image_save_bmp
 * @see imel_image_save_bmp_handle
 */
typedef enum _imel_bmp_bits {
             IMEL_BMP_BITS_24 = 24, /**< Saves the BMP image with no alpha */
             IMEL_BMP_BITS_32 = 32  /**< Saves the BMP image with alpha */
} ImelBmpBits;

/**
 * Options when saves J2K/JP2 images
 * 
 * @see imel_image_save_j2k
 * @see imel_image_save_j2k_handle
 * @see imel_image_save_jp2
 * @see imel_image_save_jp2_handle
 */
typedef enum _imel_j2k_bits {
             IMEL_J2K_BITS_24 = 24, /**< Saves the J2K/JP2 image with no alpha */
             IMEL_J2K_BITS_32 = 32  /**< Saves the J2K/JP2 image with alpha */
} ImelJ2kBits;

#endif
