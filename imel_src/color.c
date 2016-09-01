/*
 * "color.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <stdlib.h>
#include "header.h"

/**
 * @file color.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to operate with colors.
 *
 * These functions allow you to add, subtract or extract information
 * relative to the colors inside Imel types.
 */

#ifndef DOXYGEN_IGNORE_DOC

extern void imel_pixel_copy (ImelPixel *, ImelPixel);

#endif 

/**
 * @brief Sets a background color to a chosen image.
 *
 * This function copy through #imel_pixel_copy @p pixel in each pixel
 * of @p image.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.gif", 0, NULL);
 * ImelPixel bg_pixel = { 255, 255, 255, 0 };
 * 
 * imel_color_set_background (image, bg_pixel);
 * @endcode
 * 
 * @param image Image to set a new background
 * @param pixel Background color
 * 
 * @see imel_pixel_copy
 */
void imel_color_set_background (ImelImage *image, ImelPixel pixel)
{
 ImelSize x, y;
 
 return_if_fail (image);
 
 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_copy (&(image->pixel[y][x]), pixel);
             
}

static bool color_exists (ImelPixel **array, ImelPixel pxl)
{
 register long int i = -1;
 
 while ( array[++i] )
         if ( array[i]->red == pxl.red &&
              array[i]->green == pxl.green &&
              array[i]->blue == pxl.blue )
              return true;
 
 return false;
}

/**
 * @brief Gets a list of unique colors in an image.
 * 
 * This function get the unique colors number in @p image and return
 * a list of this colors.
 * 
 * @code
 * ImelImage *image = imel_image_new_from ("image.png", 0, NULL);
 * ImelSize n_colors;
 * ImelPixel *list;
 * 
 * list = imel_color_get_number (image, &n_colors);
 * ...
 * free (list);
 * @endcode
 * 
 * @param image Image from which exract the colors
 * @param number In this variabile can be saved the colors number
 * @return NULL if @p image isn't valid, else a NULL-terminated list
 * with the found colors.
 * @warning Each pixel of the returned list is linked to an @p image pixel,
 * to free memory call free only on list as in example above.
 */
ImelPixel **imel_color_get_number (ImelImage *image, ImelSize *number)
{
 ImelPixel **c = (ImelPixel **) malloc (sizeof (ImelPixel *));
 ImelSize memory = 1, y = 0, x;
 
 return_var_if_fail (image, NULL);
 
 for ( *c = NULL; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             if ( !color_exists (c, image->pixel[y][x]) )  {
                  c[memory - 1] = &image->pixel[y][x];
                  c = (ImelPixel **) realloc (c, ++memory * sizeof (ImelPixel *));
                  c[memory - 1] = NULL;
             }
       }
 }
 
 if ( number )
      *number = memory;
      
 return c;
}

/**
 * @brief Gets an array with the colors in a pixel
 * 
 * This function make a color array with rgb value in
 * @p pixel.
 * 
 * @param pixel Pixel from which get the colors.
 * @return An array with RGB channels of @p pixel.
 * @note Each channel can be setted to 0 if @p pixel level is 
 * less then -255.  
 */
ImelColor *imel_color_get_from_pixel (ImelPixel pixel)
{
 ImelColor *rgb = (ImelColor *) calloc (3, sizeof (ImelColor));
 
 if ( pixel.level < -255 )
      return rgb;
      
 rgb[0] = pixel.red;
 rgb[1] = pixel.green;
 rgb[2] = pixel.blue;
 
 return rgb;
} 

/**
 * @brief Sum two colors
 * 
 * This function sum colors @p a and @p b.
 * 
 * @param a First color
 * @param b Second color
 * @return A color with the sum of @p a and @p b, if the sum result
 * is greater then 255 return 255.
 */
ImelColor imel_color_sum (ImelColor a, ImelColor b)
{
 int j = (int) a, k = (int) b;
 static int result;
 
 result = j + k;
 
 return (ImelColor) ((result > 255) ? 255 : result);
}

/**
 * @brief Subtract two colors
 * 
 * This function subtract @p b from @p a.
 * 
 * @param a First color
 * @param b Second color
 * @return A color with the result of @p b - @p a, if the result
 * is less then 0 return 0.
 */
ImelColor imel_color_subtract (ImelColor a, ImelColor b)
{
 int j = (int) a, k = (int) b;
 static int result;
 
 result = j - k;
 
 return (ImelColor) ((result > 255) ? 255 : (result < 0) ? 0 : result);
}
