/*
 * "image_fill.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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


#include "header.h"
/**
 * @file image_fill.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to fill areas of an image
 */
 
#ifndef DOXYGEN_IGNORE_DOC

extern void         imel_draw_point                           (ImelImage *, ImelSize, ImelSize, ImelPixel);
extern ImelPoint   *imel_point_new                            (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel);
extern bool         imel_pixel_compare                        (ImelPixel a, ImelPixel b, ImelSize tollerance);
extern bool         imel_pixel_compare_level                  (ImelLevel a, ImelLevel b, ImelSize tollerance);
extern ImelPixel    imel_pixel_new                            (ImelColor red, ImelColor green, ImelColor blue, 
                                                               ImelLevel level);

#endif

static void         __fill_north                              (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                                                               ImelSize tollerance, ImelRef reference);
static void         __fill_south                              (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                                                               ImelSize tollerance, ImelRef reference);
static void         __fill_west                               (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                                                               ImelSize tollerance, ImelRef reference);
static void         __fill_east                               (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                                                               ImelSize tollerance, ImelRef reference);

static void __fill_north (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                          ImelSize tollerance, ImelRef reference)
{ 
 ImelPoint __pvt_pos = { position->x, position->y, position->pixel };
 
 if ( !position->y ) 
      return;

 __pvt_pos.y--;
 
 if ( (reference == IMEL_REF_COLOR && !imel_pixel_compare (image->pixel[__pvt_pos.y][__pvt_pos.x], target_color, tollerance))
   || (reference == IMEL_REF_LEVEL && !imel_pixel_compare_level (image->pixel[__pvt_pos.y][__pvt_pos.x].level, 
                                                                 target_color.level, tollerance)) ) {
      return;
 }

 if ( reference == IMEL_REF_LEVEL )
      image->pixel[__pvt_pos.y][__pvt_pos.x].level = position->pixel.level;
 else imel_draw_point (image,__pvt_pos.x, __pvt_pos.y, position->pixel);
 
 __fill_north (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_west (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_east (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_south (image, &__pvt_pos, target_color, tollerance, reference);
}

static void __fill_south (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                          ImelSize tollerance, ImelRef reference)
{
 ImelPoint __pvt_pos = { position->x, position->y, position->pixel };
 
 if ( position->y >= (image->height - 1) ) 
      return;

 __pvt_pos.y++;
 
 if ( (reference == IMEL_REF_COLOR && !imel_pixel_compare (image->pixel[__pvt_pos.y][__pvt_pos.x], target_color, tollerance))
   || (reference == IMEL_REF_LEVEL && !imel_pixel_compare_level (image->pixel[__pvt_pos.y][__pvt_pos.x].level, 
                                                                 target_color.level, tollerance)) ) {
      return;
 }
 
 if ( reference == IMEL_REF_LEVEL )
      image->pixel[__pvt_pos.y][__pvt_pos.x].level = position->pixel.level;
 else imel_draw_point (image,__pvt_pos.x, __pvt_pos.y, position->pixel);
                     
 __fill_south (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_west (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_east (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_north (image, &__pvt_pos, target_color, tollerance, reference);
}

static void __fill_west (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                         ImelSize tollerance, ImelRef reference)
{
 ImelPoint __pvt_pos = { position->x, position->y, position->pixel };
 
 if ( !position->x ) 
      return;

 __pvt_pos.x--;
 
 if ( (reference == IMEL_REF_COLOR && !imel_pixel_compare (image->pixel[__pvt_pos.y][__pvt_pos.x], target_color, tollerance))
   || (reference == IMEL_REF_LEVEL && !imel_pixel_compare_level (image->pixel[__pvt_pos.y][__pvt_pos.x].level, 
                                                                 target_color.level, tollerance)) ) {
      return;
 }
 
 if ( reference == IMEL_REF_LEVEL )
      image->pixel[__pvt_pos.y][__pvt_pos.x].level = position->pixel.level;
 else imel_draw_point (image,__pvt_pos.x, __pvt_pos.y, position->pixel);
 
 __fill_west (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_south (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_north (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_east (image, &__pvt_pos, target_color, tollerance, reference);
}

static void __fill_east (ImelImage *image, ImelPoint *position, ImelPixel target_color, 
                         ImelSize tollerance, ImelRef reference)
{
 ImelPoint __pvt_pos = { position->x, position->y, position->pixel };
 
 if ( position->x >= (image->width - 1) ) 
      return;

 __pvt_pos.x++;
 
 if ( (reference == IMEL_REF_COLOR && !imel_pixel_compare (image->pixel[__pvt_pos.y][__pvt_pos.x], target_color, tollerance))
   || (reference == IMEL_REF_LEVEL && !imel_pixel_compare_level (image->pixel[__pvt_pos.y][__pvt_pos.x].level, 
                                                                 target_color.level, tollerance)) ) {
      return;
 }
 
 if ( reference == IMEL_REF_LEVEL )
      image->pixel[__pvt_pos.y][__pvt_pos.x].level = position->pixel.level;
 else imel_draw_point (image,__pvt_pos.x, __pvt_pos.y, position->pixel);
                     
 __fill_east (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_north (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_south (image, &__pvt_pos, target_color, tollerance, reference);
 __fill_west (image, &__pvt_pos, target_color, tollerance, reference);
}

/**
 * @brief Fill a specified color area with another color
 * 
 * This function fill a color area with another color.
 * 
 * @param image Image where area to fill exist.
 * @param point Coordinate of the position to start to fill and the new color value.
 * @param tollerance Tollerance to find the next pixel to fill
 * 
 * @see imel_point_new
 */ 
void imel_image_fill_color_with_color (ImelImage *image, ImelPoint *point, ImelSize tollerance)
{
 ImelPixel _static_color;

 return_if_fail (image && point);
 
 if ( !(point->y < image->height && point->x < image->width) )
      return;
      
 _static_color = imel_pixel_new (image->pixel[point->y][point->x].red,
                                 image->pixel[point->y][point->x].green,
                                 image->pixel[point->y][point->x].blue,
                                 image->pixel[point->y][point->x].level);
                                 
 __fill_north (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_south (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_west (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_east (image, point, _static_color, tollerance, IMEL_REF_COLOR);
}

/**
 * @brief Fill a specified level area with another color
 * 
 * This function fill a specified level area with another color.
 * 
 * @param image Image where area to fill exist.
 * @param point Coordinate of the position to start to fill and the new color value.
 * @param tollerance Tollerance to find the next pixel to fill
 * 
 * @see imel_point_new
 */ 
void imel_image_fill_level_with_color (ImelImage *image, ImelPoint *point, ImelSize tollerance)
{
 ImelPixel _static_color;
 
 return_if_fail (image && point);
 
 if ( !(point->y < image->height && point->x < image->width) )
      return;
 
 _static_color = imel_pixel_new (image->pixel[point->y][point->x].red,
                                 image->pixel[point->y][point->x].green,
                                 image->pixel[point->y][point->x].blue,
                                 image->pixel[point->y][point->x].level);
                                 
 __fill_north (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_south (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_west (image, point, _static_color, tollerance, IMEL_REF_COLOR);
 __fill_east (image, point, _static_color, tollerance, IMEL_REF_COLOR);
}

/**
 * @brief Fill a specified color area with another level
 * 
 * This function fill a specified color area with another level.
 * 
 * @param image Image where area to fill exist.
 * @param point Coordinate of the position to start to fill and new level value.
 * @param tollerance Tollerance to find the next pixel to fill
 * 
 * @see imel_point_new
 */ 
void imel_image_fill_color_with_level (ImelImage *image, ImelPoint *point, ImelSize tollerance)
{
 ImelPixel _static_color;
 
 return_if_fail (image && point);
 
 if ( !(point->y < image->height && point->x < image->width) )
      return;
 
 _static_color = imel_pixel_new (image->pixel[point->y][point->x].red,
                                 image->pixel[point->y][point->x].green,
                                 image->pixel[point->y][point->x].blue,
                                 image->pixel[point->y][point->x].level);
 
 __fill_north (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_south (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_west (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_east (image, point, _static_color, tollerance, IMEL_REF_LEVEL);

}

/**
 * @brief Fill a specified level area with another level
 * 
 * This function fill a specified level area with another level.
 * 
 * @param image Image where area to fill exist.
 * @param point Coordinate of the position to start to fill and new level value.
 * @param tollerance Tollerance to find the next pixel to fill
 * 
 * @see imel_point_new
 */ 
void imel_image_fill_level_with_level (ImelImage *image, ImelPoint *point, ImelSize tollerance)
{
 ImelPixel _static_color;
 
 return_if_fail (image && point);
 
 if ( !(point->y < image->height && point->x < image->width) )
      return;
 
 _static_color = imel_pixel_new (image->pixel[point->y][point->x].red,
                                 image->pixel[point->y][point->x].green,
                                 image->pixel[point->y][point->x].blue,
                                 image->pixel[point->y][point->x].level);
                                 

 __fill_north (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_south (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_west (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
 __fill_east (image, point, _static_color, tollerance, IMEL_REF_LEVEL);
}
