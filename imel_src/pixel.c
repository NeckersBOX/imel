/*
 * "pixel.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "header.h"
/**
 * @file pixel.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to operate with guide lines.
 */

#define _IMEL_RANGE_COLOR(a,b,t)   (((a) <= abs_color (((int) b) + ((int) t))) \
                                 && ((a) >= abs_color (((int) b) - ((int) t))))
                                 
ImelPixel imel_pixel_union (ImelPixel a, ImelPixel b, unsigned char _opacity);

/**
 * @brief Make a new pixel
 * 
 * This function makes a new pixel with a RGB value and level specified.
 * 
 * @param red Red channel. Values between 0 and 255
 * @param green Green channel. Values between 0 and 255
 * @param blue Blue channel. Values between 0 and 255
 * @param level Level for values between 0 and 2147483647. Alpha for values
 * between 0 and -255.
 * @return A new #ImelPixel type.
 */
ImelPixel imel_pixel_new (ImelColor red, ImelColor green, ImelColor blue, ImelLevel level)
{
 ImelPixel p;

 p.red = red;
 p.green = green;
 p.blue = blue;
 p.level = level;

 return p;
}

/**
 * @brief Make a new pixel from a string
 * 
 * This function makes a new pixel with a RGB value specified from a string
 * in HTML format '\#rrggbb' and level specified separately.
 * 
 * @param string Color in HTML format
 * @param level Level for values between 0 and 2147483647. Alpha for values
 * between 0 and -255.
 * @return A new #ImelPixel type.
 */
ImelPixel imel_pixel_new_from_string (const char *string, ImelLevel level)
{
 char buff[2];
 ImelPixel p = { 0, 0, 0, -255 };
 int i;

 p.level = level;

 return_var_if_fail (string, p);

 if ( *string != '#' ) {
      imel_printf_debug ("imel_pixel_new_from_string", NULL, "warning", "string not begin with '#'");

      return p;
 }

 if ( strlen (string) != 7 ) {
      imel_printf_debug ("imel_pixel_new_from_string", NULL, "warning", "string length not valid");

      return p;
 }

 for ( i = 1; i < 7; i++ ) {
       if ( !isxdigit (string[i])) {
            imel_printf_debug ("imel_pixel_new_from_string", NULL, "warning", "string not valid");
            
            return p;
       }
 }

 for ( i = 1; i < 7; i += 2 ) {
       buff[0] = string[i];
       buff[1] = string[i+1];
            if ( i < 3 ) p.red = strtol (buff, NULL, 16);
       else if ( i < 6 ) p.green = strtol (buff, NULL, 16);
       else              p.blue = strtol (buff, NULL, 16);
 }

 return p;
}

/**
 * @brief Set a new value for a pixel
 * 
 * @param pixel Pixel to modify
 * @param red Red channel. Values between 0 and 255
 * @param green Green channel. Values between 0 and 255
 * @param blue Blue channel. Values between 0 and 255
 * @param level Level for values between 0 and 2147483647. Alpha for values
 * between 0 and -255.
 */
void imel_pixel_set (ImelPixel *pixel, ImelColor red, ImelColor green, ImelColor blue, ImelLevel level)
{
 return_if_fail (pixel);

 pixel->red = red;
 pixel->green = green;
 pixel->blue = blue;
 pixel->level = level;
}

/**
 * @brief Get the chromatic distance between two colors
 * 
 * @param a Fist color
 * @param b Second color
 * @return Chromatic distance between @p a and @p b
 */
double imel_pixel_get_distance (ImelPixel a, ImelPixel b)
{
 return sqrt (((b.red   - a.red)   * (b.red   - a.red))
	        + ((b.green - a.green) * (b.green - a.green))
	        + ((b.blue  - a.blue)  * (b.blue  - a.blue)));
}

/**
 * @brief Copy the values of a pixel in another one.
 * 
 * @param pixel Pixel to set equal to @p pxl
 * @param pxl Pixel to copy
 */
void imel_pixel_set_from_pixel (ImelPixel *pixel, ImelPixel pxl)
{
 return_if_fail (pixel);

 pixel->red = pxl.red;
 pixel->green = pxl.green;
 pixel->blue = pxl.blue;
 pixel->level = pxl.level;
}

/**
 * @brief Set a new value for a pixel from a string
 * 
 * This function sets new values for @p pixel from a string in HTML format
 * '\#rrggbb'.
 * 
 * @param pixel Pixel to modify
 * @param string Color in HTML format
 * @param level Level for values between 0 and 2147483647. Alpha for values
 * between 0 and -255.
 */
void imel_pixel_set_from_string (ImelPixel *pixel, const char *string, ImelLevel level)
{
 char buff[2];
 int i;

 return_if_fail (string);

 if ( *string != '#' ) {
      imel_printf_debug ("imel_pixel_set_from_string", NULL, "warning", "string not begin with '#'");

      return;
 }

 if ( strlen (string) != 7 ) {
      imel_printf_debug ("imel_pixel_set_from_string", NULL, "warning", "string length not valid");

      return;
 }

 for ( i = 1; i < 7; i++ ) {
       if ( !isxdigit (string[i])) {
            imel_printf_debug ("imel_pixel_set_from_string", NULL, "warning", "string not valid");

            return;
       }
 }


 for ( i = 1; i < 7; i += 2 ) {
       buff[0] = string[i];
       buff[1] = string[i+1];
            if ( i < 3 ) pixel->red = strtol (buff, NULL, 16);
       else if ( i < 6 ) pixel->green = strtol (buff, NULL, 16);
       else              pixel->blue = strtol (buff, NULL, 16);
 }
 pixel->level = level;
}

/**
 * @brief Copy a pixel over another one
 * 
 * This function copy @p src on @p dest only if <tt>src->level</tt>
 * is equal or greater then <tt>dest->level</tt>. If <tt>src->level</tt>
 * is an alpha value call #imel_pixel_union function to copy the element.
 * 
 * @param dest Destination pixel
 * @param src Source pixel
 */
void imel_pixel_copy (ImelPixel *dest, ImelPixel src)
{
 ImelPixel upxl;

 return_if_fail (dest);

 if ( src.level < dest->level && src.level > -1)
      return;

 if ( src.level < 0 )
      upxl = imel_pixel_union (*dest, src, ( src.level < -255 ) ? 0 : src.level);
 else upxl = src;

 dest->red = upxl.red;
 dest->green = upxl.green;
 dest->blue = upxl.blue;
 dest->level = upxl.level;
}

static ImelColor abs_color (int expression)
{
 return (expression < 0) ? 0 : (expression > 255) ? 255 : expression;
}

bool imel_pixel_compare (ImelPixel a, ImelPixel b, ImelSize tollerance)
{
 if ( _IMEL_RANGE_COLOR (a.red, b.red, tollerance)
   && _IMEL_RANGE_COLOR (a.green, b.green, tollerance)
   && _IMEL_RANGE_COLOR (a.blue, b.blue, tollerance) )
      return true;

 return false;
}

/**
 * @brief Compare two level
 * 
 * This function compare the levels @p a and @p b with a chosen @p tollerance.
 * 
 * @param a First Level
 * @param b Second level
 * @param tollerance Tollerance
 * @return TRUE if \f$(b-tollerance)\leq(a)\leq(b+tollerance)\f$, else FALSE.
 */
bool imel_pixel_compare_level (ImelLevel a, ImelLevel b, ImelSize tollerance)
{
 if ( a <= (b + ((ImelLevel) tollerance)) && a >= (b - ((ImelLevel) tollerance)) )
      return true;
 
 return false;
}

/**
 * @brief Join two pixel with a chosen opacity
 * 
 * This function join pixel @p a with pixel @p b given to it a chosen
 * @p _opacity.
 * 
 * @param a Base pixel
 * @param b Pixel to join with @p a
 * @param _opacity Opacity of pixel @p b. Values between 0, transparent,
 * and 255.
 * @return A new #ImelPixel type.
 */
ImelPixel imel_pixel_union (ImelPixel a, ImelPixel b, unsigned char _opacity)
{
 ImelPixel r;
 unsigned char real_opacity;
 unsigned int c[2];

 if ( a.level <= -255)
      return b;

 if ( b.level <= -255)
      return a;
 
 if ( b.level >= 0 )
      real_opacity = _opacity;
 else real_opacity = (-255 - b.level) * _opacity * -255;
 
 c[0] = (real_opacity * b.red) / 255;
 c[1] = ((255 - real_opacity) * a.red) / 255;
 r.red = ((c[0] + c[1]) > 255) ? 255 : c[0] + c[1];

 c[0] = (real_opacity * b.green) / 255;
 c[1] = ((255 - real_opacity) * a.green) / 255;
 r.green = ((c[0] + c[1]) > 255) ? 255 : c[0] + c[1];

 c[0] = (real_opacity * b.blue) / 255;
 c[1] = ((255 - real_opacity) * a.blue) / 255;
 r.blue = ((c[0] + c[1]) > 255) ? 255 : c[0] + c[1];

 r.level = ( b.level > a.level ) ? b.level : a.level;

 return r;
}

/**
 * @brief Convert an ImelPixel in a single RGBA variable
 * 
 * This function converts @p pxl data in a single 32 bit
 * variable that contains RGBA values. This conversion
 * loss information on level values different from \f$[0,-255]\f$
 * range.
 * 
 * @param pxl Pixel to convert in a RGBA value
 * @return A 32 bit variable with @p pxl data
 * 
 * @see RGBA_R_MASK
 * @see RGBA_G_MASK
 * @see RGBA_B_MASK
 * @see RGBA_A_MASK
 * @see imel_pixel_new_from_rgba
 */
uint32_t imel_pixel_get_rgba (ImelPixel pxl)
{
 uint32_t rgba = 0;

 rgba = (rgba | pxl.red) << 8;
 rgba = (rgba | pxl.green) << 8;
 rgba = (rgba | pxl.blue) << 8;

 if ( pxl.level < 0 )
      rgba |= (abs (pxl.level) > 255) ? 255 : abs (pxl.level);

 return rgba;
}

/**
 * @brief Convert an RGBA variable in a ImelPixel type
 * 
 * @param rgba RGBA variable
 * @return A new #ImelPixel type
 * 
 * @see RGBA_R_MASK
 * @see RGBA_G_MASK
 * @see RGBA_B_MASK
 * @see RGBA_A_MASK
 * @see imel_pixel_get_rgba
 */
ImelPixel imel_pixel_new_from_rgba (uint32_t rgba)
{
 return imel_pixel_new (RGBA_R_MASK (rgba), RGBA_G_MASK (rgba),
                        RGBA_B_MASK (rgba), -RGBA_A_MASK (rgba));
}

double _hue_to_rgb (double j, double k, double h)
{
 h += ( h < 0 ) ? 1 : (( h > 1 ) ? -1 : 0); 
 
 if ( h < (1.f / 6.f) ) 
      return j + (k - j) * 6.f * h;
 
 if ( h < 0.5f )
      return k;
 
 if ( h < (2.f / 3.f) )
      return j + (k - j) * (2.f / 3.f - h) * 6.f;
 
 return j;
}

/**
 * @brief Convert an ImelHSL in a ImelPixel type
 * 
 * @param value #ImelHSL type to convert
 * @return A new #ImelPixel type with level set to 0.
 * 
 * @see imel_pixel_get_hsl
 */
ImelPixel imel_pixel_new_from_hsl (ImelHSL value)
{
 ImelHSL base;
 ImelPixel p;
 double j, k;
 
 if ( !value.s ) {
	  base.h = base.s = base.l = value.l;
 }
 else {
	  k = (value.l < 0.5f) ? (value.l * (1 + value.s)) 
	                       : (value.l + value.s - value.l * value.s);
	  j = 2 * value.l - k;
	  
	  base.h = _hue_to_rgb (j, k, value.h + (1.f / 3.f)); 
	  base.s = _hue_to_rgb (j, k, value.h);
	  base.l = _hue_to_rgb (j, k, value.h - (1.f / 3.f));
 }
 
 p.red   = (ImelColor) (base.h * 255.f);
 p.green = (ImelColor) (base.s * 255.f);
 p.blue  = (ImelColor) (base.l * 255.f);
 p.level = 0;
 
 return p;
}

/**
 * @brief Convert an ImelPixel in a ImelHSL type
 * 
 * This function converts @p pxl data in a HSL rappresentation
 * that contains Hue, Saturation and Luminosity values. This conversion
 * loss information on level values.
 * 
 * @param p Pixel to convert in a #ImelHSL type
 * @return A new #ImelHSL
 * 
 * @see imel_pixel_new_from_hsl
 */
ImelHSL imel_pixel_get_hsl (ImelPixel p)
{
 ImelHSL base, value;
 double max, min, diff;
 
 base.h = ((double) p.red)   / 255.f;
 base.s = ((double) p.green) / 255.f;
 base.l = ((double) p.blue)  / 255.f;
 
 max = ( base.h > base.s ) ? (( base.h > base.l ) ? base.h : base.l) 
                           : (( base.s > base.l ) ? base.s : base.l);
 
 min = ( base.h < base.s ) ? (( base.h < base.l ) ? base.h : base.l)
                           : (( base.s < base.l ) ? base.s : base.l);
 
 value.l = (max + min) / 2;
 if ( max == min ) {
	  value.h = value.s = 0;
 }
 else {
	  diff = max - min;
	  value.s = (value.l > 0.5f) ? (diff / (2 - max - min)) : (diff / (max + min));
	  
	       if ( max == base.h ) 
	            value.h = (base.s - base.l) / diff + ((base.s < base.l) ? 6 : 0);
	  else if ( max == base.s )
		        value.h = (base.l - base.h) / diff + 2;
      else if ( max == base.l )
                value.h = (base.h - base.s) / diff + 4;
                
	  value.h /= 6.f;
 }
 
 return value;
}
