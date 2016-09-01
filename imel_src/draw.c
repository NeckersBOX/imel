/*
 * "draw.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
 *              Simone Scarinzi ( scame@hotmail.it )
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
#include <stdlib.h>
#include <math.h>
#include "header.h"

/**
 * @file draw.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to draw.
 *
 * These functions allow you to draw lines, curves, circles and much more
 * inside an image.
 * @note Most of the draw actions is made by imel_pixel_copy ()
 * @note Angle can be set from 0 to 360 degrees
 * @see imel_pixel_copy 
 */
 
#ifndef PI
# define PI (3.141592653589793)
#endif

#ifndef DOXYGEN_IGNORE_DOC

extern void       imel_pixel_set_from_pixel (ImelPixel *, ImelPixel);
extern void       imel_pixel_copy           (ImelPixel *, ImelPixel);
extern ImelPixel  imel_pixel_new            (ImelColor, ImelColor, ImelColor, ImelLevel);
extern ImelPixel  imel_pixel_union          (ImelPixel a, ImelPixel b, unsigned char _opacity);
extern void       imel_image_insert_image   (ImelImage *dest, ImelImage *src, ImelSize sx, ImelSize sy);

#endif

void imel_draw_circle               (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, ImelPixel pxl);
bool imel_draw_partial_reg_shape    (ImelImage *image, ImelSize x, ImelSize y, ImelSize r, long v,  
                                     short p , double start_angle, ImelPixel pxl);
bool imel_draw_arch                 (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                                     double start_angle, double end_angle, ImelPixel pxl);
bool imel_draw_filled_arch          (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                                     double start_angle, double end_angle, ImelPixel pxl);
                            
bool check_size                     (ImelImage *image, ImelSize xy, bool hw);
bool imel_draw_line                 (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, ImelPixel pixel);

static void size_swap (ImelSize *a, ImelSize *b)
{
 ImelSize c = *a;

 *a = *b;
 *b = c;
}

static void long_int_swap (long int *a, long int *b)
{
 long int c = *a;

 *a = *b;
 *b = c;
}

static void double_swap (double *a, double *b)
{
 double c = *a;
 
 *a = *b;
 *b = c;
}

/**
 * @brief Draw a single point in an image
 * 
 * This function draw a point in @p image at coordinate \f$(x,y)\f$ with 
 * color and level specified in @p pixel.
 * 
 * @param image Image where draw the point
 * @param x Coordinate x of the point
 * @param y Coordinate y of the point
 * @param pixel Color and level of the point
 */
void imel_draw_point (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel)
{ 
 return_if_fail (image);
 
 if ( y >= image->height ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_draw_point: warning: %s%d%s%d%s\n",
                       getpid (), "the height (", y,
                       ") > height of image (", image->height, ")");
#endif
      return;
 }

 if ( x >= image->width ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_draw_point: warning: %s%d%s%d%s\n",
                       getpid (), "the width (", x,
                       ") > width of image (", image->width, ")");
#endif
      return;
 }

 imel_pixel_copy (&(image->pixel[y][x]), pixel);
}

static void __imel_draw_point (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel)
{
 extern ImelImage *global_brush;
 
 return_if_fail (image);
 
 if ( global_brush ) {
      imel_image_insert_image (image, global_brush, x, y);
      return;
 }
 
 if ( y >= image->height ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): __imel_draw_point: warning: %s%d%s%d%s\n",
                       getpid (), "the height (", y,
                       ") > height of image (", image->height, ")");
#endif
      return;
 }

 if ( x >= image->width ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): __imel_draw_point: warning: %s%d%s%d%s\n",
                       getpid (), "the width (", x,
                       ") > width of image (", image->width, ")");
#endif
      return;
 }

 imel_pixel_copy (&(image->pixel[y][x]), pixel);
}

void __imel_draw_put_pixel (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel)
{
 return_if_fail (image);

 if ( y >= image->height || x >= image->width )
      return;

 imel_pixel_set_from_pixel (&(image->pixel[y][x]), pixel);
}

/**
 * @brief Draw a filled circle
 * 
 * This function draw a circle filled with color and level passed in @p pxl at
 * coordinate \f$(x,y)\f$ with a radius of @p radius pixels.
 * 
 * @param image Image where draw the circle
 * @param x Coordinate x of the circle center
 * @param y Coordinate y of the circle center
 * @param radius Radius of the circle in pixels
 * @param pxl Color and level of the circle
 */
void imel_draw_filled_circle (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, ImelPixel pxl)
{
 ImelSize sx, ex = x + radius + 1, sy = y - radius - 1, ey = y + radius + 1;

 return_if_fail (image);
  
 while ( ++sy < ey )
     for ( sx = x - radius; sx < ex; sx++ )
           if ( (pow (((int) sx) - ((int) x), 2) + pow (((int) sy) - ((int) y), 2)) < pow (radius, 2) )
                __imel_draw_point (image, sx, sy, pxl);
}

/**
 * @brief Draw a rectangle ( filled or not )
 * 
 * This function draw a rectangle that can be filled inside an @p image 
 * from coordinate \f$(x_1,y_1)\f$ to coordinate \f$(x_2,y_2)\f$ with color
 * and level passed in @p pixel.
 * 
 * @param image Image where draw the rectangle
 * @param x1 Start x coordinate of the rectangle
 * @param y1 Start y coordinate of the rectangle
 * @param x2 End x coordinate of the rectangle
 * @param y2 End y coordinate of the rectangle
 * @param pixel Color and level of the rectangle
 * @param fill TRUE for filled rectangle, else FALSE
 */
void imel_draw_rect (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, ImelPixel pixel, bool fill)
{
 ImelSize h;

 return_if_fail (image);

 if ( y1 > y2 ) {
      size_swap (&x1, &x2);
      size_swap (&y1, &y2);
 }

 imel_draw_line (image, x1, y1, x2, y1, pixel);
 imel_draw_line (image, x1, y1, x1, y2, pixel);
 imel_draw_line (image, x2, y1, x2, y2, pixel);
 imel_draw_line (image, x1, y2, x2 + 1, y2, pixel);
      
 if ( !fill ) {
      return;
 }

 for ( h = y1; h < y2; h++ )
       imel_draw_line (image, x1, h, x2, h, pixel);
}

/**
 * @brief Draw a rectangle with rounded angles ( filled or not )
 * 
 * This function draw a rectangle that can be filled inside an @p image
 * from coordinate \f$(x_1,y_1)\f$ to coordinate \f$(x_2,y_2)\f$ with color
 * and level passed in @p pixel and rounded angles.
 * 
 * @param image Image where draw the rectangle
 * @param x1 Start x coordinate of the rectangle
 * @param y1 Start y coordinate of the rectangle
 * @param x2 End x coordinate of the rectangle
 * @param y2 End y coordinate of the rectangle
 * @param radius Angles radius in pixels.
 * @param pixel Color and level of the rectangle
 * @param fill TRUE for filled rectangle, else FALSE
 * @return FALSE if @p image or @p radius values aren't valid, else TRUE 
 */
bool imel_draw_rect_with_rounded_angles (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2,
                                         ImelSize radius, ImelPixel pixel, bool fill)
{
 uint32_t x[2] = { x1, x2 }, y[2] = { y1, y2 };
 
 return_var_if_fail (image, false);
 
 if ( ((x[1] - x[0]) < (radius << 1)) || ((y[1] - y[0]) < (radius << 1)) ) 
      return false;
 
 if ( !fill ) {
      imel_draw_arch (image, x[0] + radius, y[0] + radius, radius, DEG_TO_RAD (180), DEG_TO_RAD (270), pixel);
      imel_draw_arch (image, x[1] - radius, y[0] + radius, radius, DEG_TO_RAD (270), DEG_TO_RAD (360), pixel);
      imel_draw_arch (image, x[0] + radius, y[1] - radius, radius, DEG_TO_RAD (90), DEG_TO_RAD (180), pixel);
      imel_draw_arch (image, x[1] - radius, y[1] - radius, radius, DEG_TO_RAD (0), DEG_TO_RAD (90), pixel);
 
      imel_draw_line (image, x[0] + (radius - 3), y[0], x[1] - radius, y[0], pixel);
      imel_draw_line (image, x[0] + radius, y[1], x[1] - radius, y[1], pixel);
      imel_draw_line (image, x[0], y[0] + radius, x[0], y[1] - (radius - 3), pixel);
      imel_draw_line (image, x[1], y[0] + (radius - 3), x[1], y[1] - radius, pixel);
      
      return true;
 }
 
 imel_draw_filled_arch (image, x[0] + radius, y[0] + radius, radius, DEG_TO_RAD (180), DEG_TO_RAD (270), pixel);
 imel_draw_filled_arch (image, x[1] - radius, y[0] + radius, radius, DEG_TO_RAD (270), DEG_TO_RAD (360), pixel);
 imel_draw_filled_arch (image, x[0] + radius, y[1] - radius, radius, DEG_TO_RAD (90), DEG_TO_RAD (180), pixel);
 imel_draw_filled_arch (image, x[1] - radius, y[1] - radius, radius, DEG_TO_RAD (0), DEG_TO_RAD (90), pixel);
 
 imel_draw_rect (image, x[0] + (radius - 3), y[0],  x[1] - radius, y[1], pixel, true);
 imel_draw_rect (image, x[0], y[0] + (radius - 3), x[1],  y[1] - (radius - 3), pixel, true);
 return true;
}

/**
 * @brief Draw more point at the same time
 * 
 * This function draw a list of @p points in the @p image
 * 
 * @param image Image where draw the points
 * @param n_points Number of points to draw or 0 if @p points is a NULL-terminated
 * array and you want to draw its all.
 * @param points Points to draw
 */
void imel_draw_point_from_array (ImelImage *image, ImelSize n_points, ImelPoint **points)
{
 ImelSize i;

 return_if_fail (image && points);

 if ( !n_points ) {
      while ( points[n_points++] );
      n_points--;
 }
 
 for ( i = 0; i < n_points; i++ )
       __imel_draw_point (image, points[i]->x, points[i]->y, points[i]->pixel);
}

/**
 * @brief Draw an ellipse
 * 
 * This function draw an ellipse in @p image with center in coordinate
 * \f$(x,y)\f$ with @p a width and @p b height. 
 * 
 * @param image Image where draw the ellipse
 * @param x Coordinate x of the ellipse center
 * @param y Coordinate y of the ellipse center
 * @param a Length of the horizontal axis
 * @param b Length of the vertical axis
 * @param pxl Color and level of the ellipse
 */
void imel_draw_ellipse (ImelImage *image, ImelSize x, ImelSize y, double a, double b, ImelPixel pxl) 
{ 
 double angle; 
 ImelSize Ox = x + a, Oy = y, X, Y; 
  
 return_if_fail (image) ; 
 
 if ( a == b ) {
      imel_draw_circle (image, x, y, (ImelSize) a, pxl);
      return;
 }
 
 __imel_draw_point (image, Ox - 1, y - 1, pxl);
 
 for ( angle = 0.00174532; angle < 6.281439987; angle += 0.00174532, Ox = X, Oy = Y ) { 
         X = (ImelSize) (((double) x) + a * cos (angle)); 
         Y = (ImelSize) (((double) y) + b * sin (angle)); 

         imel_draw_line (image, X, Y, Ox, Oy, pxl); 
 } 
} 

/**
 * @brief Draw a filled ellipse
 * 
 * This function draw a filled ellipse in @p image with center in coordinate
 * \f$(x,y)\f$ with @p a width and @p b height. 
 * 
 * @param image Image where draw the ellipse
 * @param x Coordinate x of the ellipse center
 * @param y Coordinate y of the ellipse center
 * @param a Length of the horizontal axis
 * @param b Length of the vertical axis
 * @param pxl Color and level of the ellipse
 */
void imel_draw_filled_ellipse (ImelImage *image, ImelSize x, ImelSize y, double a, double b, ImelPixel pxl)
{
 double angle; 
 ImelSize Ox = x + a, Oy = y, X, Y; 
  
 return_if_fail (image) ; 
 
 if ( a == b ) {
      imel_draw_filled_circle (image, x, y, (ImelSize) a, pxl);
      return;
 }
 
 __imel_draw_point (image, Ox - 1, y - 1, pxl);
 
 for ( angle = 0.00174532; angle < 6.281439987; angle += 0.00174532, Ox = X, Oy = Y ) { 
         X = (ImelSize) (((double) x) + a * cos (angle)); 
         Y = (ImelSize) (((double) y) + b * sin (angle)); 

         imel_draw_rect (image, X, y, Ox, Oy, pxl, true); 
 } 
}

bool check_size (ImelImage *image, ImelSize xy, bool hw)
{
 return_var_if_fail (image, true);

 if ( hw && xy <= image->width )
      return true;
 if ( !hw && xy <= image->height )
      return true;

 return false;
}

/**
 * @brief Draw a line with gradient
 * 
 * This function draw a line in @p image from coordinate \f$(\_x_1,\_y_1)\f$ 
 * to coordinate \f$(\_x_2,\_y_2)\f$ with a color and level gradient from @p start
 * to @p end.
 * 
 * @param image Image where draw the line
 * @param _x1 Start x coordinate
 * @param _y1 Start y coordinate
 * @param _x2 End x coordinate
 * @param _y2 End y coordinate
 * @param start Start color and level of the line
 * @param end End color and level of the line
 * @return FALSE if @p image isn't a valid image, else TRUE
 * @see imel_draw_line
 */
bool imel_draw_gradient_line (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, 
                              ImelSize _y2, ImelPixel start, ImelPixel end)
{
 long int x[2] = { (long int) _x1, (long int) _x2 };
 long int y[2] = { (long int) _y1, (long int) _y2 };
 double dx, dy, px, py;
 long int step = -1;
 ImelPixel pxl, rpxl;
 
 return_var_if_fail (image, false);

 if ( (_y1 == _y2 && _x1 > _x2) || (_x1 == _x2 && _y1 > _y2)
   || ((((_x1 * 100) / (_x2 ? _x2 : 1)) <= ((_y1 * 100) / (_y2 ? _y2 : 1))) && _x1 > _x2) ) {
           long_int_swap (&(y[0]), &(y[1]));
           long_int_swap (&(x[0]), &(x[1]));
 }

 if ( _y1 == _y2 && _x1 > _x2 )
      x[1]++;

 dx = x[1] - x[0];
 dy = y[1] - y[0];
 
 if ( !dx && dx == dy )
      __imel_draw_point (image, x[0], y[0], start);
      
 if ( dx >= dy ) {
      step = (end.level - start.level) / dx;
      if ( step < 0 )
           rpxl.level = start.level - step;
      else rpxl.level = start.level;
      
      for ( px = x[0]; px < x[1]; px++ ) {
            pxl = imel_pixel_union (start, end, (255 * py) / y[1]);
            rpxl = imel_pixel_new (pxl.red, pxl.green, pxl.blue, rpxl.level + step);
            
            py = ((px - x[0]) / dx) * dy + y[0];

            __imel_draw_point (image, px, py, rpxl);
      }

      return true;
 }

 step = (end.level - start.level) / dy;
 if ( step < 0 )
      rpxl.level = start.level - step;
 else rpxl.level = start.level;
 
 for ( py = y[0]; py < y[1]; py++ ) {
       pxl = imel_pixel_union (start, end, (255 * py) / y[1]);
       rpxl = imel_pixel_new (pxl.red, pxl.green, pxl.blue, rpxl.level + step);
       
       px = ((py - y[0]) / dy) * dx + x[0];

       __imel_draw_point (image, px, py, rpxl);
 }

 return true;
}

/**
 * @brief Draw a line
 * 
 * This function draw a line in @p image from coordinate \f$(\_sx,\_sy)\f$ to 
 * coordinate \f$(\_ex,\_ey)\f$ with a color and level passed in @p pixel
 * 
 * @param image Image where draw the line
 * @param _sx Start x coordinate
 * @param _sy Start y coordinate
 * @param _ex End x coordinate
 * @param _ey End y coordinate
 * @param pixel Color and level of the line
 * @return FALSE if @p image isn't a valid image, else TRUE
 */
bool imel_draw_line (ImelImage *image, ImelSize _sx, ImelSize _sy, ImelSize _ex, ImelSize _ey, ImelPixel pixel)
{
 double dx, dy, j, p;
 double sx = (double) _sx, sy = (double) _sy, ex = (double) _ex, ey = (double) _ey;
 
 return_var_if_fail (image, false);
 
 dx = sx - ex;
 dy = sy - ey;
 
 if ( _sy == _ey ) {
	  for ( j = _sx; j < _ex; j++ )
	        __imel_draw_point (image, j, _sy, pixel); 
	  
	  return true;
 }
 
 if ( _sx == _ex ) {
	  for ( j = _sy; j < _ey; j++ ) 
	        __imel_draw_point (image, _sx, j, pixel);
	  
	  return true;
 }
  
 if ( !dx || !dy ) {
      __imel_draw_point (image, ex, ey, pixel);
      __imel_draw_point (image, sx, sy, pixel);
      
      return true;
 }
 
 if ( abs (dx) > abs (dy) ) {
      if ( sx > ex ) {
           double_swap (&sx, &ex);
           double_swap (&sy, &ey);
      }
      
      for ( j = sx; j < ex; j++ ) {
            p = ((j - sx) / dx) * dy + sy;
            __imel_draw_point (image, j, p, pixel);
      }
      
      return true;
 }
 else {
      if ( sy > ey ) {
           double_swap (&sx, &ex);
           double_swap (&sy, &ey);
      }
      
      for ( j = sy; j < ey; j++ ) {
            p = ((j - sy) / dy) * dx + sx;
            __imel_draw_point (image, p, j, pixel);
      }
      
      return true;
 }
}

/**
 * @brief Draw a filled line
 * 
 * This function draw a line in @p image from coordinate \f$(\_x_1,\_y_1)\f$ 
 * to coordinate \f$(\_x_2,\_y_2)\f$ with a color and level passed in 
 * @p pixel, where each point of the line is linked to a coordinate 
 * \f$(ox,oy)\f$.
 * 
 * @param image Image where draw the line
 * @param _x1 Start x coordinate
 * @param _y1 Start y coordinate
 * @param _x2 End x coordinate
 * @param _y2 End y coordinate
 * @param ox Coordinate x of the common point
 * @param oy Coordinate y of the common point
 * @param pixel Color and level of the line
 * @return FALSE if @p image isn't a valid image, else TRUE
 */
bool imel_draw_filled_line (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2, 
                            ImelSize ox, ImelSize oy, ImelPixel pixel)
{
 long int x[2] = { (long int) _x1, (long int) _x2 };
 long int y[2] = { (long int) _y1, (long int) _y2 };
 double dx, dy, px, py;

 return_var_if_fail (image, false);
 
 if ( _y1 == _y2 && _x1 == _x2 ) {
      imel_draw_line (image, _y1, _x1, ox, oy, pixel);
      return true;
 }
 
 if ( (_x1 > _x2 && _y1 > _y2) ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }

 dx = x[1] - x[0];
 dy = y[1] - y[0];
 
 imel_draw_line (image, x[0], y[0], ox, oy, pixel);
 if ( abs (dx) >= abs (dy) ) {
      if ( x[0] > x[1] ) {
           long_int_swap (&(y[0]), &(y[1]));
           long_int_swap (&(x[0]), &(x[1]));
      }
      
      for ( px = x[0]; px < x[1]; px++ ) {
            py = ((px - x[0]) / dx) * dy + y[0];

            imel_draw_line (image, px, py, ox, oy, pixel);
      }

      return true;
 }

 if ( y[0] > y[1] ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }
      
 for ( py = y[0]; py < y[1]; py++ ) {
       px = ((py - y[0]) / dy) * dx + x[0];

       imel_draw_line (image, px, py, ox, oy, pixel);
 }

 return true;
}

ImelSize **__imel_draw_get_line_points_array (ImelSize _x1, ImelSize _y1, ImelSize _x2, 
                                              ImelSize _y2, long int *lx, long int *ly)
{
 ImelSize **ptr;
 long int x[2] = { (long int) _x1, (long int) _x2 };
 long int y[2] = { (long int) _y1, (long int) _y2 };
 long int j = -1;
 double dx, dy, px, py;

 return_var_if_fail (lx && ly, NULL);
 
 ptr = (ImelSize **) malloc (sizeof (ImelSize *) * 2);
 
 if ( _y1 == _y2 && _x1 == _x2 ) {
      *lx = 0;
      *ly = 0;
      ptr[0] = (ImelSize *) malloc (sizeof (ImelSize));
      ptr[1] = (ImelSize *) malloc (sizeof (ImelSize));
      
      ptr[0][0] = _x1;
      ptr[1][0] = _y1;
      
      return ptr;
 }
 
 if ( (_x1 > _x2 && _y1 > _y2) ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }

 dx = *lx = x[1] - x[0];
 dy = *ly = y[1] - y[0];
 
 if ( abs (dx) >= abs (dy) ) {
      ptr[0] = (ImelSize *) malloc (sizeof (ImelSize) * dx);
      ptr[1] = (ImelSize *) malloc (sizeof (ImelSize) * dx);
      
      if ( x[0] > x[1] ) {
           long_int_swap (&(y[0]), &(y[1]));
           long_int_swap (&(x[0]), &(x[1]));
      }
      
      for ( px = x[0]; px < x[1]; px++ ) {
            py = ((px - x[0]) / dx) * dy + y[0];

            ptr[0][++j] = px;
            ptr[1][j] = py;
      }

      return ptr;
 }

 if ( y[0] > y[1] ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }
 
 ptr[0] = (ImelSize *) malloc (sizeof (ImelSize) * dy);
 ptr[1] = (ImelSize *) malloc (sizeof (ImelSize) * dy);
      
 for ( py = y[0]; py < y[1]; py++ ) {
       px = ((py - y[0]) / dy) * dx + x[0];

       ptr[0][++j] = px;
       ptr[1][j] = py;
 }

 return ptr; 
}

/**
 * @brief Draw a non contiguous figure
 * 
 * This function draw a non contiguous figure in @p image where each start and end point 
 * is knowed. Each point have color and level passed @p pixel
 * 
 * @code
 * ImelPoint *starts[4] = { NULL }, *ends[3];
 * ImelPixel pixel = { 255, 0, 0, 0 };
 *
 * image = imel_image_new (64, 64);
 *
 * starts[0] = imel_point_new (image, 8, 8, pixel);
 * starts[1] = imel_point_new (image, 56, 8, pixel);
 * starts[2] = imel_point_new (image, 32, 58, pixel);
 *
 * ends[0] = imel_point_new (image, 32, 32, pixel);
 * ends[1] = ends[2] = ends[0];
 *
 * imel_draw_figure (image, 3, starts, ends, pixel);
 * imel_point_free (ends[0]);
 * imel_point_array_free (starts);
 * @endcode
 * @image html images/draw_figure.jpg "Example output"
 * @image latex images/draw_figure.eps "Example output"
 * @param image Image where draw the figure
 * @param n_points Number of points in @p starts and @p ends
 * @param starts Start points
 * @param ends End points
 * @param pixel Color and level of the figure 
 */
void imel_draw_figure (ImelImage *image, ImelSize n_points, ImelPoint **starts, ImelPoint **ends, ImelPixel pixel)
{
 ImelSize i = 0;

 return_if_fail (image && starts && ends);

 for ( ; i < n_points; i++ ) 
         imel_draw_line (image, starts[i]->x, starts[i]->y,
                         ends[i]->x, ends[i]->y, pixel);
}

/**
 * @brief Draw a contiguous figure
 * 
 * This function draw a contiguous figure in @p image from a list of @p points 
 * where each point is linked to the next one.
 * 
 * @code
 * ImelImage *image = imel_image_new (100, 100);
 * ImelPixel white = imel_pixel_new (0xff, 0xff, 0xff, 0);
 * ImelPoint *points[5] = {
 *          imel_point_new (image, 10, 10, white),
 *          imel_point_new (image, 90, 10, white),
 *          imel_point_new (image, 90, 90, white),
 *          imel_point_new (image, 10, 90, white),
 *          NULL };
 *
 * points[4] = *points;
 * imel_draw_contiguous_figure (image, 5, points, white);
 * points[4] = NULL;
 * imel_point_array_free (points);
 * @endcode
 * @param image Image where draw the figure
 * @param n_points Number of points in @p points or 0 if it's a NULL-terminated array.
 * @param points Points to link togheter
 * @param pixel Color and level of the figure
 */
void imel_draw_contiguous_figure (ImelImage *image, ImelSize n_points, ImelPoint **points, ImelPixel pixel)
{
 ImelSize i = 0;

 return_if_fail (image && points);
 
 if ( !n_points ) {
      if ( *points )
           while ( points[++n_points] );
      else return;
 }
 
 n_points--;
 for ( i = 0; i < n_points; i++ )
         imel_draw_line (image, points[i]->x, points[i]->y,
                         points[i + 1]->x, points[i + 1]->y, pixel);
}

/**
 * @brief Draw a Bèzier's curve
 * 
 * This function draw a Bèzier's curve in @p image with color and level
 * passed in @p pixel.
 * 
 * @code
 * ImelPixel px_p = { 255, 0, 0, 0 }, px_c = { 0, 0, 0, 1 };
 * ImelImage *image;
 *
 * image = imel_image_new (200, 200);
 * imel_draw_circle (image, 10, 190, 5, px_p);
 * imel_font_write_string (image, 17, 183, "x1, y1", 7, px_p);
 * imel_draw_circle (image, 10, 60, 5, px_p);
 * imel_font_write_string (image, 17, 53, "x2, y2", 7, px_p);
 * imel_draw_circle (image, 190, 190, 5, px_p);
 * imel_font_write_string (image, 140, 183, "x3, y3", 7, px_p);
 * imel_draw_circle (image, 130, 10, 5, px_p);
 * imel_font_write_string (image, 137, 3, "x4, y4", 7, px_p);
 *
 * imel_draw_curve (image, 10, 190, 10, 60, 190, 190, 130, 10, 200, px_c);
 * @endcode
 * @image html images/curva_bezier.jpg "Example output"
 * @image latex images/curva_bezier.eps "Example output"
 * @param image Image where draw the curve
 * @param x1 Start coordinate x of the curve
 * @param y1 Start coordinate y of the curve
 * @param x2 First reference coordinate x
 * @param y2 First reference coordinate y
 * @param x3 End coordinate x of the curve
 * @param y3 End coordinate y of the curve
 * @param x4 Second reference coordinate x
 * @param y4 Second reference coordinate y
 * @param _p Number of steps to draw the curve, -1 to set default steps ( 1024 )
 * @param pixel Color and level of the curve 
 */
void imel_draw_curve (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, 
                      ImelSize x3, ImelSize y3, ImelSize x4, ImelSize y4, int _p, ImelPixel pixel)
{
  float t;
  float x = x1, y = y1;
  float lx, ly;
  float a, b;
  float p = 1.0f / (( _p < 0 ) ? 1024 : _p);
  
  return_if_fail (image);
  
  for( b = 1 - (a = (t = 0)); t < 1; b = 1 - (a = (t += p)) ) {
       lx = x;
       ly = y;
       x = (x1 * pow (b, 3)) + 3 * x2 * pow (b, 2) * a + 3 * x4 * b * pow (a, 2) + x3 * pow (a, 3);
       y = (y1 * pow (b, 3)) + 3 * y2 * pow (b, 2) * a + 3 * y4 * b * pow (a, 2) + y3 * pow (a, 3);

       imel_draw_line (image, lx, ly, x, y, pixel);
  }
  imel_draw_line (image, x, y, x3, y3, pixel);
}

/**
 * @brief Draw lines between all points passed
 * 
 * This function draw lines from each point to all different points linking
 * each one to all others.
 * 
 * @param image Image where draw these lines
 * @param points A NULL-terminated array with the points to link
 * @param pxl Color and level of the lines
 */
void imel_draw_line_connecting_all_points (ImelImage *image, ImelPoint **points, ImelPixel pxl)
{
 int32_t j = -1, k;
 
 return_if_fail (image && points);
 
 while ( points[++j] )
         for ( k = 0; points[k]; k++ )
               if ( k != j )
                    imel_draw_line (image, points[j]->x, points[j]->y,
                                    points[k]->x, points[k]->y, pxl);
}

/**
 * @brief Draw a Bèzier's curve with gradient
 * 
 * This function draw a Bèzier's curve in @p image with gradient from color 
 * and level passed in @p start to color and level passed in @p end.
 * 
 * @param image Image where draw the curve
 * @param x1 Start coordinate x of the curve
 * @param y1 Start coordinate y of the curve
 * @param x2 First reference coordinate x
 * @param y2 First reference coordinate y
 * @param x3 End coordinate x of the curve
 * @param y3 End coordinate y of the curve
 * @param x4 Second reference coordinate x
 * @param y4 Second reference coordinate y
 * @param _p Number of steps to draw the curve, -1 to set default steps ( 1024 )
 * @param start Start color and level of the curve 
 * @param end End color and level of the curve
 * @see imel_draw_curve
 */
void imel_draw_gradient_curve (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, 
                               ImelSize x3, ImelSize y3, ImelSize x4, ImelSize y4, int _p, 
                               ImelPixel start, ImelPixel end)
{
  float t;
  float x = x1, y = y1;
  float lx, ly;
  float a, b;
  int s = -1, e = (( _p < 0) ? 1024 : _p );
  float p = 1.0f / (( _p < 0 ) ? 1024 : _p);
  long int step = -1;
  ImelPixel pxl, rpxl;
  
  return_if_fail (image);
  
  step = (end.level - start.level) / e;
  if ( step < 0 )
       rpxl.level = start.level - step;
  else rpxl.level = start.level;
  
  for( b = 1 - (a = (t = 0)); t < 1.0f; b = 1 - (a = (t += p)) ) {
       pxl = imel_pixel_union (start, end, (255.0f * ++s) / e);
       rpxl = imel_pixel_new (pxl.red, pxl.green, pxl.blue, rpxl.level + step);
       
       lx = x;
       ly = y;
       x = (x1 * pow (b, 3)) + 3 * x2 * pow (b, 2) * a + 3 * x4 * b * pow (a, 2) + x3 * pow (a, 3);
       y = (y1 * pow (b, 3)) + 3 * y2 * pow (b, 2) * a + 3 * y4 * b * pow (a, 2) + y3 * pow (a, 3);

       imel_draw_line (image, lx, ly, x, y, rpxl);
  }
  imel_draw_line (image, x, y, x3, y3, rpxl);
}

/**
 * @brief Draw a dashed line
 * 
 * This function draw a dashed line in @p image from coordinate \f$(\_x_1,\_y_1)\f$
 * to coordinate \f$(\_x_2,\_y_2)\f$ with small lines length @p size_line pixels and 
 * space between them of @p space_line pixels.
 *
 * @param image Image where draw the line
 * @param _x1 Start coordinate x of the line
 * @param _y1 Start coordinate y of the line
 * @param _x2 End coordinate x of the line
 * @param _y2 End coordinate y of the line
 * @param size_line Length of the small lines 
 * @param space_line Space between the small lines
 * @param pixel Color and level of the line
 * @return FALSE if image or size_line values aren't valid, else TRUE
 */
bool imel_draw_dashed_line (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2,
                            ImelSize size_line, ImelSize space_line, ImelPixel pixel)
{
 long int x[2] = { (long int) _x1, (long int) _x2 };
 long int y[2] = { (long int) _y1, (long int) _y2 };
 int j = 0, k;
 double dx, dy, px, py;

 return_var_if_fail (size_line, true);
 
 if ( !space_line )
      return imel_draw_line (image, _x1, _y1, _x2, _y2, pixel);
 
 return_var_if_fail (image, false);
 
 if ( _y1 == _y2 && _x1 == _x2 ) {
      __imel_draw_point (image, _y1, _x1, pixel);
      return true;
 }
 
 if ( (_x1 > _x2 && _y1 > _y2) ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }

 dx = x[1] - x[0];
 dy = y[1] - y[0];
 
 __imel_draw_point (image, x[0], y[0], pixel);
 if ( abs (dx) >= abs (dy) ) {
      if ( x[0] > x[1] ) {
           long_int_swap (&(y[0]), &(y[1]));
           long_int_swap (&(x[0]), &(x[1]));
      }
      
      for ( px = x[0]; px < x[1]; px++ ) {
            py = ((px - x[0]) / dx) * dy + y[0];
   
            if ( j++ < size_line && !k )
                 __imel_draw_point (image, px, py, pixel);

            if ( j > size_line )
                 if ( ++k >= space_line )
                      k = j = 0;
      }

      return true;
 }

 if ( y[0] > y[1] ) {
      long_int_swap (&(y[0]), &(y[1]));
      long_int_swap (&(x[0]), &(x[1]));
 }
      
 for ( py = y[0]; py < y[1]; py++ ) {
       px = ((py - y[0]) / dy) * dx + x[0];

       if ( j++ < size_line && !k )
            __imel_draw_point (image, px, py, pixel);

       if ( j > size_line ) {
            if ( k < space_line )
                 k++;
            else k = j = 0;
       }
 }

 return true;
}

/**
 * @brief Draw a dashed grid
 * 
 * This function draw a dashed grid in @p image from coordinate \f$(sx,sy)\f$ 
 * to coordinate \f$(ex,ey)\f$ with columns length @p col_space and rows 
 * length @p row_space.
 * 
 * @param image Image where draw the grid
 * @param sx Start coordinate x of the grid
 * @param sy Start coordinate y of the grid
 * @param ex End coordinate x of the grid
 * @param ey End coordinate y of the grid
 * @param col_space Space between columns
 * @param row_space Space between rows
 * @param size_line Length of the small lines 
 * @param space_line Space between the small lines
 * @param init_from_start TRUE if the first column and row init 
 * from coordinate \f$(sx,sy)\f$, else FALSE
 * @param pixel Color and level of the line
 * @see imel_draw_dashed_line
 */
void imel_draw_dashed_grid (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey,  
                            ImelSize col_space, ImelSize row_space, ImelSize size_line, 
                            ImelSize space_line, bool init_from_start, ImelPixel pixel)
{
 ImelSize x, y;

 return_if_fail (image);

 if ( sx > ex || sy > ey ) {
      imel_printf_debug ("imel_draw_dashed_grid", NULL, "warning",
                         "the start point > end point");
      return;
 }

 for ( x = init_from_start ? sx : sx + col_space; x < ex; x += col_space ) {
       for ( y = init_from_start ? sy : sy + row_space; y < ey; y += row_space ) {
             imel_draw_dashed_line (image, x, sy, x, ey, size_line, space_line, pixel);
             imel_draw_dashed_line (image, sx, y, ex, y, size_line, space_line, pixel);
       }
 }

}

/**
 * @brief Draw a grid
 * 
 * This function draw a grid in @p image from coordinate \f$(sx,sy)\f$ to
 * coordinate \f$(ex,ey)\f$ witch columns length @p col_space and rows 
 * length @p row_space.
 * 
 * @param image Image where draw the grid
 * @param sx Start coordinate x of the grid
 * @param sy Start coordinate y of the grid
 * @param ex End coordinate x of the grid
 * @param ey End coordinate y of the grid
 * @param col_space Space between columns
 * @param row_space Space between rows
 * @param init_from_start TRUE if the first column and row init from 
 * coordinate \f$(sx,sy)\f$, else FALSE
 * @param pixel Color and level of the line
 */
void imel_draw_grid (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey,
                     ImelSize col_space, ImelSize row_space, bool init_from_start, ImelPixel pixel)
{
 imel_draw_dashed_grid (image, sx, sy, ex, ey, col_space, row_space, 1, 0, init_from_start, pixel);
}

/**
 * @brief Draw a gradient
 * 
 * This function draw a gradient in @p image, with an @p orientation chosen,
 * from color and level passed in @p start_color to @p end_color.
 * 
 * @param image Image where draw the gradient
 * @param orientation Gradient orientation
 * @param start Start row or column ( depends from orientation )
 * @param end End row or column ( depends from orientation )
 * @param start_color Start color and level
 * @param end_color End color and level
 */
void imel_draw_gradient (ImelImage *image, ImelOrientation orientation,
                         ImelSize start, ImelSize end, ImelPixel start_color,
                         ImelPixel end_color)
{
 ImelSize s = ((start < end) ? start : end) - 1, e = (end < start) ? start : end;
 ImelPixel pxl;
 ImelLevel level;

 return_if_fail (image);
 
 if ( orientation == IMEL_ORIENTATION_HORIZONTAL ) {
      while ( ++s < e ) {
              pxl = imel_pixel_union (start_color, end_color, (255 * s) / e);
              pxl.level = (s * end_color.level ) / (start_color.level ? start_color.level : 1);
              if ( !start_color.level && pxl.level == 1 )
                   pxl.level = 0;
              imel_draw_line (image, s, 0, s, image->height, pxl);
      }

      return;
 }

 while ( ++s < e ) {
         pxl = imel_pixel_union (start_color, end_color, (255 * s) / e);
              pxl.level =  (s * end_color.level ) / (start_color.level ? start_color.level : 1);
              if ( !start_color.level && pxl.level == 1 )
                   pxl.level = 0;
         imel_draw_line (image, 0, s, image->width, s, pxl);
 }
}

/**
 * @brief Draw a regular shape
 * 
 * This function draw a regular shape in @p image with center in coordinate 
 * \f$(x,y)\f$, chosen @p radius and @p v vertices with a rotation 
 * of @p start_angle radians.
 * 
 * @param image Image where draw the shape
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param r Radius of the shape
 * @param v Vertices of the shape
 * @param start_angle Rotation of the shape in radians
 * @param pxl Color and level of the shape
 * @return TRUE if all values are valid, else FALSE 
 * @see imel_draw_partial_reg_shape
 */
bool imel_draw_reg_shape (ImelImage *image, ImelSize x, ImelSize y, ImelSize r, long v, double start_angle, ImelPixel pxl)
{
 return imel_draw_partial_reg_shape (image, x, y, r, v, 100, start_angle, pxl);
}

/**
 * @brief Draw a circle
 * 
 * This function draw a circle in @p image with center in coordinate \f$(x,y)\f$ and 
 * radius @p radius.
 * 
 * @param image Image where draw the circle
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param radius Radius size
 * @param pxl Color and level of the circle
 */
void imel_draw_circle (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, ImelPixel pxl)
{
 long v = ( radius < 127 ) ? 50 : radius >> 2;
 ImelSize sx = x - radius - 1, ex = x + radius + 1; 
 long int po[2] = { -1, -1 }, py[2];
 double p;

 return_if_fail (image);

 if ( radius < 6 ) {
      /* old method */
      while ( ++sx < ex ) {
              p = sqrt (pow (((double) radius), 2) - pow (((double) sx - x), 2));

              py[0] = ((long int) y) + ((long int) p);
              if ( po[0] == -1 && ((ImelSize) py[0]) < image->height ) {
                   imel_pixel_copy (&(image->pixel[py[0]][sx]), pxl);
              }
              else imel_draw_line (image, sx - 1, (po[0] < py[0]) ? po[0] : py[0], sx,
                                   (po[0] > py[0]) ? po[0] : py[0], pxl);
              po[0] = py[0];

              py[1] = ((long int) y) - ((long int) p);
              if ( po[1] == -1 && ((ImelSize) py[1]) < image->height ) {
                   imel_pixel_copy (&(image->pixel[py[1]][sx]), pxl);
              }
              else imel_draw_line (image, sx - 1, (po[1] < py[1]) ? po[1] : py[1], sx,
                                   (po[1] > py[1]) ? po[1] : py[1], pxl);
              po[1] = py[1];
      }
 }  
 else imel_draw_partial_reg_shape (image, x, y, radius, v, 100, 0.0f, pxl);
}

/**
 * @brief Draw a partial regular shape
 * 
 * This function draw a partial regular shape in @p image with center in coordinate
 * \f$(x,y)\f$, radius @p r, vertices @p v and rotation @p start_angle in radians.
 * The shape will be draw for only the @p p percent.
 * 
 * @param image Image where draw the shape
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param r Radius of the shape
 * @param v Number of vertices
 * @param p Percentage of the shape ( Values from 0 to 100 )
 * @param start_angle Rotation angle in radians
 * @param pxl Color and level of the shape
 * @return TRUE if all values are valid, else FALSE
 * @see imel_draw_reg_shape
 */
bool imel_draw_partial_reg_shape (ImelImage *image, ImelSize x, ImelSize y, ImelSize r, 
                                  long v, short p , double start_angle, ImelPixel pxl)
{
 short counter = 0;
 double increment = 6.283185307f / ((double) v ? v : 1.f);
 long percentage = (long)(((double)(p * v) / (100.00000)) + 0.500000 ) ;
 ImelSize Ox, Oy, Fx, Fy, X, Y; 
	
 return_var_if_fail (image && v > 2 && start_angle < 3.141592654f 
                     && start_angle > -3.141592654f && p > 0 && p < 101 , false); 

 Fx = Ox = (ImelSize) (((double) x) + (((double) r) * cos (start_angle))); 
 Fy = Oy = (ImelSize) (((double) y) - (((double) r) * sin (start_angle)));
	
 for ( start_angle += increment ; ++counter <= percentage ; start_angle += increment, Ox = X, Oy = Y ) { 
       X = (ImelSize) (((double) x) + (((double) r) * cos (start_angle))); 
       Y = (ImelSize) (((double) y) - (((double) r) * sin (start_angle)));

       imel_draw_line (image, X, Y, Ox, Oy, pxl);
 } 
 
 if ( p == 100)
	  imel_draw_line (image, Fx, Fy, Ox, Oy, pxl);
 
 return true;
}

/**
 * @brief Draw an arch
 * 
 * This function draw an arch in @p image with center in coordinate \f$(x,y)\f$ 
 * and radius of @p radius pixels. 
 * 
 * @param image Image where draw the arch
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param radius Radius of the arch
 * @param start_angle Start angle of the arch in radians
 * @param end_angle End angle of the arch in radians
 * @param pxl Color and level of the arch
 * @return TRUE if all values are valid, else FALSE
 * @see imel_draw_filled_arch
 */
bool imel_draw_arch (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                     double start_angle, double end_angle, ImelPixel pxl)
{
 double j, ox, oy, ax, ay;
 
 return_var_if_fail (image, false);
 
 if ( start_angle > end_angle )
      while ( start_angle > end_angle ) 
              end_angle += PI * 2;
 
 if ( end_angle > DEG_TO_RAD (360) )
      return false;
 
 ox = ax = ((double) x) + radius * cos (start_angle);
 oy = ay = ((double) y) + radius * sin (start_angle);
 __imel_draw_point (image, (ImelSize) ox, (ImelSize) oy, pxl);
 
 for ( j = RAD_TO_DEG (start_angle), end_angle = RAD_TO_DEG (end_angle); 
       j < end_angle; j++, ox = ax, oy = ay ) { 
       
       ax = (((double) x) + radius * cos (DEG_TO_RAD (j)));
       ay = (((double) y) + radius * sin (DEG_TO_RAD (j)));
       
       imel_draw_line (image, (ImelSize) ox, (ImelSize) oy, 
                       (ImelSize) ax, (ImelSize) ay, pxl);       
 }
 
 return true;
}

/**
 * @brief Draw a filled arch
 * 
 * This function draw a filled arch in @p image with center in coordinate \f$(x,y)\f$
 * and radius of @p radius pixels. 
 * 
 * @param image Image where draw the arch
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param radius Radius of the arch
 * @param start_angle Start angle of the arch in radians
 * @param end_angle End angle of the arch in radians
 * @param pxl Color and level of the arch
 * @return TRUE if all values are valid, else FALSE
 * @see imel_draw_arch
 */
bool imel_draw_filled_arch (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                            double start_angle, double end_angle, ImelPixel pxl)
{
 double j, ox, oy, ax, ay;
 
 return_var_if_fail (image, false);
 
 if ( start_angle > end_angle )
      while ( start_angle > end_angle ) 
              end_angle += PI * 2;
 
 if ( end_angle > DEG_TO_RAD (360) )
      return false;
 
 ox = ax = ((double) x) + radius * cos (start_angle);
 oy = ay = ((double) y) + radius * sin (start_angle);
 imel_draw_line (image, (ImelSize) ox, (ImelSize) oy, x, y, pxl);
 
 for ( j = RAD_TO_DEG (start_angle), end_angle = RAD_TO_DEG (end_angle); 
       j < end_angle; j++, ox = ax, oy = ay ) { 
       
       ax = (((double) x) + radius * cos (DEG_TO_RAD (j)));
       ay = (((double) y) + radius * sin (DEG_TO_RAD (j)));
       
       imel_draw_filled_line (image, (ImelSize) ox, (ImelSize) oy, 
                              (ImelSize) ax, (ImelSize) ay, x, y, pxl);       
 }
 
 return true;
}

/**
 * @brief Draw a spiral
 * 
 * This function draw an Archimedean spiral in @p image with center in 
 * coordinate \f$(x,y)\f$ with a radius of @p radius pixels and @p distance
 * pixels between each spiral's arm.
 * 
 * @param image Image where draw the spiral
 * @param x Center coordinate x
 * @param y Center coordinate y
 * @param radius Radius of the spiral
 * @param distance Distance between each spiral's arm
 * @param pxl Color and level of the spiral
 * @return FALSE if image isn't valid, else TRUE
 */
bool imel_draw_spiral (ImelImage *image, ImelSize x, ImelSize y, 
                       ImelSize radius, ImelSize distance, ImelPixel pxl)
{
 double c, xs, ys, factor;
 double rotate = (double) (radius);
 double limx[] = { ((double) x) + ((double) radius), ((double) x) - ((double) radius) };
 double limy[] = { ((double) y) + ((double) radius), ((double) y) - ((double) radius) };
 
 return_var_if_fail (image, false);
 
 factor = (distance * 0.166666666); 
 for ( c = 1; c < rotate; c += 0.0001 ) {
       xs = ((double) x) + (cos (c) * (c * factor));
       ys = ((double) y) - (sin (c) * (c * factor));
       
       if ( xs > limx[0] || xs < limx[1] 
         || ys > limy[0] || ys < limy[1] )
            break;
            
       __imel_draw_point (image, (ImelSize) xs, (ImelSize) ys, pxl);
 }
 
 return true;
}
