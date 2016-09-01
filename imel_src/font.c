/*
 * "font.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <string.h>
#include <limits.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "header.h"
#include "font.h"

/**
 * @file font.c
 * @author Davide Francesco Merico
 * @brief This file contains function to use fonts in an image.
 *
 * These functions allow you to write a string with the internal Imel
 * font or with a truetype font loaded. 
 * 
 * @note To use a truetype font is used the FreeType library.
 */
 
static ImelImage *imel_font_make_image                    (char c, ImelSize px, ImelPixel pixel);
static void      _imel_font_write_char_from_bitmap_image  (ImelImage **image, FT_GlyphSlot glyph_slot, int *x,
                                                           int y, int max_y, int pixel, ImelPixel pxl);

#ifndef DOXYGEN_IGNORE_DOC

extern ImelImage *imel_image_new (ImelSize width, ImelSize height);
extern ImelPixel imel_pixel_new (ImelColor red, ImelColor green, ImelColor blue, long int level);
extern ImelImage *imel_image_resize (ImelImage *image, ImelSize width, ImelSize height);
extern void imel_image_insert_image (ImelImage *dest, ImelImage *src, ImelSize sx, ImelSize sy);
extern ImelPixel imel_pixel_union (ImelPixel a, ImelPixel b, unsigned char _opacity);
extern void imel_image_free (ImelImage *image);
extern void imel_pixel_copy (ImelPixel *, ImelPixel);
extern void imel_draw_point (ImelImage *, ImelSize, ImelSize, ImelPixel);

#endif

/**
 * @brief Write a string with the internal font
 * 
 * This function write the @p string in @p image from coordinate \f$(x,y)\f$
 * with a size of @p px.
 * 
 * @param image Image where write the @p string
 * @param x Start x coordinate
 * @param y Start y coordinate
 * @param string String to write in @p image
 * @param px Font size, can be specified through ImelFontSize enum.
 * @param pixel Color and level of the string
 * @see ImelFontSize
 */
void imel_font_write_string (ImelImage *image, ImelSize x, ImelSize y,
                             const char *string, ImelSize px, ImelPixel pixel)
{
 int i, len, j;
 ImelImage *c;

 return_if_fail (string && image);

 for ( i = j = 0, len = strlen (string); i < len; i++, j++ ) {
       if ( string[i] == '\n' ) {
            y += px * 2;
            j = -1;
            continue;
       }
            
       c = imel_font_make_image (string[i], px, pixel);
       imel_image_insert_image (image, c, x + (j * px), y);
       imel_image_free (c);
 }
}

/**
 * @brief Write a string in vertical orientation with the internal font
 * 
 * This function write the @p string in @p image from coordinate \f$(x,y)\f$
 * with a size of @p px.
 * 
 * @param image Image where write the @p string
 * @param x Start x coordinate
 * @param y Start y coordinate
 * @param string String to write in @p image
 * @param px Font size, can be specified through ImelFontSize enum.
 * @param pixel Color and level of the string
 * @see ImelFontSize
 */
void imel_font_write_vstring (ImelImage *image, ImelSize x, ImelSize y,
                              const char *string, ImelSize px, ImelPixel pixel)
{
 int i, len;
 ImelImage *c;

 return_if_fail (string && image);

 for ( i = 0, len = strlen (string); i < len; i++ ) {
       if ( string[i] == '\n' )
            continue; 
            
       c = imel_font_make_image (string[i], px, pixel);
       imel_image_insert_image (image, c, x, y + (i * (px * 2)));
       imel_image_free (c);
 }  
}

static ImelImage *imel_font_make_image (char c, ImelSize px, ImelPixel pixel)
{
 ImelImage *im, *r;
 int x, y, j;
 
 im = imel_image_new (7, 14);
 j = (c < 127) ? c - 0x20 : 0x20;
 for ( y = 0; y < 14; y++ ) 
       for ( x = 0; x < 7; x++ )
             if ( imel_font[j][y][x] ) 
                  imel_draw_point (im, x, y, pixel);

 r = imel_image_resize (im, px, px * 2);
 imel_image_free (im);
 
 return r;
}

/**
 * @brief Write a string with a truetype font
 * 
 * This function write the @p string in @p image from coordinate \f$(\_x,\_y)\f$
 * with a size of @p px.
 * 
 * @param image Image, where write the @p string, passed by address
 * @param ttf_file TrueType font file name
 * @param _x Start x coordinate
 * @param y Start y coordinate
 * @param string String to write in @p image
 * @param px Font size
 * @param pixel Color and level of the string
 * @param ... Additional parameters are characterized by a string and a value. 
 * @note The string for last parameter are "render-type" and "charmap" for the
 * possibily value check the link below.
 * @see http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Render_Mode
 * @see http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Encoding
 */
bool imel_font_write_string_with_truetype_font (ImelImage **image, char *ttf_file, ImelSize _x, ImelSize y,
                                                char *string, ImelSize px, ImelPixel pixel, ...)
{
 FT_Library library;
 FT_Face face;
 int len, i = 0, x = _x, max_y = 0, index, j;
 char *argument;
 const char *opt_argument_list[] = { "render-type", 
                                     "charmap", NULL };
 va_list arg_list;
 
 return_var_if_fail (image && ttf_file && string, false);

 FT_Init_FreeType (&library);
 if ( FT_New_Face (library, ttf_file, 0, &face) )
      return false;

 FT_Set_Pixel_Sizes (face, 0, px);
 /*
 for ( va_start (arg_list, pixel), argument = va_arg (arg_list, char *);
       argument; argument = va_arg (arg_list, char *) ) {
       for ( index = 0, i = -1; opt_argument_list[index]; index++ ) {
             if ( !strcmp (opt_argument_list[index], argument) ) {
             	  i = 0;
             	  break;
			 }
	   }
	   
	   if ( i )
	        break;
	   
	   switch (index) {
	   	 case 0: 
	   	   FT_Render_Glyph (face->glyph, va_arg (arg_list, FT_Render_Mode));
	   	   break;
	   	 case 1:
	   	   FT_Select_Charmap (face, va_arg (arg_list, FT_Encoding));
	   	   break;
	   	 default:
           imel_printf_debug ("imel_font_write_string_with_truetype_font", NULL, "warning",
                              "unknown argument");
           index = -1;
           break;
	   }
	   
	   if ( index == -1 )
	        break;
 }
 va_end (arg_list);
 */
 for ( len = strlen (string); i < len; i++ ) {
       if ( FT_Load_Char (face, string[i], FT_LOAD_RENDER) )
            continue;

       if ( max_y < face->glyph->bitmap_top )
            max_y = face->glyph->bitmap_top;
 }

 for ( i = 0, j = x; i < len; i++ ) {
       if ( string[i] == '\n' ) {
            y += px;
            j = x;
            continue;
       }
       
       if ( FT_Load_Char (face, string[i], FT_LOAD_RENDER) )
            continue;
        
       _imel_font_write_char_from_bitmap_image (image, face->glyph, &j, y, max_y, px, pixel);
 }

 FT_Done_Face (face);
 FT_Done_FreeType (library);

 return true;
}
/**
 * @brief Write a string in vertical orientation with a truetype font
 * 
 * This function write the @p string in @p image from coordinate \f$(\_x,\_y)\f$
 * with a size of @p px.
 * 
 * @param image Image, where write the @p string, passed by address
 * @param ttf_file TrueType font file name
 * @param _x Start x coordinate
 * @param y Start y coordinate
 * @param string String to write in @p image
 * @param px Font size
 * @param pixel Color and level of the string
 * @param ... Additional parameters are characterized by a string and a value. 
 * @note The string for last parameter are "render-type" and "charmap" for the
 * possibily value check the link below.
 * @see http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Render_Mode
 * @see http://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Encoding
 */
bool imel_font_write_vstring_with_truetype_font (ImelImage **image, char *ttf_file, ImelSize _x, ImelSize y,
                                                 char *string, ImelSize px, ImelPixel pixel, ...)
{
 FT_Library library;
 FT_Face face;
 int len, i = 0, x = _x, max_y = 0, index, j;
 char *argument;
 const char *opt_argument_list[] = { "render-type", 
                                     "charmap", NULL };
 va_list arg_list;
 
 return_var_if_fail (image && ttf_file && string, false);

 FT_Init_FreeType (&library);
 if ( FT_New_Face (library, ttf_file, 0, &face) )
      return false;

 FT_Set_Pixel_Sizes (face, 0, px);
 
 for ( va_start (arg_list, pixel), argument = va_arg (arg_list, char *);
       argument; argument = va_arg (arg_list, char *) ) {
       for ( index = 0, i = -1; opt_argument_list[index]; index++ ) {
             if ( !strcmp (opt_argument_list[index], argument) ) {
             	  i = 0;
             	  break;
			 }
	   }
	   
	   if ( i )
	        break;
	   
	   switch (index) {
	   	 case 0: 
	   	   FT_Render_Glyph (face->glyph, va_arg (arg_list, FT_Render_Mode));
	   	   break;
	   	 case 1:
	   	   FT_Select_Charmap (face, va_arg (arg_list, FT_Encoding));
	   	   break;
	   	 default:
           imel_printf_debug ("imel_font_write_string_with_truetype_font", NULL, "warning",
                              "unknown argument");
           index = -1;
           break;
	   }
	   
	   if ( index == -1 )
	        break;
 }
 va_end (arg_list);
 
 for ( len = strlen (string); i < len; i++ ) {
       if ( FT_Load_Char (face, string[i], FT_LOAD_RENDER) )
            continue;

       if ( max_y < face->glyph->bitmap_top )
            max_y = face->glyph->bitmap_top;
 }

 for ( i = 0, j = x; i < len; i++, y += px, j = x ) {
       if ( string[i] == '\n' )
            continue;
            
       if ( FT_Load_Char (face, string[i], FT_LOAD_RENDER) )
            continue;
        
       _imel_font_write_char_from_bitmap_image (image, face->glyph, &j, y, max_y, px, pixel);
 }

 FT_Done_Face (face);
 FT_Done_FreeType (library);

 return true;
}

static void _imel_font_write_char_from_bitmap_image (ImelImage **image, FT_GlyphSlot glyph_slot,
                                                     int *x, int y, int max_y, int pixel, ImelPixel pxl)
{
 int w, h, _color;
 ImelPixel *p, up;
 FT_Bitmap *bitmap;
 int ry, rx;

 return_if_fail (image && glyph_slot && x);

 bitmap = &(glyph_slot->bitmap);
 /* ? */
 pixel = 0;

 for ( w = 0 ; w < bitmap->width; w++ ) {
       for ( h = 0; h < bitmap->rows; h++ ) {
             ry = y + (max_y - glyph_slot->bitmap_top) + h;
             rx = *x + w + glyph_slot->bitmap_left;
             
             if ( ry >= (*image)->height || rx >= (*image)->width )
                  continue;
                  
             p = &((*image)->pixel[ry][rx]);
             _color = bitmap->buffer[h * bitmap->width + w];
             
             if ( !_color )
                  continue;
                  
             up = imel_pixel_union (*p, pxl, _color);
             if ( p->level < pxl.level )
                  up.level = -255 + _color;
                  
             imel_pixel_copy (p, up);
       }
 }

 *x += glyph_slot->metrics.horiAdvance >> 6;
}
