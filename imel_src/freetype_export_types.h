/*
 * "freetype_export_types.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef freetype_types
#define freetype_types
/**
 * @file freetype_export_types.h
 * @author Davide Francesco Merico
 * @brief This file contains macro and type for the FreeType library.
 */
 
#ifndef FT_ENC_TAG
#define FT_ENC_TAG( value, a, b, c, d )         \
          value = ( ( (unsigned long int) (a) << 24 ) |  \
                    ( (unsigned long int) (b) << 16 ) |  \
                    ( (unsigned long int) (c) <<  8 ) |  \
                      (unsigned long int) (d)         )

#endif /* FT_ENC_TAG */

  typedef enum  FT_Encoding_
  {
    FT_ENC_TAG( FT_ENCODING_NONE, 0, 0, 0, 0 ),

    FT_ENC_TAG( FT_ENCODING_MS_SYMBOL, 's', 'y', 'm', 'b' ),
    FT_ENC_TAG( FT_ENCODING_UNICODE,   'u', 'n', 'i', 'c' ),

    FT_ENC_TAG( FT_ENCODING_SJIS,    's', 'j', 'i', 's' ),
    FT_ENC_TAG( FT_ENCODING_GB2312,  'g', 'b', ' ', ' ' ),
    FT_ENC_TAG( FT_ENCODING_BIG5,    'b', 'i', 'g', '5' ),
    FT_ENC_TAG( FT_ENCODING_WANSUNG, 'w', 'a', 'n', 's' ),
    FT_ENC_TAG( FT_ENCODING_JOHAB,   'j', 'o', 'h', 'a' ),

    /* for backwards compatibility */
    FT_ENCODING_MS_SJIS    = FT_ENCODING_SJIS,
    FT_ENCODING_MS_GB2312  = FT_ENCODING_GB2312,
    FT_ENCODING_MS_BIG5    = FT_ENCODING_BIG5,
    FT_ENCODING_MS_WANSUNG = FT_ENCODING_WANSUNG,
    FT_ENCODING_MS_JOHAB   = FT_ENCODING_JOHAB,

    FT_ENC_TAG( FT_ENCODING_ADOBE_STANDARD, 'A', 'D', 'O', 'B' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_EXPERT,   'A', 'D', 'B', 'E' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_CUSTOM,   'A', 'D', 'B', 'C' ),
    FT_ENC_TAG( FT_ENCODING_ADOBE_LATIN_1,  'l', 'a', 't', '1' ),

    FT_ENC_TAG( FT_ENCODING_OLD_LATIN_2, 'l', 'a', 't', '2' ),

    FT_ENC_TAG( FT_ENCODING_APPLE_ROMAN, 'a', 'r', 'm', 'n' )

  } FT_Encoding;
 
 typedef enum  FT_Render_Mode_
  {
    FT_RENDER_MODE_NORMAL = 0,
    FT_RENDER_MODE_LIGHT,
    FT_RENDER_MODE_MONO,
    FT_RENDER_MODE_LCD,
    FT_RENDER_MODE_LCD_V,

    FT_RENDER_MODE_MAX

  } FT_Render_Mode; 
#endif
