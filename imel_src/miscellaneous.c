/*
 * "miscellaneous.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#define HAVE_imel_printf_debug_FUNC
#include "header.h"
/**
 * @file miscellaneous.c
 * @author Davide Francesco Merico
 * @brief This file contains miscellaneus function.
 */
 
#ifndef DOXYGEN_IGNORE_DOC

extern ImelImage  *imel_image_copy  (ImelImage *image);
extern void        imel_image_free  (ImelImage *image);

#endif

/**
 * If brush is enabled, this variable will contain the
 * brush image and will be used in all drawing functions
 * except #imel_draw_point.
 */
ImelImage *global_brush;

/**
 * @brief Enable the brush
 * 
 * @param brush Image containing the brush
 * @return TRUE on success, FALSE on error.
 * 
 * @see global_brush
 * @see imel_disable_brush
 */
bool imel_enable_brush (ImelImage *brush)
{
 return_var_if_fail (brush, false);
 
 global_brush = imel_image_copy (brush);
 
 return true;
}

/**
 * @brief Disable the brush
 * 
 * @return TRUE on success, FALSE on error.
 * 
 * @see global_brush
 * @see imel_disable_brush
 */
bool imel_disable_brush (void)
{
 return_var_if_fail (global_brush, false);
 
 imel_image_free (global_brush);
 global_brush = NULL;
 
 return true;
}

bool imel_printf_debug (const char *function, const char *filename, 
                        const char *error_level, char *__format, ...)
{
 va_list list;

#ifdef debug_enable
 fprintf (stderr, "Debug (%d): ", getpid ());
 
 if ( function )
      fprintf (stderr, "%s: ", function);
 
 if ( filename )
      fprintf (stderr, "%s: ", filename);
 
 if ( error_level ) 
      fprintf (stderr, "%s: ", error_level);
 
 if ( __format ) {
      va_start (list, __format);
      vfprintf (stderr, __format, list);
      va_end (list);
 }
 else fprintf (stderr, "(none)");
 
 fputc ('\n', stderr);
 
 return true;
#endif

#ifndef debug_enable
 return false;
#endif
}

