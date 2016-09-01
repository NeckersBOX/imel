/*
 * "point.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
#include <math.h>
#include "header.h"
/**
 * @file point.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to operate with points
 */
 
#ifndef DOXYGEN_IGNORE_DOC

extern void imel_pixel_copy (ImelPixel *, ImelPixel);
extern bool check_size (ImelImage *image, ImelSize xy, bool hw);
extern double imel_value_convert (ImelValue from_value, double value, ImelValue to_value, ...);

#endif

static ImelPoint *__imel_point_set_new (double x, double y, ImelPixel pixel)
{
 ImelPoint *p;
 
 p = (ImelPoint *) malloc (sizeof (ImelPoint));
 p->x = (ImelSize) x;
 p->y = (ImelSize) y;
 p->pixel.red = pixel.red;
 p->pixel.green = pixel.green;
 p->pixel.blue = pixel.blue;
 p->pixel.level = pixel.level;
 
 return p;
}

static void double_swap (double *a, double *b)
{
 double c = *a;

 *a = *b;
 *b = c;
}

/**
 * @brief Make a new point
 * 
 * This function makes a point at coordinate \f$(x,y)\in image\f$ with
 * a chosen color and level.
 * 
 * @param image Reference image
 * @param x Point x coordinate
 * @param y Point y coordinate
 * @param pixel Point color and level
 * @return A new #ImelPoint type or NULL on error
 */
ImelPoint *imel_point_new (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel)
{
 ImelPoint *l_point;

 return_var_if_fail (image, NULL);

 if ( !check_size (image, x, true) || !check_size (image, y, false) ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_point_new: warning: %s\n",
                       getpid (), "the height, or width, of rect > height, or width, of image");
#endif
      return NULL;
 }

 l_point = (ImelPoint *) malloc (sizeof (ImelPoint));
 l_point->x = x;
 l_point->y = y;
 l_point->pixel.red = pixel.red;
 l_point->pixel.green = pixel.green;
 l_point->pixel.blue = pixel.blue;
 l_point->pixel.level = pixel.level;

 return l_point;
}

/**
 * @brief Free the memory allocated for the point
 * 
 * @param point Point to free
 * @note Same as <tt>free (point)</tt>
 */
void imel_point_free (ImelPoint *point)
{
 return_if_fail (point);

 free (point);
}

/**
 * @brief Free the memory allocated for an array of points
 * 
 * @param points NULL-terminated #ImelPoint array
 */
void imel_point_array_free (ImelPoint **points)
{
 ImelSize i;

 return_if_fail (points);

 for ( i = 0; points[i]; free (points[i]), i++ );
 free (points);
}

/**
 * @brief Get a reference point from an image
 * 
 * @param image Reference image
 * @param x Point x coordinate
 * @param y Point y coordinate
 * @return A new #ImelPoint type with a pixel set equal to the image pixel
 * at coordinate \f$(x,y)\f$.
 */
ImelPoint *imel_point_get_point_from_image (ImelImage *image, ImelSize x, ImelSize y)
{
 ImelPoint *l_point;

 return_var_if_fail (image, NULL);

 if ( x > (image->width - 1) || y > (image->height - 1) ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_point_new: warning: %s\n",
                       getpid (), "the height, or width, of rect > height, or width, of image");
#endif
      return NULL;
 }

 l_point = (ImelPoint *) malloc (sizeof (ImelPoint));
 l_point->x = x;
 l_point->y = y;
 l_point->pixel.red = image->pixel[y][x].red;
 l_point->pixel.green = image->pixel[y][x].green;
 l_point->pixel.blue = image->pixel[y][x].blue;
 l_point->pixel.level = image->pixel[y][x].level;

 return l_point;
}

/**
 * @brief Get a line points
 * 
 * This function get all points in the line from coordinate \f$(\_x_1,\_y_1)\f$
 * to coordinate \f$(\_x_2,\_y_2)\f$. You can get the line width and height 
 * respectively from @p lx and @p ly argument. 
 * 
 * @param _x1 Start line x coordinate
 * @param _y1 Start line y coordinate
 * @param _x2 End line x coordinate
 * @param _y2 End line y coordinate
 * @param lx NULL or if passed, this function put inside it the line width
 * @param ly NULL or if passed, this function put inside it the line height
 * @param value_type Type of value passed as @p value
 * @param value Number of points you want to get from the line. The points will be
 * uniformely distributed.
 * @return A NULL-terminated array with the line points.
 * 
 * @see ImelValue
 * @see imel_point_array_free
 */
ImelPoint **imel_point_get_from_line (ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2, long int *lx, 
                                      long int *ly, ImelValue value_type, double value)
{
 ImelPoint **points;
 static ImelPixel alpha = { 0, 0, 0, -255 };
 double dx, dy, px, py, pixel, jmp;
 double x[2] = { (double) _x1, (double) _x2 };
 double y[2] = { (double) _y1, (double) _y2 };
 long int j = -1;
  
 if ( _y1 == _y2 && _x1 == _x2 ) {
      if ( lx )
           *lx = 0;
      if ( ly ) 
           *ly = 0;
           
      points = (ImelPoint **) malloc (sizeof (ImelPoint *) << 1);
      *points = __imel_point_set_new (_x1, _y1, alpha);
      points[1] = NULL;
      
      return points;
 }
 
 if ( (_x1 > _x2 && _y1 > _y2) ) {
      double_swap (&(y[0]), &(y[1]));
      double_swap (&(x[0]), &(x[1]));
 }

 dx = x[1] - x[0];
 dy = y[1] - y[0];
 
 if ( lx )
      *lx = abs ((int) dx);
 if ( ly )
      *ly = abs ((int) dy);
 
 if ( abs ((int) dx) >= abs ((int) dy) ) {
      pixel = imel_value_convert (value_type, value, IMEL_VALUE_PIXEL, (double) abs (dx));
      if ( pixel >= abs ((int) dx) )
           jmp = 1.0f;
      else jmp = ((double) abs ((int) dx)) / pixel;
      
      if ( !jmp )
            jmp = 1;

      points = (ImelPoint **) malloc (sizeof (ImelPoint *) * (1 + (abs ((int) dx) / jmp)));
           
      if ( x[0] > x[1] ) {
           double_swap (&(y[0]), &(y[1]));
           double_swap (&(x[0]), &(x[1]));
      }
      
      for ( px = x[0]; px < x[1]; px += jmp ) {
            py = ((px - x[0]) / dx) * dy + y[0];
            
            points[++j] = __imel_point_set_new (px, py, alpha);
      }
      points[++j] = NULL;

      return points;
 }

 pixel = imel_value_convert (value_type, value, IMEL_VALUE_PIXEL, (double) abs ((int) dy));
 if ( pixel >= abs ((int) dy) )
      jmp = 1.0f;
 else jmp = ((double) abs ((int) dy)) / pixel;
      
 if ( !jmp )
       jmp = 1;

 points = (ImelPoint **) malloc (sizeof (ImelPoint *) * (1 + (abs ((int) dy) / jmp)));
           
 if ( y[0] > y[1] ) {
      double_swap (&(y[0]), &(y[1]));
      double_swap (&(x[0]), &(x[1]));
 }

 for ( py = y[0]; py < y[1]; py += jmp ) {
       px = ((py - y[0]) / dy) * dx + x[0];

       points[++j] = __imel_point_set_new (px, py, alpha);
 }
 points[++j] = NULL;

 return points;
}

/**
 * @brief Get a regular shape points
 * 
 * This function get the points from a regular shape with center in coordinate
 * \f$(x,y)\f$, @p r radius, @p v vertices and an angle of @p start angle.
 * 
 * @param x Center x coordinate
 * @param y Center y coordinate
 * @param r Radius
 * @param v Vertices
 * @param start_angle Start angle in radians
 * @return A NULL-terminated array with the regular shape points
 * 
 * @see imel_point_array_free
 * @see RAD_TO_DEG
 * @see DEG_TO_RAD
 */
ImelPoint **imel_point_get_from_reg_shape (ImelSize x, ImelSize y, ImelSize r, long v, double start_angle)
{
 int j = -1;
 ImelPoint **points;
 static ImelPixel alpha = { 0, 0, 0, -255 };
 double increment = 6.283185307f / ((double) v ? v : 1.f);
 
 return_var_if_fail (v > 2 && start_angle < 3.141592654f 
                     && start_angle > -3.141592654f, NULL);
 
 points = (ImelPoint **) malloc (sizeof (ImelPoint *) * (v + 2));
 
 *points = __imel_point_set_new (((double) x) + (((double) r) * cos (start_angle)), 
                                 ((double) y) - (((double) r) * sin (start_angle)), alpha);
	
 for ( start_angle += increment ; ++j < v ; start_angle += increment )
       points[j] = __imel_point_set_new (((double) x) + (((double) r) * cos (start_angle)), 
                                         ((double) y) - (((double) r) * sin (start_angle)), alpha);
 points[j] = NULL;
 
 return points;
} 

/**
 * @brief Get the darkest point of an image
 * 
 * @param image Reference image
 * @return A new #ImelPoint type
 *
 * @see imel_point_get_brightest_point
 * @see imel_point_get_darkest_points
 */
ImelPoint *imel_point_get_darkest_point (ImelImage *image)
{
 ImelSize y, x, dx, dy;
 double light, min_light = 255.f;
 
 return_var_if_fail (image, NULL);
 
 for ( y = 0; y < image->height; y++ ) {
      for ( x = 0; x < image->width; x++ ) {
            light = (0.30f * ((double) image->pixel[y][x].red)) 
                  + (0.59f * ((double) image->pixel[y][x].green))
                  + (0.11f * ((double) image->pixel[y][x].blue));
                  
            if ( light < min_light ) {
                 min_light = light;
                 dx = x;
                 dy = y;
            }
      }
 }
 
 return imel_point_get_point_from_image (image, dx, dy);
}

/**
 * @brief Get the darkest points of an image
 * 
 * This function return the darkest point in @p image and
 * if are more then one point return a reference to all occourrence of
 * the darkest point.
 * 
 * @param image Reference image
 * @return A new #ImelPoint type
 *
 * @see imel_point_get_brightest_point
 * @see imel_point_get_darkest_point
 */
ImelPoint **imel_point_get_darkest_points (ImelImage *image)
{
 ImelPoint **points;
 ImelSize y, x, dx, dy, j = 1;
 double light, min_light = 255.f;
 
 return_var_if_fail (image, NULL);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             light = (0.30f * ((double) image->pixel[y][x].red)) 
                   + (0.59f * ((double) image->pixel[y][x].green))
                   + (0.11f * ((double) image->pixel[y][x].blue));
                   
             if ( light < min_light )
                  min_light = light;
       }
 }
 
 points = (ImelPoint **) malloc (j * sizeof (ImelPoint *));
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             light = (0.30f * ((double) image->pixel[y][x].red)) 
                   + (0.59f * ((double) image->pixel[y][x].green))
                   + (0.11f * ((double) image->pixel[y][x].blue));
             
             if ( light == min_light ) {
                  points[j - 1] = imel_point_get_point_from_image (image, x, y);
                  points = (ImelPoint **) realloc (points, ++j * sizeof (ImelPoint *));
             }
       }
 }
 points[j - 1] = NULL;
 
 return points;
}

/**
 * @brief Get the brightest point of an image
 * 
 * @param image Reference image
 * @return A new #ImelPoint type
 *
 * @see imel_point_get_brightest_points
 * @see imel_point_get_darkest_point
 */
ImelPoint *imel_point_get_brightest_point (ImelImage *image)
{
 ImelSize y, x, dx, dy;
 double light, min_light = 0.f;
 
 return_var_if_fail (image, NULL);
 
 for ( y = 0; y < image->height; y++ ) {
      for ( x = 0; x < image->width; x++ ) {
            light = (0.30f * ((double) image->pixel[y][x].red)) 
                  + (0.59f * ((double) image->pixel[y][x].green))
                  + (0.11f * ((double) image->pixel[y][x].blue));
                  
            if ( light > min_light ) {
                 min_light = light;
                 dx = x;
                 dy = y;
            }
      }
 }
 
 return imel_point_get_point_from_image (image, dx, dy);
}

/**
 * @brief Get the brightest points of an image
 * 
 * This function return the brightest point in @p image and
 * if are more then one point return a reference to all occourrence of
 * the brightest point.
 * 
 * @param image Reference image
 * @return A new #ImelPoint type
 *
 * @see imel_point_get_brightest_point
 * @see imel_point_get_darkest_points
 */
ImelPoint **imel_point_get_brightest_points (ImelImage *image)
{
 ImelPoint **points;
 ImelSize y, x, dx, dy, j = 1;
 double light, min_light = 0.f;
 
 return_var_if_fail (image, NULL);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             light = (0.30f * ((double) image->pixel[y][x].red)) 
                   + (0.59f * ((double) image->pixel[y][x].green))
                   + (0.11f * ((double) image->pixel[y][x].blue));
                   
             if ( light > min_light )
                  min_light = light;
       }
 }
 
 points = (ImelPoint **) malloc (j * sizeof (ImelPoint *));
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             light = (0.30f * ((double) image->pixel[y][x].red)) 
                   + (0.59f * ((double) image->pixel[y][x].green))
                   + (0.11f * ((double) image->pixel[y][x].blue));
             
             if ( light == min_light ) {
                  points[j - 1] = imel_point_get_point_from_image (image, x, y);
                  points = (ImelPoint **) realloc (points, ++j * sizeof (ImelPoint *));
             }
       }
 }
 points[j - 1] = NULL;
 
 return points;
}
