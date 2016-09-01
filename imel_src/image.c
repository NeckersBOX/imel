/*
 * "image.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#define _GNU_SOURCE
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "header.h"
/**
 * @file image.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to elaborate images
 */

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) < (b)) ? (b) : (a))

#ifndef DOXYGEN_IGNORE_DOC

extern ImelColor        imel_color_subtract               (ImelColor, ImelColor);
extern ImelColor        imel_color_sum                    (ImelColor, ImelColor);

extern void             imel_effect_antialias             (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_antique               (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_brightness            (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_contrast              (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_contrast_stretching   (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_direct_antialias      (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_image_add             (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_image_subtract        (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_invert                (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_normalize             (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_rasterize             (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_white_black           (ImelImagePtr, ImelGenericPtr);
extern void             imel_effect_color_to_alpha        (ImelImagePtr, ImelGenericPtr);


extern bool             imel_pixel_compare                (ImelPixel, ImelPixel, ImelSize);
extern bool             imel_pixel_compare_level          (ImelLevel, ImelLevel, ImelSize);
extern void             imel_pixel_copy                   (ImelPixel *, ImelPixel);
extern ImelPixel        imel_pixel_new                    (ImelColor, ImelColor, ImelColor, ImelLevel);
extern void             imel_pixel_set                    (ImelPixel *, ImelColor, ImelColor, ImelColor, ImelLevel);
extern ImelPixel        imel_pixel_union                  (ImelPixel, ImelPixel, unsigned char);
extern void             imel_pixel_set_from_pixel         (ImelPixel *, ImelPixel);

extern ImelSize       **__imel_draw_get_line_points_array (ImelSize, ImelSize, ImelSize, ImelSize, long int *, long int *);
extern void             __imel_draw_put_pixel             (ImelImage *, ImelSize, ImelSize, ImelPixel);
extern void             __imel_pattern_image_insert       (ImelImage *, ImelImage *, ImelSize, ImelSize, ImelPatternOperation);
extern bool             check_size                        (ImelImage *, ImelSize, bool);
extern void             imel_draw_line                    (ImelImage *, ImelSize, ImelSize, ImelSize, ImelSize, ImelPixel);
extern void             imel_draw_point                   (ImelImage *, ImelSize, ImelSize, ImelPixel);
extern void             imel_draw_rect                    (ImelImage *, ImelSize, ImelSize, ImelSize, ImelSize, ImelPixel, bool);
                           
extern void             imel_font_write_string            (ImelImage *, ImelSize, ImelSize, const char *, ImelSize, ImelPixel);

extern ImelInfoCut     *imel_info_cut_get_min             (ImelInfoCut *, ImelOrientation);
extern ImelSize         imel_info_cut_get_split           (ImelImage *, ImelInfoCut *, ImelOrientation);
extern ImelInfoCut     *imel_info_cut_get_next            (ImelImage *, ImelInfoCut *, ImelSize);

#endif
          
static void _imel_image_fill_with_color (ImelImage *, ImelPoint *, ImelPixel, ImelSize);

/* if 'ImelRandom' equal to 1 the function srand () cannot be called */
bool ImelRandom = 0;

/**
 * @brief Make a new image
 * 
 * This function make a new image with black background and level set to -255.
 * 
 * @param width Image width
 * @param height Image height
 * @return A new ImelImage or NULL on error
 * 
 * @see imel_image_new_with_background_color
 */
ImelImage *imel_image_new (ImelSize width, ImelSize height)
{
 ImelImage *l_image;
 ImelSize x, y;
 
 return_var_if_fail (width && height, NULL);
 
 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 return_var_if_fail (l_image, NULL);
 
 l_image->width = width;
 l_image->height = height;

 l_image->pixel = (ImelPixel **) malloc (height * sizeof (ImelPixel *)
                                         + __memory_buffer);
                                         
 if ( !l_image->pixel ) {
	  free (l_image);
	  return NULL;
 }
 
 for ( y = 0; y < height; y++ ) {
       (l_image->pixel)[y] = (ImelPixel *) calloc (width + __memory_buffer,
                                                   sizeof (ImelPixel));
       
       if ( !(l_image->pixel)[y] ) {
		    for ( y--; y > -1; free ((l_image->pixel)[y]) );
		    free (l_image->pixel);
		    free (l_image);
		    
		    return NULL;
	   }
	   
       for ( x = 0; x < width; x++ )
            (l_image->pixel)[y][x].level = -255;
 }

 return l_image;
}

/**
 * @brief Make a new image with background color and level specified
 * 
 * This function make a new image which each pixel is set to @p pixel passed.
 * 
 * @param width Image width
 * @param height Image height
 * @param pixel Color and level of the image
 * @return a new ImelImage
 * 
 * @see imel_image_new
 */
ImelImage *imel_image_new_with_background_color (ImelSize width, ImelSize height, ImelPixel pixel)
{
 ImelImage *l_image;
 ImelSize y, x;

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = width;
 l_image->height = height;

 l_image->pixel = (ImelPixel **) malloc (height * sizeof (ImelPixel *)
                                         + __memory_buffer);
 for ( y = 0; y < height; y++ ) {
       l_image->pixel[y] = (ImelPixel *) malloc (width * sizeof (ImelPixel)
                                                 + __memory_buffer);

       for ( x = 0; x < width; x++ )
             imel_pixel_set_from_pixel (&(l_image->pixel[y][x]), pixel);
 }

 return l_image;
}

/**
 * @brief Duplicate an image
 * 
 * This function copy @p image passed in a new one.
 * 
 * @param image Image to copy
 * @return A new ImelImage equal to @p image
 */
ImelImage *imel_image_copy (ImelImage *image)
{
 ImelImage *l_image;
 ImelSize x, y;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->width;
 l_image->height = image->height;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *)
                                         + __memory_buffer);
 for ( y = 0; y < image->height; y++ ) {
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel)
                                                 + __memory_buffer);
       for ( x = 0; x < image->width; x++ )
             imel_pixel_set (&(l_image->pixel[y][x]),
                             image->pixel[y][x].red,
                             image->pixel[y][x].green,
                             image->pixel[y][x].blue,
                             image->pixel[y][x].level);
 }

 return l_image;
}

/**
 * @brief Get image width
 * 
 * This function get @p image width.
 * 
 * @param image Image from which get the width
 * @return @p image width
 * @note Same as <tt>image->width</tt>
 */
ImelSize imel_image_get_width (ImelImage *image)
{
 return_var_if_fail (image, 0);

 return image->width;
}

/**
 * @brief Get image height
 * 
 * This function get @p image height.
 * 
 * @param image Image from which get the height
 * @return @p image height
 * @note Same as <tt>image->height</tt>
 */
ImelSize imel_image_get_height (ImelImage *image)
{
 return_var_if_fail (image, 0);

 return image->height;
}

/**
 * @brief Free an image
 * 
 * This function free memory allocated by @p image.
 * 
 * @param image Image to free
 */
void imel_image_free (ImelImage *image)
{
 ImelSize y;

 return_if_fail (image);

 if ( image->pixel ) {
      for ( y = 0; y < image->height; y++ ) {
            if ( !image->pixel[y] )
                 continue;
            free (image->pixel[y]);
       }
       free (image->pixel);
 }

 free (image);
}

/**
 * @brief Apply an effect to an image
 * 
 * This function apply the @p effect to the @p image.
 * 
 * @param image Image on which apply the @p effect
 * @param effect Effect to apply to the @p image
 * @param ... Options for the @p effect
 * @see ImelEffect
 */
void imel_image_apply_effect (ImelImage *image, ImelEffect effect, ...)
{
 ImelGenericPtr argument;
 va_list opt_argument;
 static ImelGenericFuncPtr effect_func[] = { imel_effect_white_black,
                                             imel_effect_antique,
                                             imel_effect_invert,
                                             imel_effect_normalize,
                                             imel_effect_brightness,
                                             imel_effect_contrast_stretching,
                                             imel_effect_contrast,
                                             imel_effect_rasterize,
                                             imel_effect_antialias,
                                             imel_effect_direct_antialias,
                                             imel_effect_image_add,
                                             imel_effect_image_subtract,
                                             imel_effect_color_to_alpha,
                                             NULL
                                           };

 return_if_fail (image);

 va_start (opt_argument, effect);
 argument = va_arg (opt_argument, void *);
 va_end (opt_argument);

 effect_func[effect] (image, argument);
}

/**
 * @brief Apply a filter to an image
 * 
 * This function set to 255 the channel, or the channels, specified
 * as @p mask.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.jpg", 0, NULL);
 * 
 * imel_image_apply_filter (image, IMEL_MASK_RED | IMEL_MASK_BLUE);
 * @endcode
 * 
 * @param image Image on which apply the filter
 * @param mask Channel, or channels, to set to 255.
 * @see ImelMask
 * @see imel_image_remove_base_color
 */
void imel_image_apply_filter (ImelImage *image, ImelMask mask)
{
 ImelSize y, x;
 ImelPixel *p;

 return_if_fail (image);

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;
             switch ( mask ) {
              case IMEL_MASK_RED:
                     imel_pixel_set (p, 255, p->green, p->blue, p->level);
                     break;
              case IMEL_MASK_GREEN:
                     imel_pixel_set (p, p->red, 255, p->blue, p->level);
                     break;
              case IMEL_MASK_BLUE:
                     imel_pixel_set (p, p->red, p->green, 255, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN:
                     imel_pixel_set (p, 255, 255, p->blue, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 255, p->red, p->blue, 0);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 255, 255, p->blue, 0);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE:
                     imel_pixel_set (p, 255, p->green, 255, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 255, p->green, 255, 0);
                     break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                     imel_pixel_set (p, p->red, 255, 255, p->level);
                     break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, p->red, 255, 255, 0);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                     imel_pixel_set (p, 255, 255, 255, p->level);
                     break;
              default: break;
             }
       }
 }
}

/**
 * @brief Apply a color to an image
 * 
 * This function apply a chosen color to the @p image. It can be set as
 * unique color or as reference to existed color in @p image.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.jpg", 0, NULL);
 * 
 * imel_image_apply_color (image, 0xfd, 0xb4, 0x55, false);
 * ...
 * imel_image_apply_color (image, 0xfd, 0xb4, ox55, true);
 * @endcode
 * @image html images/orig.jpg   "Original Image"
 * @image html images/imac-f.jpg "Passed FALSE as last argument"
 * @image html images/imac-t.jpg "Passed TRUE as last argument"
 * @image latex images/orig.eps   "Original Image"
 * @image latex images/imac-f.eps "Passed FALSE as last argument"
 * @image latex images/imac-t.eps "Passed TRUE as last argument"
 * @param image Image on which apply the color
 * @param red Red channel of the color to apply
 * @param green Green channel of the color to apply
 * @param blue Blue channel of the color to apply
 * @param mono TRUE if the color chosen is the only color of the image, else FALSE
 * @see imel_image_apply_color_from_string
 */
void imel_image_apply_color (ImelImage *image, ImelColor red, ImelColor green, ImelColor blue, bool mono)
{
 ImelPixel *p;
 ImelSize x, y;

 return_if_fail (image);

 if ( mono )
      imel_image_apply_effect (image, IMEL_EFFECT_WHITE_BLACK);

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;
             imel_pixel_set (p, ( p->red * red ) / 255, ( p->green * green ) / 255,
                             ( p->blue * blue ) / 255, p->level);
        }
 }
}

/**
 * @brief Remove a color
 * 
 * This function set to 0 the channel, or the channels, specified
 * 
 * This function set to 255 the channel, or the channels, specified
 * as @p mask.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.jpg", 0, NULL);
 * 
 * imel_image_remove_base_color (image, IMEL_MASK_RED | IMEL_MASK_BLUE);
 * @endcode
 * 
 * @param image Image on which remove the color
 * @param mask Channel, or channels, to set to 0.
 * @see ImelMask
 * @see imel_image_apply_filter
 */
void imel_image_remove_base_color (ImelImage *image, ImelMask mask)
{
 ImelSize y, x;
 ImelPixel *p;

 return_if_fail (image);

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;
             switch ( mask ) {
              case IMEL_MASK_RED:
                     imel_pixel_set (p, 0, p->green, p->blue, p->level);
                     break;
              case IMEL_MASK_GREEN:
                     imel_pixel_set (p, p->red, 0, p->blue, p->level);
                     break;
              case IMEL_MASK_BLUE:
                     imel_pixel_set (p, p->red, p->green, 0, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN:
                     imel_pixel_set (p, 0, 0, p->blue, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 0, p->green, p->blue, -255);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 0, 0, p->blue, -255);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE:
                     imel_pixel_set (p, 0, p->green, 0, p->level);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, 0, p->green, 0, -255);
                     break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                     imel_pixel_set (p, p->red, 0, 0, p->level);
                     break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                     imel_pixel_set (p, p->red, 0, 0, -255);
                     break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                     imel_pixel_set (p, 0, 0, 0, p->level);
                     break;
              default: break;
             }
       }
 }
}

/**
 * @brief Apply a color from a string to an image
 * 
 * This function apply a chosen color from a string to the @p image. 
 * It can be set as unique color or as reference to existed color in @p image.
 * 
 * @param image Image on which apply the color
 * @param string Color to apply to the image in HTML format ( '\#rrggbb' )
 * @param mono TRUE if the color chosen is the only color of the image, else FALSE
 * @see imel_image_apply_color
 */
void imel_image_apply_color_from_string (ImelImage *image, const char *string, bool mono)
{
 ImelPixel *p;
 ImelSize x, y;
 ImelColor red, green, blue;
 int i;
 char buff[2];

 return_if_fail (image);

 if ( mono )
      imel_image_apply_effect (image, IMEL_EFFECT_WHITE_BLACK);

 if ( *string != '#' ) {
      imel_printf_debug ("imel_image_apply_color_from_string", NULL, "warning", 
                        "string not begin with '#'"); 
      return;
 }

 if ( strlen (string) != 7 ) {
      imel_printf_debug ("imel_image_apply_color_from_string", NULL, "warning", 
                        "string length not valid"); 
      return;
 }

 for ( i = 1; i < 7; i++ ) {
       if ( !isxdigit (string[i]) ) {
            imel_printf_debug ("imel_image_apply_color_from_string", NULL, "warning", 
                               "string not valid"); 
            return;
       }
 }

 for ( i = 1; i < 7; i += 2 ) {
       buff[0] = string[i];
       buff[1] = string[i+1];
            if ( i < 3 ) red = strtol (buff, NULL, 16);
       else if ( i < 6 ) green = strtol (buff, NULL, 16);
       else              blue = strtol (buff, NULL, 16);
 }

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;
             imel_pixel_set (p, ( p->red * red ) / 255, ( p->green * green ) / 255,
                             ( p->blue * blue ) / 255, p->level);
        }
 }
}

/**
 * @brief Replace a color with an other one.
 * 
 * This function replace the pixel @p src with @p desc thorugh #imel_draw_point () in
 * @p image. The research of @p src in @p image will be done with a @p tollerance
 * calculated in this function with #imel_pixel_compare ().
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.jpg", 0, NULL);
 * ImelPixel find = { 0xff, 0x66, 0x00, 0 }, replace = { 0xff, 0x00, 0x00, 0 };
 * 
 * imel_image_replace_color (image, find, replace, 16);
 * @endcode
 * 
 * @param image Image where replace @p src with @p dest
 * @param src Pixel to replace with @p dest
 * @param dest New pixel for @p src occourrences.
 * @param tollerance Tollerance for @p src. Values between 0 and 255.
 * @see imel_draw_point
 * @see imel_pixel_compare
 * @see imel_immage_replace_area_color 
 */
void imel_image_replace_color (ImelImage *image, ImelPixel src, ImelPixel dest, ImelSize tollerance)
{
 ImelSize x, y;

 return_if_fail (image);

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             if ( imel_pixel_compare (image->pixel[y][x], src, tollerance) )
                  imel_draw_point (image, x, y, dest);
}

/**
 * @brief Replace a color with an other one in a specified area.
 * 
 * This function replace the pixel @p src with @p desc though #imel_draw_point () in
 * an area from coordinate \f$(\_x_1,\_y_1)\f$ to coordinate \f$(\_x_2,\_y_2)\f$ inside 
 * the @p image. 
 * The research of @p src in @p image will be done with a @p tollerance calculated in 
 * this function with #imel_pixel_compare ().
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.jpg", 0, NULL);
 * ImelPixel find = { 0xff, 0x66, 0x00, 0 }, replace = { 0xff, 0x00, 0x00, 0 };
 * ImelSize start[2], end[2];
 * 
 * start[0] = image->width  / 2;
 * start[1] = image->height / 2;
 *   end[0] = image->width  - start[0];
 *   end[1] = image->height - start[1];
 * 
 * imel_image_replace_area_color (image, find, replace, 16, start[0], start[1], end[0], end[1]);
 * @endcode
 * @image html images/pattern.jpg "Original Image"
 * @image html images/replace_area_color.jpg "Example Output"
 * @image latex images/pattern.eps "Original Image"
 * @image latex images/replace_area_color.eps "Example Output"
 * @param image Image where replace @p src with @p dest
 * @param src Pixel to replace with @p dest
 * @param dest New pixel for @p src occourrences.
 * @param tollerance Tollerance for @p src. Values between 0 and 255.
 * @param _x1 Start x coordinate
 * @param _y1 Start y coordinate
 * @param _x2 End x coordinate
 * @param _y2 End y coordinate
 * @see imel_draw_point
 * @see imel_pixel_compare
 * @see imel_image_replace_color
 */
void imel_image_replace_area_color (ImelImage *image, ImelPixel src, ImelPixel dest, ImelSize tollerance,
                                    ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2)
{
 ImelSize x, y, sx, sy, ex, ey;

 return_if_fail (image);
 sx = (_x1 < _x2) ? _x1 : _x2;
 ex = (_x2 > _x1) ? _x2 : _x1;
 sy = (_y1 < _y2) ? _y1 : _y2;
 ey = (_y2 > _y1) ? _y2 : _y1;

 if ( sx > image->width || ex > image->width ) {
      imel_printf_debug ("imel_image_replace_area_color", NULL, "warning", 
                         "the width of the replace area color > width of image");

      return;
 }

 if ( sy > image->height || ey > image->height ) {
      imel_printf_debug ("imel_image_replace_area_color", NULL, "warning", 
                         "the height of the replace area color > height of image");

      return;
 }

 for ( y = sy; y < ey; y++ )
       for ( x = sx; x < ex; x++ )
             if ( imel_pixel_compare (image->pixel[y][x], src, tollerance) )
                  imel_draw_point (image, x, y, dest);
}

/**
 * @brief Resize an image
 * 
 * This function resize the @p image to @p width and @p height specified.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.tiff", 0, NULL);
 * ImelImage *half;
 * 
 * half = imel_image_resize (image, image->width / 2, image->height / 2);
 * imel_image_free (image);
 * @endcode
 *  
 * @param image Image to resize
 * @param width New width
 * @param height New height
 * @return A copy of @p image resized.
 */
ImelImage *imel_image_resize (ImelImage *image, ImelSize width, ImelSize height)
{
 ImelImage *l_image;
 ImelSize w, h, t[2];

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = width;
 l_image->height = height;
 l_image->pixel = (ImelPixel **) malloc (height * sizeof (ImelPixel *)
                                         + __memory_buffer);
 for ( h = 0; h < height; h++ )
       l_image->pixel[h] = (ImelPixel *) malloc (width * sizeof (ImelPixel)
                                                 + __memory_buffer);

 for ( h = 0; h < height; h++ ) {
       t[0] = (image->height * h) / height;
       for ( w = 0; w < width; w++ ) {
             t[1] = (image->width * w) / width;
             l_image->pixel[h][w].level = image->pixel[t[0]][t[1]].level;
             imel_pixel_copy (&(l_image->pixel[h][w]), image->pixel[t[0]][t[1]]);
       }
 }

 return l_image;
}

/**
 * @brief Rotate an image to left
 * 
 * This function rotate @p image passed to left.
 * 
 * @param image Image to rotate
 * @return A copy of @p image rotated to left
 * @see imel_image_rotate
 * @see imel_image_rotate_to_right
 * @see imel_image_rotate_complete
 */
ImelImage *imel_image_rotate_to_left (ImelImage *image)
{
 ImelSize x, y;
 ImelImage *l_image;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->height;
 l_image->height = image->width;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));
 for ( y = 0; y < l_image->height; y++ )
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_copy (&(l_image->pixel[image->width - ( 1 + x )][y]), image->pixel[y][x]);

 return l_image;
}

/**
 * @brief Rotate an image to right
 * 
 * This function rotate @p image passed to right.
 * 
 * @param image Image to rotate
 * @return A copy of @p image rotated to right
 * @see imel_image_rotate
 * @see imel_image_rotate_to_left
 * @see imel_image_rotate_complete
 */
ImelImage *imel_image_rotate_to_right (ImelImage *image)
{
 ImelSize x, y;
 ImelImage *l_image;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->height;
 l_image->height = image->width;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));
 for ( y = 0; y < l_image->height; y++ )
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_copy (&(l_image->pixel[x][l_image->width - ( 1 + y )]), image->pixel[y][x]);

 return l_image;
}

/**
 * @brief Rotate an image to 180 degrees
 * 
 * This function rotate @p image passed to 180 degrees.
 * 
 * @param image Image to rotate
 * @return A copy of @p image rotated to 180 degrees. 
 * @see imel_image_rotate
 * @see imel_image_rotate_to_left
 * @see imel_image_rotate_to_right
 */
ImelImage *imel_image_rotate_complete (ImelImage *image)
{
 ImelSize x, y;
 ImelImage *l_image;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->width;
 l_image->height = image->height;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));
 for ( y = 0; y < l_image->height; y++ )
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_copy (&(l_image->pixel[l_image->height - ( 1 + y )][l_image->width - ( 1 + x )]), image->pixel[y][x]);

 return l_image;
}

/**
 * @brief Mirror an image to horizontal
 * 
 * This function applies the mirror horizontal effect to @p image.
 * 
 * @param image Image to mirror
 * @return A copy of @p image mirrored horizontally
 * @see imel_image_mirror_vertical
 */ 
ImelImage *imel_image_mirror_horizontal (ImelImage *image)
{
 ImelSize x, y;
 ImelImage *l_image;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->width;
 l_image->height = image->height;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));
 for ( y = 0; y < l_image->height; y++ )
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_set_from_pixel (&(l_image->pixel[y][l_image->width - ( 1 + x )]), image->pixel[y][x]);

 return l_image;
}

/**
 * @brief Mirror an image to vertical
 * 
 * This function applies the mirror vertical effect to @p image.
 * 
 * @param image Image to mirror
 * @return A copy of @p image mirrored vertically
 * @see imel_image_mirror_horizontal
 */ 
ImelImage *imel_image_mirror_vertical (ImelImage *image)
{
 ImelSize x, y;
 ImelImage *l_image;

 return_var_if_fail (image, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = image->width;
 l_image->height = image->height;

 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));
 for ( y = 0; y < l_image->height; y++ )
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));

 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_set_from_pixel (&(l_image->pixel[l_image->height - ( 1 + y )][x]), image->pixel[y][x]);

 return l_image;
}

/**
 * @brief Rotate an image to a chosen angle.
 * 
 * This function rotate the @p image to @p rotate_rad radians.
 * 
 * @param image Image to rotate
 * @param rotate_rad Rotation value in radians
 * @return A copy of @p image rotated
 * @see imel_image_rotate_to_left
 * @see imel_image_rotate_to_right
 * @see imel_image_rotate_complete
 */
ImelImage *imel_image_rotate (ImelImage *image, double rotate_rad)
{
 ImelImage *l_image;
 ImelSize y, x, sx, sy;
 int32_t rx, ry;
 int32_t max[2] = { 0, 0 }, min[2];
 
 return_var_if_fail (image, NULL);
 
 min[0] = image->width;
 min[1] = image->height;
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             rx = (int32_t) (((double) x) * cos (rotate_rad) - ((double) y) * sin (rotate_rad));
             ry = (int32_t) (((double) x) * sin (rotate_rad) + ((double) y) * cos (rotate_rad));
             
             if ( rx < min[0] ) 
                  min[0] = rx;
             if ( rx > max[0] )
                  max[0] = rx;
                  
             if ( ry < min[1] ) 
                  min[1] = ry;
             if ( ry > max[1] )
                  max[1] = ry;
       }
 }
 
 l_image = imel_image_new ((max[0] - min[0]) + 1, (max[1] - min[1]) + 1);
 sx = (ImelSize) abs (min[0]);
 sy = (ImelSize) abs (min[1]);
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             rx = (int32_t) (((double) x) * cos (rotate_rad) - ((double) y) * sin (rotate_rad));
             ry = (int32_t) (((double) x) * sin (rotate_rad) + ((double) y) * cos (rotate_rad));
             
             imel_pixel_set_from_pixel (&(l_image->pixel[((ImelSize) ry) + sy][((ImelSize) rx) + sx]), image->pixel[y][x]);
       }
 }
 
 return l_image;
}

/**
 * @brief Makes an horizontal or vertical perspective
 * 
 * This function makes a perspective horizontal or vertical with a certain angle.
 * 
 * @code
 * ImelImage *src = imel_image_new_from ("butterfly.jpg", 0, NULL);
 * ImelImage **p;
 * 
 * p = imel_image_perspective (src, 0.785398, IMEL_ORIENTATION_HORIZONTAL);
 * ...
 * p = imel_image_perspective (src, -0.785398, IMEL_ORIENTATION_HORIZONTAL);
 * ...
 * p = imel_image_perspective (src, 0.785398, IMEL_ORIENTATION_VERTICAL);
 * ...
 * p = imel_image_perspective (src, -0.785398, IMEL_ORIENTATION_VERTICAL);
 * @endcode
 * @image html images/perspective.jpg "Example"
 * @image latex images/perspective.eps "Example"
 * @param image Original image
 * @param rad_angle Perspective angle in radians.
 * @param orientation Perspective type.
 * @return A new image with the effect applied.
 * @see ImelOrientation 
 * @see imel_image_slant
 */
ImelImage *imel_image_perspective (ImelImage *image, double rad_angle, ImelOrientation orientation)
{
 ImelImage *l_image = NULL;
 ImelSize x, y, j, k;
 bool dir;
 double sr;
 
 return_var_if_fail (image, NULL);
 
 if ( !(dir = rad_angle > 0.f) ) 
      rad_angle *= -1.f;
      
 while ( rad_angle > 1.5708 )
         rad_angle -= 1.5708;
 sr = sin (rad_angle);
 
 l_image = imel_image_new (image->width, image->height);
      
 if ( orientation == IMEL_ORIENTATION_HORIZONTAL ) {   
      if ( dir ) {
           for ( x = 0; x < image->width; x++ ) {
                 j = (ImelSize) (((double) ((x * image->height) / image->width)) * sr);
                 k = image->height - j;
        
                 for ( y = 0; y < image->height; y++ )
                       imel_draw_point (l_image, x, ((k * y) / image->height) + ( j >> 1 ),
                                        image->pixel[y][x]);
           }
      }
      else {
           for ( x = 0; x < image->width; x++ ) {
                 j = (ImelSize) (((double) (((image->width - (x + 1)) * image->height) / image->width)) * sr);
                 k = image->height - j;
        
                 for ( y = 0; y < image->height; y++ )
                       imel_draw_point (l_image, x, ((k * y) / image->height) + ( j >> 1 ),
                                        image->pixel[y][x]);
           }
      }
      
      return l_image;
 }
 
 if ( dir ) {
      for ( y = 0; y < image->height; y++ ) {
            j = (ImelSize) (((double) (y * image->width) / image->height) * sr);
            k = image->width - j;
       
            for ( x = 0; x < image->width; x++ )
                  imel_draw_point (l_image, ((k * x) / image->width) + ( j >> 1 ), y,
                                   image->pixel[y][x]);
      }
 }
 else {
      for ( y = 0; y < image->height; y++ ) {
            j = (ImelSize) (((double) (((image->height - (y + 1)) * image->width) / image->height)) * sr);
            k = image->width - j;
       
            for ( x = 0; x < image->width; x++ )
                  imel_draw_point (l_image, ((k * x) / image->width) + ( j >> 1 ), y,
                                   image->pixel[y][x]);
      }
 }
 
 return l_image;
}

/**
 * @brief Cut an image
 * 
 * This function cuts @p image from the coordinate @p sx, @p sy to the coordinate @p ex, @p ey.
 * 
 * @param image Image to cut
 * @param sx Start x coordinate
 * @param sy Start y coordinate
 * @param ex End x coordinate
 * @param ey End y coordinate
 * @return A new image with the @p image cutted
 * @see imel_image_auto_cut
 * @see imel_image_cut_grid
 */
ImelImage *imel_image_cut (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey)
{
 ImelImage *l_image = NULL;
 ImelSize x, y;

 return_var_if_fail (image, NULL);

 if ( !check_size (image, sx, true) || !check_size (image, ex, true) ) {
      imel_printf_debug ("imel_image_cut", NULL, "warning",
                         "the width of the cutted area > width of image");
      return l_image;
 }

 if ( !check_size (image, sy, false) || !check_size (image, ey, false) ) {
      imel_printf_debug ("imel_image_cut", NULL, "warning",
                         "the height of the cutted area > height of image");
      return l_image;
 }

 if ( sx > ex || sy > ey ) {
      imel_printf_debug ("imel_image_cut", NULL, "warning",
                         "the start point of the cutted area > end point");      
      return l_image;
 }

 l_image = imel_image_new (ex - sx, ey - sy);

 for ( y = sy; y < ey; y++ )
       for ( x = sx; x < ex; x++ )
             imel_pixel_set (&(l_image->pixel[y - sy][x - sx]),
                             image->pixel[y][x].red, image->pixel[y][x].green,
                             image->pixel[y][x].blue, image->pixel[y][x].level);

 return l_image;
}

void __imel_image_cut_row (ImelImage *image, ImelImage ***images, ImelSize *n_images, 
                           ImelInfoCut *cut_info, ImelSize p1, ImelSize p2)
{
 ImelInfoCut *prev, *next;
 
 next = imel_info_cut_get_min (cut_info, IMEL_ORIENTATION_VERTICAL);
 prev = next;
 
 (*images)[--(*n_images)] = imel_image_cut (image, 0, p1, 
                                            prev ? prev->position : image->width, p2);
 
 while ( next = imel_info_cut_get_next (image, cut_info, next->index) ) {
		 (*images)[--(*n_images)] = imel_image_cut (image, 
		                                            prev->position, p1,
		                                            next->position, p2);
		 
		 prev = next;
 }
 
 if ( prev ) 
      (*images)[--(*n_images)] = imel_image_cut (image, 
                                                 prev->position, p1,
                                                 image->width, p2);	  
}

/**
 * @brief Cut an image in more images
 * 
 * This function cut @p image in more images through a guide lines
 * passed as @p cut_info.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("cut_orig.jpg", 0, NULL);
 * ImelImage **tiles;
 * ImelInfoCut *cut_info;
 * int j;
 * 
 * cut_info = imel_info_cut_new (IMEL_ORIENTATION_HORIZONTAL, 35);
 * cut_info = imel_info_cut_add (cut_info, IMEL_ORIENTATION_HORIZONTAL, image->height - 35);
 * cut_info = imel_info_cut_add (cut_info, IMEL_ORIENTATION_VERTICAL, 35);
 * cut_info = imel_info_cut_add (cut_info, IMEL_ORIENTATION_VERTICAL, image->width - 35);
 *
 * tiles = imel_image_cut_grid (image, cut_info);
 * imel_info_cut_free (cut_info);
 * imel_image_free (image);
 * ...
 * for ( j = 0; tiles[j]; imel_image_free (tiles[j]), j++ );
 * free (tiles);
 * @endcode
 * @image html images/cut_orig.jpg "Original Image"
 * @image html images/cut_result.jpg "Result Images"
 * @image latex images/cut_orig.eps "Original Image"
 * @image latex images/cut_result.eps "Result Images"
 * 
 * @param image Image to cut
 * @param cut_info guide lines
 * @return Cutted images in order inverse ( column, row ) inside a
 * NULL-terminated LIFO array.
 * 
 * @see imel_image_cut
 * @see imel_image_auto_cut
 * @see imel_info_cut_new
 * @see imel_info_cut_add
 * @see imel_info_cut_free
 * @see ImelOrientation
 */
ImelImage **imel_image_cut_grid (ImelImage *image, ImelInfoCut *cut_info)
{
 ImelImage **images;
 ImelSize n_images;
 ImelInfoCut *prev, *next;
 
 return_var_if_fail (cut_info && image, NULL);
 
 n_images = (imel_info_cut_get_split (image, cut_info, IMEL_ORIENTATION_HORIZONTAL) + 1)
          * (imel_info_cut_get_split (image, cut_info, IMEL_ORIENTATION_VERTICAL) + 1);
 
 images = (ImelImage **) malloc ((n_images + 1) * sizeof (ImelImage *));
 images[n_images] = NULL;
 
 next = imel_info_cut_get_min (cut_info, IMEL_ORIENTATION_HORIZONTAL);
 prev = next;
 
 __imel_image_cut_row (image, &images, &n_images, cut_info, 
                       0, prev ? prev->position : image->height);
 
 while ( next = imel_info_cut_get_next (image, cut_info, next->index) ) {
		 __imel_image_cut_row (image, &images, &n_images, cut_info,
		                       prev->position, next->position);
		 
		 prev = next;
 }
 
 if ( prev ) 
	  __imel_image_cut_row (image, &images, &n_images, cut_info,
	                        prev->position, image->height);
 
 return images;        
}

/**
 * @brief Cut automatically an image
 * 
 * This function cuts automatically an @p image removing from the sides all the aereas
 * with a specified color or level.
 * 
 * @code
 * ImelImage *image, *cut_image;
 * ImelPixel cut_pixel = { 16, 16, 16, -255 };
 *
 * image = imel_image_new_from ("image.bmp", 0, NULL);
 *
 * // Auto cut transparency
 * cut_image = imel_image_auto_cut (image, 254, IMEL_REF_LEVEL, cut_pixel.level);
 * imel_image_free (image);
 *       
 * // Auto cut color ( {0, 0, 0} - {32, 32, 32} )
 * image = imel_image_auto_cut (cut_image, 16, IMEL_REF_COLOR, cut_pixel);
 * imel_image_free (cut_image);
 * ...
 * imel_image_free (image);
 * @endcode
 * @image html images/imel_cutme.jpg "Original Image"
 * @image html images/auto_cut.jpg "Image Result"
 * @image latex images/imel_cutme.eps "Original Image"
 * @image latex images/auto_cut.eps "Image Result"
 * 
 * @param image Image to cut
 * @param tollerance Tollerance for level or color to remove.
 * @param reference Which type of cut do
 * @param ... Level ( ImelSize ) or color ( ImelPixel ) to remove from the sides.
 * @return Cutted image.
 * 
 * @see imel_image_cut
 * @see imel_image_cut_grid
 * @see ImelRef
 */
ImelImage *imel_image_auto_cut (ImelImage *image, ImelSize tollerance, ImelRef reference, ...)
{
 ImelPixel pxl;
 ImelLevel level;
 va_list opt_argument;
 ImelSize sx, sy, ex = 0, ey = 0, x, y;
 
 return_var_if_fail (image, NULL);
 
 va_start (opt_argument, reference);
 if ( reference )
      level = va_arg (opt_argument, ImelLevel);
 else pxl = va_arg (opt_argument, ImelPixel);
 va_end (opt_argument);
 
 sx = image->width;
 sy = image->height;
 
 for ( x = 0; x < image->width; x++ ) {
       for ( y = 0; y < image->height; y++ ) {
             if ( ((reference && !imel_pixel_compare_level (level, image->pixel[y][x].level, tollerance))
               || (!reference && !imel_pixel_compare (pxl, image->pixel[y][x], tollerance))) && sx > x )
                  sx = x;       
            
             if ( ((reference && !imel_pixel_compare_level (level, image->pixel[y][x].level, tollerance))
               || (!reference && !imel_pixel_compare (pxl, image->pixel[y][x], tollerance))) && ex < x ) 
                  ex = x;
       }      
 }
                  
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             if ( ((reference && !imel_pixel_compare_level (level, image->pixel[y][x].level, tollerance))
               || (!reference && !imel_pixel_compare (pxl, image->pixel[y][x], tollerance))) && sy > y )
                  sy = y;       
            
             if ( ((reference && !imel_pixel_compare_level (level, image->pixel[y][x].level, tollerance))
               || (!reference && !imel_pixel_compare (pxl, image->pixel[y][x], tollerance))) && ey < y ) 
                  ey = y;
       }      
 } 
 
 if ( sx > ++ex && sy > ++ey )
      return imel_image_cut (image, ex, ey, sx, sy);
 
 if ( sy > ey )
      return imel_image_cut (image, sx, ey, ex, sy);
      
 if ( sx > ex )
      return imel_image_cut (image, ex, sy, sx, ey);
 
 if ( ey != image->height )
      ey++;
      
 return imel_image_cut (image, sx, sy, ex, ey);
}

/**
 * @brief Insert an image in another one
 * 
 * This function insert the image @p src in the image @p dest from position
 * \f$(sx,sy)\f$.
 * 
 * @param dest Destination image
 * @param src Image to insert in @p dest
 * @param sx Start x coordinate
 * @param sy Start y coordinate
 * 
 * @note This function uses indirectly #imel_pixel_copy () function to insert @p src in @p dest
 * @see imel_pixel_copy
 * @see imel_image_union
 */
void imel_image_insert_image (ImelImage *dest, ImelImage *src, ImelSize sx, ImelSize sy)
{
 ImelSize y, x;

 return_if_fail (dest && src);

 for ( y = sy; y < dest->height && y < (src->height + sy); y++ )
       for ( x = sx; x < dest->width && x < (src->width + sx); x++ )
             imel_draw_point (dest, x, y, src->pixel[y-sy][x-sx]);
}

/**
 * @brief Apply a logic operation between two images.
 * 
 * This function apply the @p logic_operation to @p img1 and @p img2.
 * 
 * @param img1 First image
 * @param img2 Second image
 * @param logic_operation Type of operation
 * @return An image result from the operation between @p img1 and @p img2
 * 
 * @see ImelLogicOperation
 */
ImelImage *imel_image_apply_logic_operation (ImelImage *img1, ImelImage *img2, ImelLogicOperation logic_operation)
{
 ImelImage *l_image;
 ImelSize x, y;
 ImelPixel pxl;

 return_var_if_fail (img1 && img2, NULL);

 l_image = (ImelImage *) malloc (sizeof (ImelImage));
 l_image->width = img1->width;
 l_image->height = img1->height;
 l_image->pixel = (ImelPixel **) malloc (l_image->height * sizeof (ImelPixel *));

 for ( y = 0; y < l_image->height; y++ ) {
       l_image->pixel[y] = (ImelPixel *) malloc (l_image->width * sizeof (ImelPixel));
       for ( x = 0; x < l_image->width; x++ ) {
             if ( x >= img2->width || y >= img2->height ) {
                  switch ( logic_operation ) {
                     case IMEL_LOGIC_AND:
                            pxl.red = pxl.green = pxl.blue = 0;
                            pxl.level = img1->pixel[y][x].level;
                            break;
                     case IMEL_LOGIC_OR:
                     case IMEL_LOGIC_XOR:
                            break;
                  }
             }
             else {
                  switch ( logic_operation ) {
                     case IMEL_LOGIC_AND:
                            pxl.red = img1->pixel[y][x].red & img2->pixel[y][x].red;
                            pxl.green = img1->pixel[y][x].green & img2->pixel[y][x].green;
                            pxl.blue = img1->pixel[y][x].blue & img2->pixel[y][x].blue;
                            pxl.level = min (img1->pixel[y][x].level, img2->pixel[y][x].level);
                            break;
                     case IMEL_LOGIC_OR:
                            pxl.red = img1->pixel[y][x].red | img2->pixel[y][x].red;
                            pxl.green = img1->pixel[y][x].green | img2->pixel[y][x].green;
                            pxl.blue = img1->pixel[y][x].blue | img2->pixel[y][x].blue;
                            pxl.level = min (img1->pixel[y][x].level, img2->pixel[y][x].level);
                            break;
                     case IMEL_LOGIC_XOR:
                            pxl.red = img1->pixel[y][x].red ^ img2->pixel[y][x].red;
                            pxl.green = img1->pixel[y][x].green ^ img2->pixel[y][x].green;
                            pxl.blue = img1->pixel[y][x].blue ^ img2->pixel[y][x].blue;
                            pxl.level = min (img1->pixel[y][x].level, img2->pixel[y][x].level);
                            break;
                  }
             }
             imel_pixel_set (&(l_image->pixel[y][x]), pxl.red, pxl.green, pxl.blue, pxl.level);
       }
 }

 return l_image;
}

/**
 * @brief Get histogram values from an image
 * 
 * This function get the histogram values from @p image for @p histogram_type chosen.
 * 
 * @param image Image from which get the values
 * @param histogram_type Types of values to get
 * @return An array with 256 element
 * 
 * @see ImelHistogram
 * @see imel_image_get_histogram_image
 * @see imel_image_get_histograms_image
 */
int *imel_image_get_histogram (ImelImage *image, ImelHistogram histogram_type)
{
 int *histogram = NULL;
 ImelSize x, y;

 return_var_if_fail (image, NULL);

 histogram = (int *) calloc (256, sizeof (int));

 for ( x = 0; x < image->width; x++ ) {
       for ( y = 0; y < image->height; y++ ) {
             switch (histogram_type) {
                case IMEL_HISTOGRAM_RED:
                       histogram[image->pixel[y][x].red]++;
                       break;
                case IMEL_HISTOGRAM_GREEN:
                       histogram[image->pixel[y][x].green]++;
                       break;
                case IMEL_HISTOGRAM_BLUE:
                       histogram[image->pixel[y][x].blue]++;
                       break;
                case IMEL_HISTOGRAM_COMPLETE:
                       histogram[image->pixel[y][x].red]++;
                       histogram[image->pixel[y][x].green]++;
                       histogram[image->pixel[y][x].blue]++;
                       break;
             }
       }
 }

 return histogram;
}

/**
 * @brief Make an histogram image for an image chosen
 * 
 * This function make an histogram for @p image from which are already
 * elaborated the values through #imel_image_get_histogram () function.
 * 
 * @image html images/histogram_me.jpg "Original Image"
 * @image html images/histogram.jpg "Output Image"
 * @image latex images/histogram_me.eps "Original Image"
 * @image latex images/histogram.eps "Output Image"
 * 
 * @param image Image from which get the histogram
 * @param __histogram Values returned from imel_image_get_histogram () function
 * @param histogram_type Histogram type.
 * @return Histogram image
 * 
 * @see ImelHistogram
 * @see imel_image_get_histogram
 * @see imel_image_get_histograms_image
 */
ImelImage *imel_image_get_histogram_image (ImelImage *image, int *__histogram, ImelHistogram histogram_type)
{
 int *histogram = __histogram ? __histogram : imel_image_get_histogram (image, histogram_type);
 ImelSize i = 0, max;
 ImelImage *l_image;
 ImelPixel pxl = {250, 250, 250, 0};
 char *string;

 return_var_if_fail (image, NULL);

 l_image = imel_image_new_with_background_color (263, 152, pxl);

 imel_pixel_set (&pxl, 10, 10, 10, 1);
 imel_draw_rect (l_image, 2, 2, 261, 18, pxl, false);
 switch (histogram_type) {
   case IMEL_HISTOGRAM_RED:
          string = strdup ("Histogram Type: RED");
          break;
   case IMEL_HISTOGRAM_GREEN:
          string = strdup ("Histogram Type: GREEN");
          break;
   case IMEL_HISTOGRAM_BLUE:
          string = strdup ("Histogram Type: BLUE");
          break;
   case IMEL_HISTOGRAM_COMPLETE:
          string = strdup ("Histogram Type: RGB");
          break;
 }
 imel_font_write_string (l_image, 4, 4, string, IMEL_FONT_SIZE_MEDIUM, pxl);
 free (string);

 imel_draw_rect (l_image, 2, 20, 261, 138, pxl, false);
 imel_font_write_string (l_image, 3, 127, "0", IMEL_FONT_SIZE_MEDIUM, pxl);
 imel_font_write_string (l_image, 240, 127, "255", IMEL_FONT_SIZE_MEDIUM, pxl);

 for ( max = 0; i < 255; i++ ) {
       if ( ((ImelSize) histogram[i]) > max )
            max = (ImelSize) histogram[i];
 }

 for ( i = 256; i--; )
         imel_draw_line (l_image, 259 - (255 - i), 125, 258 - (254 - i),
                                  125 - ((histogram[i] * 100) / max), pxl);

 imel_draw_rect (l_image, 3, 140, 260, 150, pxl, false);
 for ( i = 0; i < 256; i++ ) {
       switch (histogram_type) {
         case IMEL_HISTOGRAM_RED:
                pxl = imel_pixel_new (i, 0, 0, 1);
                break;
         case IMEL_HISTOGRAM_GREEN:
                pxl = imel_pixel_new (0, i, 0, 1);
                break;
         case IMEL_HISTOGRAM_BLUE:
                pxl = imel_pixel_new (0, 0, i, 1);
                break;
         case IMEL_HISTOGRAM_COMPLETE:
                pxl = imel_pixel_new (i, i, i, 1);
                break;
       }
       imel_draw_line (l_image, 4 + i, 141, 4 + i, 150, pxl);
 }

 return l_image;
}

/**
 * @brief Make all types of histogram of an image
 * 
 * This function make all types of histogram of @p image with a chosen @p layout.
 * 
 * @image html images/histogram_panels.jpg
 * @image latex images/histogram_panels.eps
 * 
 * @param image Image from which get histograms
 * @param layout Layout of the histograms
 * @return The image with all the histograms
 * 
 * @see ImelHistogramLayout
 * @see imel_image_get_histogram
 * @see imel_image_get_histogram_image
 */
ImelImage *imel_image_get_histograms_image (ImelImage *image, ImelHistogramLayout layout)
{
 ImelImage *l_image = NULL, *histogram[4];

 return_var_if_fail (image, NULL);

 histogram[0] = imel_image_get_histogram_image (image, NULL, IMEL_HISTOGRAM_RED);
 histogram[1] = imel_image_get_histogram_image (image, NULL, IMEL_HISTOGRAM_GREEN);
 histogram[2] = imel_image_get_histogram_image (image, NULL, IMEL_HISTOGRAM_BLUE);
 histogram[3] = imel_image_get_histogram_image (image, NULL, IMEL_HISTOGRAM_COMPLETE);

 switch ( layout ) {
   case IMEL_HISTOGRAM_LAYOUT_VERTICAL:
          l_image = imel_image_new (263, 608);

          imel_image_insert_image (l_image, histogram[0], 0, 0);
          imel_image_insert_image (l_image, histogram[1], 0, 152);
          imel_image_insert_image (l_image, histogram[2], 0, 304);
          imel_image_insert_image (l_image, histogram[3], 0, 456);
          break;
   case IMEL_HISTOGRAM_LAYOUT_HORIZONTAL:
          l_image = imel_image_new (1052, 152);

          imel_image_insert_image (l_image, histogram[0], 0, 0);
          imel_image_insert_image (l_image, histogram[1], 263, 0);
          imel_image_insert_image (l_image, histogram[2], 526, 0);
          imel_image_insert_image (l_image, histogram[3], 789, 0);
          break;
   case IMEL_HISTOGRAM_LAYOUT_PANELS:
          l_image = imel_image_new (526, 304);

          imel_image_insert_image (l_image, histogram[0], 0, 0);
          imel_image_insert_image (l_image, histogram[1], 263, 0);
          imel_image_insert_image (l_image, histogram[2], 0, 152);
          imel_image_insert_image (l_image, histogram[3], 263, 152);
          break;
 }

 imel_image_free (histogram[0]);
 imel_image_free (histogram[1]);
 imel_image_free (histogram[2]);
 imel_image_free (histogram[3]);

 return l_image;
}

/**
 * @brief Change level of an image
 * 
 * This function change the level of @p image. It can be added or setted through
 * the @p level_operation argument.
 * 
 * @param image Image to which change the level
 * @param level_operation Type of operation
 * @param level New level or value for @p level_operation
 * 
 * @see ImelLevelOperation
 * @see imel_image_change_color_level
 */
void imel_image_change_level (ImelImage *image, ImelLevelOperation level_operation,
                              ImelLevel level)
{
 ImelSize x, y = -1;

 return_if_fail (image);

 while ( ++y < image->height ) {
         for ( x = 0; x < image->width; x++ ) {
               if ( level_operation == IMEL_LEVEL_OPERATION_SET )
                    image->pixel[y][x].level = level;
               else image->pixel[y][x].level += level;
         }
 }
}

/**
 * @brief Change level to a specified color
 * 
 * This function change the level value for each @p image pixel that have a color
 * equal or similar to @p color_pxl.
 * 
 * @param image Image to elaborate
 * @param level_operation Type of operation
 * @param level New level or value for @p level_operation
 * @param color_pxl Color to change the level
 * @param tollerance Tollerance to find @p color 
 * 
 * @see ImelLevelOperation
 * @see imel_pixel_compare
 * @see imel_image_change_level
 */
void imel_image_change_color_level (ImelImage *image, ImelLevelOperation level_operation,
                                    ImelLevel level, ImelPixel color_pxl, ImelColor tollerance)
{
 ImelSize x, y = -1;

 return_if_fail (image);

 while ( ++y < image->height ) {
         for ( x = 0; x < image->width; x++ ) {
               if ( imel_pixel_compare (image->pixel[y][x], color_pxl, tollerance) ) {
                    if ( level_operation == IMEL_LEVEL_OPERATION_SET )
                         image->pixel[y][x].level = level;
                    else image->pixel[y][x].level += level;
               }
         }
 }
}

/**
 * @brief Apply a convolution matrix to an image
 * 
 * This function apply a convolution matrix of chosen size to @p image.
 * 
 * @param image Image to apply the @p filter
 * @param filter Convolution matrix
 * @param width Width of matrix
 * @param height Height of matrix
 * @param factor Multiply factor 
 * @param bias Offset to apply to matrix
 * 
 * @see https://en.wikipedia.org/wiki/Kernel_(image_processing)
 */
void imel_image_apply_convolution (ImelImage *image, double **filter, int width,
                                   int height, double factor, double bias)
{
 ImelSize x, y;
 int imgx, imgy, j, k;
 double rgb[3];

 return_if_fail (image && filter);

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
               memset (rgb, 0, sizeof (double) * 3);
               
               for ( j = 0; j < width; j++ ) {
                     for ( k = 0; k < height; k++ ) {
                           imgx = (x - width / 2 + j + image->width) % image->width;
                           imgy = (y - height / 2 + k + image->height) % image->height;

                           rgb[0] += ((double) image->pixel[imgy][imgx].red) * filter[j][k];
                           rgb[1] += ((double) image->pixel[imgy][imgx].green) * filter[j][k];
                           rgb[2] += ((double) image->pixel[imgy][imgx].blue) * filter[j][k];
                     }
                }

                image->pixel[y][x].red = min (abs ((int) (factor * rgb[0] + bias)), 255);
                image->pixel[y][x].green = min (abs ((int) (factor * rgb[1] + bias)), 255);
                image->pixel[y][x].blue = min (abs ((int) (factor * rgb[2] + bias)), 255);
       }
 }
}

/**
 * @brief Apply a pattern to an imaage
 * 
 * This function apply an image @p pattern to @p image with a chosen @p operation.
 * 
 * @code
 * ImelImage *pattern = imel_image_new_from ("pattern.png", 0, NULL);
 * ImelImage *image = imel_image_new (150, 151);
 * 
 * imel_image_apply_pattern (image, pattern, IMEL_PATTERN_OPERATION_INSERT);
 * imel_image_free (pattern);
 * @endcode
 * @image html images/stripe.jpg "Pattern image"
 * @image html images/pattern.jpg "Output image"
 * @image latex images/stripe.eps "Pattern image"
 * @image latex images/pattern.eps "Output image"
 * 
 * @param image Image to apply @p pattern
 * @param pattern Pattern image
 * @param operation Type of operation
 * 
 * @see ImelPatternOperation
 */
void imel_image_apply_pattern (ImelImage *image, ImelImage *pattern, ImelPatternOperation operation)
{
 ImelSize x, y;

 return_if_fail (image && pattern);
 
 for ( y = 0; y < image->height; y += pattern->height )
       for ( x = 0; x < image->width; x += pattern->width )
             __imel_pattern_image_insert (image, pattern, x, y, operation);
}

void __imel_pattern_image_insert (ImelImage *dest, ImelImage *src, ImelSize sx, ImelSize sy, ImelPatternOperation operation)
{
 ImelSize y, x;
 ImelPixel pxl;
 
 return_if_fail (dest && src);
 
 switch ( operation ) {
    case IMEL_PATTERN_OPERATION_INSERT: 
          for ( y = sy; y < dest->height && y < (src->height + sy); y++ )
               for ( x = sx; x < dest->width && x < (src->width + sx); x++ )
                     imel_draw_point (dest, x, y, src->pixel[y-sy][x-sx]);
          break;
    case IMEL_PATTERN_OPERATION_SUM:
          for ( y = sy; y < dest->height && y < (src->height + sy); y++ ) {
               for ( x = sx; x < dest->width && x < (src->width + sx); x++ ) {
                     dest->pixel[y][x].red   = imel_color_sum (dest->pixel[y][x].red, src->pixel[y - sy][x - sx].red);
                     dest->pixel[y][x].green = imel_color_sum (dest->pixel[y][x].green, src->pixel[y - sy][x - sx].green);
                     dest->pixel[y][x].blue  = imel_color_sum (dest->pixel[y][x].blue, src->pixel[y - sy][x - sx].blue);
                     dest->pixel[y][x].level += src->pixel[y - sy][x - sx].level;
               }
          }
          break;
    case IMEL_PATTERN_OPERATION_SUBTRACT:
          for ( y = sy; y < dest->height && y < (src->height + sy); y++ ) {
               for ( x = sx; x < dest->width && x < (src->width + sx); x++ ) {
                     dest->pixel[y][x].red   = imel_color_subtract (dest->pixel[y][x].red, src->pixel[y - sy][x - sx].red);
                     dest->pixel[y][x].green = imel_color_subtract (dest->pixel[y][x].green, src->pixel[y - sy][x - sx].green);
                     dest->pixel[y][x].blue  = imel_color_subtract (dest->pixel[y][x].blue, src->pixel[y - sy][x - sx].blue);
                     dest->pixel[y][x].level -= src->pixel[y - sy][x - sx].level;
               }
          }
          break;
 }
}

/**
 * @brief Insert an image in another with a chosen opacity
 * 
 * This function insert @p img2 over @p img1 with a chosen @p opacity at position
 * specified by @p alignment.
 * 
 * @param img1 Base image
 * @param img2 Image to insert over @p img1
 * @param opacity Opacity of @p img2. Values between 0 and 255.
 * @param alignment Alignment of @p img2 in @p img1
 * @return Result image
 * 
 * @see imel_pixel_union
 * @see ImelAlignment
 */
ImelImage *imel_image_union (ImelImage *img1, ImelImage *img2, unsigned char opacity, ImelAlignment alignment)
{
 ImelImage *image;
 long int x, y, j, k[2], width[2], height[2];

 image = imel_image_copy (img1);
 
 width[0] = (long int) img1->width;
 width[1] = (long int) img2->width;
 height[0] = (long int) img1->height;
 height[1] = (long int) img2->height;
 
 switch (alignment) {
   case IMEL_ALIGNMENT_TL:
        for ( y = 0; y < height[0] && y < height[1]; y++ )
              for ( x = 0; x < width[0] && x < width[1]; x++ )
                    image->pixel[y][x] = imel_pixel_union (img1->pixel[y][x], img2->pixel[y][x], opacity);
        break;
   case IMEL_ALIGNMENT_TR:
        if ( width[0] <= width[1] ) {
             j = width[1] - width[0];
             for ( y = 0; y < height[0] && y < height[1]; y++ )
                   for ( x = 0; x < width[0] && x < width[1]; x++ )
                         image->pixel[y][x] = imel_pixel_union (img1->pixel[y][x], img2->pixel[y][j + x], opacity);
        }
        else {
             j = width[0] - width[1];
             for ( y = 0; y < height[0] && y < height[1]; y++ )
                   for ( x = 0; x < width[0] && x < width[1]; x++ )
                         image->pixel[y][j + x] = imel_pixel_union (img1->pixel[y][j + x], img2->pixel[y][x], opacity);
        }
        break;
   case IMEL_ALIGNMENT_BL:
        if ( height[0] >= height[1] ) {
             j = height[0] - height[1];
             for ( y = 0; y < height[0] && y < height[1]; y++ )
                   for ( x = 0; x < width[0] && x < width[1]; x++ )
                         image->pixel[j + y][x] = imel_pixel_union (img1->pixel[j + y][x], img2->pixel[y][x], opacity);
        }
        else {
             j = height[1] - height[0];
             for ( y = 0; y < height[0] && y < height[1]; y++ )
                   for ( x = 0; x < width[0] && x < width[1]; x++ )
                         image->pixel[y][x] = imel_pixel_union (img1->pixel[y][x], img2->pixel[j + y][x], opacity);
        }
        break;
   case IMEL_ALIGNMENT_BR:
        for ( y = height[0] - 1, k[0] = height[1] - 1; y >= 0 && k[0] >= 0; k[0]--, y-- ) {
              for ( x = width[0] - 1, k[1] = width[1] - 1; x >= 0 && k[1] >= 0; k[1]--, x-- ) {
                    image->pixel[y][x] = imel_pixel_union (img1->pixel[y][x], img2->pixel[k[0]][k[1]], opacity);
                    if ( !k[1] || !x )
                         break;
              }
              if ( !k[0] || !y )
                   break;
        }
        break;
 }

 return image;
}

/**
 * @brief Slant an image or a small area inside it
 * 
 * This function slant @p image following a line from coordinate \f$(x_0,y_0)\f$
 * to coordinate \f$(x_1,y_1)\f$ with a choosen @p orientation.
 * 
 * @image html images/distort_example.jpg
 * @image latex images/distort_example.eps
 * 
 * @param image Image to slant
 * @param x0 Start x coordinate of followed line
 * @param y0 Start y coordinate of followed line
 * @param x1 End x coordinate of followed line
 * @param y1 End y coordinate of followed line
 * @param orientation Vertical or Horizontal slant
 * @param lengthens If TRUE the pixels that are 'empty' after the operation
 * will be setted with the value of the last pixel in that line, else if FALSE
 * that pixels will be setted with total transparency and base color black.
 * @return Result image
 * 
 * @see ImelOrientation
 * @see imel_image_shift
 * @see imel_image_shift_lines
 * @see imel_image_perspective
 */
ImelImage *imel_image_slant (ImelImage *image, ImelSize x0, ImelSize y0, ImelSize x1, ImelSize y1, 
                             ImelOrientation orientation, bool lengthens)
{
 ImelImage *l_image;
 ImelSize x, **p, j = 0, ex, y, ey;
 long int k, m, length[2];
 static ImelPixel alpha = { 0, 0, 0, -255 };
 
 return_var_if_fail (image, NULL);
 
 p = __imel_draw_get_line_points_array (x0, y0, x1, y1, &(length[0]), &(length[1]));
 l_image = imel_image_copy (image);
 
 if ( orientation == IMEL_ORIENTATION_VERTICAL ) {
      for ( x = **p, ex = **p + *length, free (p[0]); x < ex && x < image->width; x++, j++ ) {
            if ( !j ) 
                 continue;
       
            if ( *p[1] > p[1][*length - 1] ) {
                 k = ((long int) *(p[1])) - ((long int) p[1][j]);
                 m = ((long int) image->height) - (k + 1);

                 for ( y = k; y < image->height; y++ ) {
                       __imel_draw_put_pixel (l_image, x, y - k, image->pixel[y][x]);
                       
                       if ( y > m ) {
                            if ( lengthens )
                                 __imel_draw_put_pixel (l_image, x, y, image->pixel[image->height - 1][x]); 
                            else __imel_draw_put_pixel (l_image, x, y, alpha);
                       }
                  }
            }
            else {
                  k = ((long int) p[1][j]) - ((long int) *(p[1]));
                  for ( y = 0; y < image->height; y++ ) {
                        if ( y < k ) {
                             if ( lengthens )
                                  __imel_draw_put_pixel (l_image, x, y, image->pixel[0][x]);
                             else __imel_draw_put_pixel (l_image, x, y, alpha);
                        }
                       
                        __imel_draw_put_pixel (l_image, x, y + k, image->pixel[y][x]);
                  }
           }
      }
      free (p[1]);
 }
 else {
      for ( y = *(p[1]), ey = *(p[1]) + length[1], free (p[1]); y < ey && y < image->height; y++, j++ ) {
            if ( !j ) 
                 continue;
       
            if ( *p[0] > p[0][length[1] - 1] ) {
                 k = ((long int) *(p[0])) - ((long int) p[0][j]);
                 m = ((long int) image->width) - (k + 1);

                 for ( x = k; x < image->width; x++ ) {
                       __imel_draw_put_pixel (l_image, x - k, y, image->pixel[y][x]);
                       
                       if ( x > m ) {
                            if ( lengthens )
                                 __imel_draw_put_pixel (l_image, x, y, image->pixel[y][image->width - 1]); 
                            else __imel_draw_put_pixel (l_image, x, y, alpha);
                       }
                  }
            }
            else {
                  k = ((long int) p[0][j]) - ((long int) *(p[0]));
                  for ( x = 0; x < image->width; x++ ) {
                        if ( x < k ) {
                             if ( lengthens )
                                  __imel_draw_put_pixel (l_image, x, y, image->pixel[y][0]);
                             else __imel_draw_put_pixel (l_image, x, y, alpha);
                        }
                       
                        __imel_draw_put_pixel (l_image, x + k, y, image->pixel[y][x]);
                  }
           }
      }
      free (p[0]);
 }
 
 free (p);
 
 return l_image;
}

/**
 * @brief Shift an image
 * 
 * This function shift @p image to the Right, to the Left, Up or Down of chosen size.
 * 
 * @param image Image to shift
 * @param orientation Specified if the shift is horizontal or vertical
 * @param move_pxl Length of shift ( positive values for Right, Down, negative for Left, Up )
 * @param lengthens If TRUE the pixels that are 'empty' after the operation
 * will be setted with the value of the last pixel in that line, else if FALSE
 * that pixels will be setted with total transparency and base color black.
 * 
 * @see ImelOrientation
 * @see imel_image_shift_lines
 * @see imel_image_perspective
 * @see imel_image_slant
 */
void imel_image_shift (ImelImage *image, ImelOrientation orientation, long int move_pxl, bool lengthens)
{
 long int y, x;
 static ImelPixel alpha = { 0, 0, 0, -255 };
 
 return_if_fail (image && move_pxl);
 if ( orientation == IMEL_ORIENTATION_HORIZONTAL ) {
      return_if_fail (abs (move_pxl) < image->width);
      
      if ( move_pxl > -1 ) {
           for ( y = 0; y < image->height; y++ ) {
                 for ( x = image->width - 1; x > -1; x-- ) 
                       __imel_draw_put_pixel (image, x + move_pxl, y, image->pixel[y][x]);
                       
                 for ( x = move_pxl - 1; x > -1; x-- )
                       if ( lengthens )
                            __imel_draw_put_pixel (image, x, y, image->pixel[y][move_pxl]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
      else {
           for ( y = 0; y < image->height; y++ ) {
                 for ( x = 0; x < image->width; x++ )  
                       __imel_draw_put_pixel (image, x + move_pxl, y, image->pixel[y][x]);
                 
                 for ( x = image->width - (move_pxl + 1); x < image->width; x++ ) 
                       if ( lengthens ) 
                            __imel_draw_put_pixel (image, x, y, image->pixel[y][-move_pxl]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
 }
 else {
      return_if_fail (abs (move_pxl) < image->height);
      
      if ( move_pxl > -1 ) {
           for ( x = 0; x < image->width; x++ ) {
                 for ( y = image->height - 1; y > -1; y-- ) 
                       __imel_draw_put_pixel (image, x, y + move_pxl, image->pixel[y][x]);
                       
                 for ( y = move_pxl - 1; y > -1; y-- )
                       if ( lengthens )
                            __imel_draw_put_pixel (image, x, y, image->pixel[move_pxl][x]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
      else {
           for ( x = 0; x < image->width; x++ ) {
                 for ( y = 0; x < image->height; y++ )  
                       __imel_draw_put_pixel (image, x, y + move_pxl, image->pixel[y][x]);
                 
                 for ( y = image->height - (move_pxl + 1); y < image->height; y++ ) 
                       if ( lengthens ) 
                            __imel_draw_put_pixel (image, x, y, image->pixel[-move_pxl][x]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
 }
}

/**
 * @brief Shift only some image lines
 * 
 * This function shift the @p line and next @p width lines of @p image to the Right, 
 * to the Left, Up or Down of chosen size.
 * 
 * @param image Image with the lines to shift
 * @param move_type Specified if the shift is horizontal or vertical
 * @param line Position of the start line to shift
 * @param width Width of lines to shift ( including start line )
 * @param move_pixel Length of shift ( positive values for Right, Down, negative for Left, Up )
 * @param lengthens If TRUE the pixels that are 'empty' after the operation
 * will be setted with the value of the last pixel in that line, else if FALSE
 * that pixels will be setted with total transparency and base color black.
 * 
 * @see ImelOrientation
 * @see imel_image_shift
 * @see imel_image_perspective
 * @see imel_image_slant
 */
bool imel_image_shift_lines (ImelImage *image, ImelOrientation move_type, ImelSize line, ImelSize width, 
                             long int move_pixel, bool lengthens)
{
 ImelSize really_width;
 long int y, ey, x, ex;
 static ImelPixel alpha = { 0, 0, 0, -255 };
 
 return_var_if_fail (width && image, false);
 
 if ( move_type == IMEL_ORIENTATION_HORIZONTAL ) {
      return_var_if_fail (abs (move_pixel) < image->width, false);
      really_width = ((line + width) >= image->height) ? (image->height - ++line) : width;
      
      if ( move_pixel > -1 ) {
           for ( y = line, ey = line + really_width; y < ey; y++ ) {
                 for ( x = image->width - 1; x > -1; x-- ) 
                       __imel_draw_put_pixel (image, x + move_pixel, y, image->pixel[y][x]);
                       
                 for ( x = move_pixel - 1; x > -1; x-- )
                       if ( lengthens )
                            __imel_draw_put_pixel (image, x, y, image->pixel[y][move_pixel]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
      else {
           for ( y = line, ey = line + really_width; y < ey; y++ ) {
                 for ( x = 0; x < image->width; x++ )  
                       __imel_draw_put_pixel (image, x + move_pixel, y, image->pixel[y][x]);
                 
                 for ( x = image->width - (move_pixel + 1); x < image->width; x++ ) 
                       if ( lengthens ) 
                            __imel_draw_put_pixel (image, x, y, image->pixel[y][-move_pixel]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
 }
 else {
      return_var_if_fail (abs (move_pixel) < image->height, false);
      really_width = ((line + width) >= image->width) ? (image->width - ++line) : width;
      
      if ( move_pixel > -1 ) {
           for ( x = line, ex = line + really_width; x < ex; x++ ) {
                 for ( y = image->height - 1; y > -1; y-- ) 
                       __imel_draw_put_pixel (image, x, y + move_pixel, image->pixel[y][x]);
                       
                 for ( y = move_pixel - 1; y > -1; y-- )
                       if ( lengthens )
                            __imel_draw_put_pixel (image, x, y, image->pixel[move_pixel][x]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
      else {
           for ( x = line, ex = line + really_width; x < ex; x++ ) {
                 for ( y = 0; x < image->height; y++ )  
                       __imel_draw_put_pixel (image, x, y + move_pixel, image->pixel[y][x]);
                 
                 for ( y = image->height - (move_pixel + 1); y < image->height; y++ ) 
                       if ( lengthens ) 
                            __imel_draw_put_pixel (image, x, y, image->pixel[-move_pixel][x]);
                       else __imel_draw_put_pixel (image, x, y, alpha); 
           }
      }
 }
 
 return true;
}

/**
 * @brief Shift the RGB values of an image
 * 
 * This function applies a shift operation for each RGB channel. Negative
 * value for left shift, positive for right shift. 
 * 
 * @param image Image to apply the shift
 * @param bpc_shift_red Shift for red channel
 * @param bpc_shift_green Shift for green channel
 * @param bpc_shift_blue Shift for blue channel
 */
void imel_image_shift_bpc (ImelImage *image, int bpc_shift_red, int bpc_shift_green, int bpc_shift_blue)
{
 static bool flag[3];
 ImelSize x, y;
 
 return_if_fail (image);
 
 flag[0] = bpc_shift_red < 1;
 flag[1] = bpc_shift_green < 1;
 flag[2] = bpc_shift_blue < 1;
 
 bpc_shift_red = abs (bpc_shift_red);
 bpc_shift_green = abs (bpc_shift_green);
 bpc_shift_blue = abs (bpc_shift_blue);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             if ( flag[0] )
                  image->pixel[y][x].red <<= bpc_shift_red;
             else image->pixel[y][x].red >>= bpc_shift_red;
             
             if ( flag[1] )
                  image->pixel[y][x].green <<= bpc_shift_green;
             else image->pixel[y][x].green >>= bpc_shift_green;
             
             if ( flag[2] )
                  image->pixel[y][x].blue <<= bpc_shift_blue;
             else image->pixel[y][x].blue >>= bpc_shift_blue;
       }
 }
}

/**
 * @brief Remove noise from an image
 * 
 * This function remove noise from @p image. Compare each pixel with all other around
 * in a square with side @p size_q pixel. If current pixel are a value greater then
 * the average of others pixel compared, with a specified @p tollerance, change
 * its value with the average found.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("apply_noise.jpg", 0, NULL);
 * ImelMask mask = IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE;
 * 
 * imel_image_remove_noise (image, 6, mask, 24);
 * @endcode
 * @image html images/apply_noise.jpg "Input Image"
 * @image html images/remove_noise.jpg "Result Image"
 * @image latex images/apply_noise.eps "Input Image"
 * @image latex images/remove_noise.eps "Result Image"
 * 
 * @param image Image with noise
 * @param size_q Size of the square side where the current pixel are.
 * @param mask Channels affected from noise.
 * @param tollerance Tollerance when compare current pixel with the average of others.
 * 
 * @see imel_image_apply_noise
 */ 
void imel_image_remove_noise (ImelImage *image, ImelSize size_q, ImelMask mask, ImelColor tollerance)
{
 long int y, x, j, k, q = size_q >> 1, z;
 int32_t m[4];
 
 return_if_fail (image && size_q);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = z = 0; x < image->width; x++, z = 0 ) {
             memset (m, 0, sizeof (long int) * 3);
             
             for ( j = y - q; j <= (y + q); j++ ) {
                   for ( k = x - q; k <= (x + q); k++ ) {
                         if ( (k > -1 && k < image->width) && (j > -1 && j < image->height) ) {
                              m[0] += (int32_t) image->pixel[j][k].red;
                              m[1] += (int32_t) image->pixel[j][k].green;
                              m[2] += (int32_t) image->pixel[j][k].blue;
                              
                              if ( image->pixel[j][k].level > -1 )
                                   m[3] += 0xff + image->pixel[j][k].level;
                              else m[3] += image->pixel[j][k].level;
                              
                              z++;
                         }
                   }
             }
             
             m[0] /= z ? z : 1;
             m[1] /= z ? z : 1;
             m[2] /= z ? z : 1;
             m[3] /= z ? z : 1;
             
             switch ( mask ) {
              case IMEL_MASK_RED:
                    if ( !imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance) )
                         imel_pixel_set (&(image->pixel[y][x]), m[0], image->pixel[y][x].green,
                                         image->pixel[y][x].blue, image->pixel[y][x].level);
                    break;
              case IMEL_MASK_GREEN:
                    if ( !imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance) )  
                         imel_pixel_set (&(image->pixel[y][x]), image->pixel[y][x].red, m[1], 
                                         image->pixel[y][x].blue, image->pixel[y][x].level);
                    break;
              case IMEL_MASK_BLUE:
                    if ( !imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance) ) 
                         imel_pixel_set (&(image->pixel[y][x]), image->pixel[y][x].red, 
                                         image->pixel[y][x].green, m[2], image->pixel[y][x].level);
                    break;
              case IMEL_MASK_LEVEL:
                    if ( !imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance) )
                         imel_pixel_set (&(image->pixel[y][x]), image->pixel[y][x].red,
                                         image->pixel[y][x].green, image->pixel[y][x].blue, m[3]);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)) )
                         imel_pixel_set (&(image->pixel[y][x]), m[0], m[1], image->pixel[y][x].blue,
                                         image->pixel[y][x].level);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_LEVEL:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance) 
                        && imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], image->pixel[y][x].green,  
                                         image->pixel[y][x].blue, m[3]);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_LEVEL:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)
                        && imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], m[1], image->pixel[y][x].blue, m[3]);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance) 
                        && imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], image->pixel[y][x].green,
                                         m[2], image->pixel[y][x].level);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance)
                        && imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], image->pixel[y][x].green, m[2], m[3]);
                    break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                    if ( !(imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)
                        && imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance)) )
                         imel_pixel_set (&(image->pixel[y][x]), image->pixel[y][x].red, m[1], m[2],
                                         image->pixel[y][x].level);
                    break;
              case IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)
                        && imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), image->pixel[y][x].red, m[1], m[2], 
                                         image->pixel[y][x].level);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)
                        && imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], m[1], m[2], image->pixel[y][x].level);
                    break;
              case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                    if ( !(imel_pixel_compare_level (m[0], (ImelLevel) image->pixel[y][x].red, tollerance)
                        && imel_pixel_compare_level (m[1], (ImelLevel) image->pixel[y][x].green, tollerance)
                        && imel_pixel_compare_level (m[2], (ImelLevel) image->pixel[y][x].blue, tollerance)
                        && imel_pixel_compare_level (m[3], image->pixel[y][x].level, tollerance)) ) 
                         imel_pixel_set (&(image->pixel[y][x]), m[0], m[1], m[2], m[3]);
                    break;                
            }
       }
 }
}

/**
 * @brief Apply a noise to an image
 * 
 * This function apply a noise to @p image at one or more RGB channels.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("apply_noise.jpg", 0, NULL);
 * ImelMask mask = IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE;
 * 
 * imel_image_apply_noise (image, 90, 30, mask, IMEL_NOISE_OPERATION_SUM, false);
 * @endcode
 * @image html images/noise.jpg "Input Image"
 * @image html images/apply_noise.jpg "Result Image"
 * @image latex images/noise.eps "Input Image"
 * @image latex images/apply_noise.eps "Result Image"
 * 
 * @param image Image to apply the noise
 * @param noise_range Value for @p operation. This value specifies the max value
 * can be added, subtracted, multiply or divided when noise is applied. 
 * @param noise_quantity Specifies how much noise can be applied randomly to image. 
 * Values: 1 for apply the noise to each pixel, 4294967295 is the max value.
 * @param mask Channels affected from noise.
 * @param operation Operation to do when apply the noise
 * @param nepc If TRUE apply the noise value calculated to each RGB channel specified,
 * else each noise value will be calculated separately.
 * 
 * @see ImelNoiseOperation
 * @see imel_image_remove_noise
 */ 
void imel_image_apply_noise (ImelImage *image, ImelColor noise_range, ImelSize noise_quantity, 
                             ImelMask mask, ImelNoiseOperation operation, bool nepc)
{
 ImelSize x, y;
 ImelSize noise_value, noise_to_color;
 ImelNoiseOperation __operation;
 
 if ( !ImelRandom ) {
      srand (time (NULL));
      ImelRandom = 1;
 }
 
 return_if_fail (image && noise_quantity > 0 && noise_range > 0);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             if ( operation == IMEL_NOISE_OPERATION_RANDOM )
                  __operation = rand () % 4;
             else __operation = operation;
             
             if ( nepc ) {
                  noise_value = rand () % noise_range;
                  
                  if ( (rand () % noise_quantity) )
                       continue;
                       
                  switch ( mask ) {
                    case IMEL_MASK_RED:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                           + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                           - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                           * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                           + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                           - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                           * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                           + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].blue = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                           - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                           * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].blue = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_LEVEL:                               
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) noise_value), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) noise_value), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) noise_value), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) noise_value), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    break;                
            }
       }
       else {                  
             if ( (rand () % noise_quantity) )
                  continue;
                  
             switch ( mask ) {
                    case IMEL_MASK_RED:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                           + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                           - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                           * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                           + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                           - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                           * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                           + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].blue = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                           - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                           * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_LEVEL:                               
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    case IMEL_MASK_RED | IMEL_MASK_GREEN | IMEL_MASK_BLUE | IMEL_MASK_LEVEL:
                          switch ( __operation ) {
                             case IMEL_NOISE_OPERATION_SUM:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             + ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_SUBTRACT:
                                   image->pixel[y][x].red   = (ImelColor) max (((int32_t) image->pixel[y][x].red) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].green = (ImelColor) max (((int32_t) image->pixel[y][x].green) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].blue  = (ImelColor) max (((int32_t) image->pixel[y][x].blue) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   image->pixel[y][x].level = (ImelLevel) max (((int32_t) image->pixel[y][x].level) 
                                                                             - ((int32_t) (rand () % noise_range)), 0);
                                   break;
                             case IMEL_NOISE_OPERATION_MULTIPLY:
                                   image->pixel[y][x].red   = (ImelColor) min (((int32_t) image->pixel[y][x].red) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].green = (ImelColor) min (((int32_t) image->pixel[y][x].green) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].blue  = (ImelColor) min (((int32_t) image->pixel[y][x].blue) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   image->pixel[y][x].level = (ImelLevel) min (((int32_t) image->pixel[y][x].level) 
                                                                             * ((int32_t) (rand () % noise_range)), 255);
                                   break;
                             case IMEL_NOISE_OPERATION_DIVIDE:
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].red   = image->pixel[y][x].red / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].green = image->pixel[y][x].green / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].blue  = image->pixel[y][x].blue / (noise_value ? noise_value : 1);
                                   noise_value = rand () % noise_range;
                                   image->pixel[y][x].level = image->pixel[y][x].level / (noise_value ? noise_value : 1);
                                   break;
                          }
                          break;
                    break;                
            }
       }
    }
 } 
}
