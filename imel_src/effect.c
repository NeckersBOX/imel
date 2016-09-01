/*
 * "effect.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

extern void imel_pixel_set (ImelPixel *, ImelColor, ImelColor, ImelColor, ImelLevel);
extern void imel_pixel_set_from_pixel (ImelPixel *pixel, ImelPixel pxl);
extern ImelImage *imel_image_new (ImelSize width, ImelSize height);
extern ImelColor imel_color_sum (ImelColor a, ImelColor b);
extern ImelColor imel_color_subtract (ImelColor a, ImelColor b);
extern void imel_image_free (ImelImage *image);

static ImelColor abs_color (int expression)
{
 return (expression < 0) ? 0 : (expression > 255) ? 255 : expression;
}

void imel_effect_white_black (ImelImagePtr image, ImelGenericPtr data)
{
 ImelColor c;
 ImelSize y, x;
 ImelPixel *p;

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                 continue;

             c = (0.3 * p->red) + (0.59 * p->green) + (0.11 * p->blue);
             imel_pixel_set (p, c, c, c, p->level);
       }
 }
}

void imel_effect_antique (ImelImagePtr image, ImelGenericPtr data)
{
 ImelColor c;
 ImelSize y, x;
 ImelPixel *p;

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             c = ((64 * p->red) + (160 * p->green) + (32 * p->blue)) / 256;
             imel_pixel_set (p, ((c * 3) + p->red) / 4, ((c * 3) + p->green) / 4,
                                ((c * 3) + p->blue) / 4, p->level);
       }
 }
}

void imel_effect_invert (ImelImagePtr image, ImelGenericPtr data)
{
 ImelSize y, x;
 ImelPixel *p;

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;
             imel_pixel_set (p, 255 - p->red, 255 - p->green, 255 - p->blue, p->level);
       }
 }
}

void imel_effect_normalize (ImelImagePtr image, ImelGenericPtr data)
{
 ImelSize y, x, mask = (ImelSize) data, normalize[3];
 ImelPixel *p;

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             normalize[0] += image->pixel[y][x].red;
             normalize[1] += image->pixel[y][x].green;
             normalize[2] += image->pixel[y][x].blue;
       }
 }

 normalize[0] /= image->height * image->width;
 normalize[1] /= image->height * image->width;
 normalize[2] /= image->height * image->width;

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             switch ( mask ) {
                case IMEL_MASK_RED:
                      imel_pixel_set (p, normalize[0], p->green, p->blue, p->level);
                      break;
                case IMEL_MASK_GREEN:
                      imel_pixel_set (p, p->red, normalize[1], p->blue, p->level);
                      break;
                case IMEL_MASK_BLUE:
                      imel_pixel_set (p, p->red, p->green, normalize[2], p->level);
                      break;
                case IMEL_MASK_RED | IMEL_MASK_GREEN:
                      imel_pixel_set (p, normalize[0], normalize[1], p->blue, p->level);
                      break;
                case IMEL_MASK_RED | IMEL_MASK_BLUE:
                      imel_pixel_set (p, normalize[0], p->green, normalize[2], p->level);
                      break;
                case IMEL_MASK_GREEN | IMEL_MASK_BLUE:
                      imel_pixel_set (p, p->red, normalize[1], normalize[2], p->level);
                      break;
              }
       }
 }
}

void imel_effect_brightness (ImelImagePtr image, ImelGenericPtr data)
{
 ImelPixel *p;
 int perc = (int) data;
 ImelColor red, green, blue;
 ImelSize y, x, mask = ( perc > -1 ) ? 1 : 0;

 perc = ((perc = ((mask ? perc : perc * -1) * 255) / 100) > 255) ? 
        255 * (mask ? 1 : -1) : perc * (mask ? 1 : -1);

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             red = ((p->red + perc) > 255) ? 255 : ((p->red + perc) < 0) ? 0 : p->red + perc;
             green = ((p->green + perc) > 255) ? 255 : ((p->green + perc) < 0) ? 0 : p->green + perc;
             blue = ((p->blue + perc) > 255) ? 255 : ((p->blue + perc) < 0) ? 0 : p->blue + perc;
             imel_pixel_set (p, red, green, blue, p->level);
       }
 }
}

void imel_effect_contrast_stretching (ImelImagePtr image, ImelGenericPtr data)
{
 ImelSize x, y;
 ImelPixel *p;
 int tmp_color;
 ImelColor red, green, blue, x0, x1, rgb[2][3] = {
                                                  { 0xff, 0xff, 0xff },
                                                  { 0x00, 0x00, 0x00 }
                                                 };

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             rgb[0][0] = (p->red < rgb[0][0]) ? p->red : rgb[0][0];
             rgb[0][1] = (p->green < rgb[0][1]) ? p->green : rgb[0][1];
             rgb[0][2] = (p->blue < rgb[0][2]) ? p->blue : rgb[0][2];
             rgb[1][0] = (p->red > rgb[1][0]) ? p->red : rgb[1][0];
             rgb[1][1] = (p->green > rgb[1][1]) ? p->green : rgb[1][1];
             rgb[1][2] = (p->blue > rgb[1][2]) ? p->blue : rgb[1][2];
        }
 }

 x0 = ( rgb[0][0] < rgb[0][1] ) ? ( rgb[0][0] < rgb[0][2] ) ? rgb[0][0] : rgb[0][2] :
                                  ( rgb[0][1] < rgb[0][2] ) ? rgb[0][1] : rgb[0][2];
 x1 = ( rgb[1][0] > rgb[1][1] ) ? ( rgb[1][0] > rgb[1][2] ) ? rgb[1][0] : rgb[1][2] :
                                  ( rgb[1][1] > rgb[1][2] ) ? rgb[1][1] : rgb[1][2];

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             if ( p->level < 0 )
                  continue;

             if ( x0 != x1 ) {
                  tmp_color = (255 * (p->red - x0)) / ( x1 - x0 );
                  red = ( tmp_color > 255 ) ? 255 : ( tmp_color < 0 ) ? 0 : tmp_color;
             }
             else red = p->red;

             if ( x0 != x1 ) {
                  tmp_color = (255 * (p->green - x0)) / ( x1 - x0 );
                  green = ( tmp_color > 255 ) ? 255 : ( tmp_color < 0 ) ? 0 : tmp_color;
             }
             else green = p->green;

             if ( x0 != x1 ) {
                  tmp_color = (255 * (p->blue - x0)) / ( x1 - x0 );
                  blue = ( tmp_color > 255 ) ? 255 : ( tmp_color < 0 ) ? 0 : tmp_color;
             }
             else blue = p->blue;

             imel_pixel_set (p, red, green, blue, p->level);
       }
 }
}

void imel_effect_contrast (ImelImagePtr image, ImelGenericPtr data)
{
 int s = (int) data;
 ImelSize y, x;
 ImelPixel *p;
 ImelColor red, green, blue;
 float contrast;
 float contrast_arg = (s > 128) ? 1.0f : (s < -127) ? -1.0f : s / 127.0f;

 if ( contrast_arg >= 0.0f ) {
      contrast_arg = (contrast_arg > 0.99999f) ? 0.99999 : contrast_arg;
      contrast_arg = 1.0f / (1.0f - contrast_arg);
 }
 else {
      contrast_arg = (contrast_arg < -1.0f) ? -1.0f : contrast_arg;
      contrast_arg = 1.0f + contrast_arg;
 }

 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);

             contrast = (((((float) p->red) / 255) - 0.5f) * contrast_arg) + 0.5f;
             red = (contrast > 1.0f) ? 255 : (contrast < 0.0f ) ? 0 : contrast * 255;

             contrast = (((((float) p->green) / 255) - 0.5f) * contrast_arg) + 0.5f;
             green = (contrast > 1.0f) ? 255 : (contrast < 0.0f ) ? 0 : contrast * 255;

             contrast = (((((float) p->blue) / 255) - 0.5f) * contrast_arg) + 0.5f;
             blue = (contrast > 1.0f) ? 255 : (contrast < 0.0f ) ? 0 : contrast * 255;

             imel_pixel_set (p, red, green, blue, p->level);
       }
 }
}

void imel_effect_rasterize (ImelImagePtr image, ImelGenericPtr data)
{
 ImelSize s = (ImelSize) data, y, x, normalize[3], tmp[2], q[2];
 ImelPixel *p;
 
 if ( s < 1 ) {
#ifdef debug_enable
      fprintf (stderr, "Debug (%d): imel_effect_rasterize: warning: %s\n",
                       getpid (), "get 0 as argument ( Wrong cast? )");
#endif
      return;
 }
 
 if ( s > image->width || s > image->height )
      s = ( image->width < image->height ) ? image->width : image->height;


 for ( y = 0; y < image->height; y += s ) {
       for ( x = 0; x < image->width; x += s ) {
             tmp[0] = ((y + s) > image->height) ? image->height : y + s;
             tmp[1] = ((x + s) > image->width) ? image->width : x + s;

             for ( q[0] = y; q[0] < tmp[0]; q[0]++ ) {
                   for ( q[1] = x; q[1] < tmp[1]; q[1]++ ) {
                         p = &(image->pixel[q[0]][q[1]]);
                         if ( p->level < 0 )
                              continue;

                         normalize[0] += p->red;
                         normalize[1] += p->green;
                         normalize[2] += p->blue;
                   }
             }

             normalize[0] /= (tmp[0] - y) * (tmp[1] - x);
             normalize[1] /= (tmp[0] - y) * (tmp[1] - x);
             normalize[2] /= (tmp[0] - y) * (tmp[1] - x);

             for ( q[0] = y; q[0] < tmp[0]; q[0]++ ) {
                   for ( q[1] = x; q[1] < tmp[1]; q[1]++ ) {
                         p = &(image->pixel[q[0]][q[1]]);
                         if ( p->level < 0 )
                              continue;

                         imel_pixel_set (p, normalize[0], normalize[1], normalize[2], p->level);
                   }
              }

               normalize[0] = normalize[1] = normalize[2] = 0;
      }
 }
}

void imel_effect_antialias (ImelImagePtr image, ImelGenericPtr data)
{
 ImelImage *l_image;
 long int x, y, j, k, z;
 int64_t m[4];
 int q = ((int) data) >> 1;
 
 l_image = imel_image_new (image->width, image->height);
 for ( y = 0; y < image->height; y++ ) {
       for ( x = z = 0; x < image->width; x++, z = 0 ) {
             memset (m, 0, sizeof (int64_t) * 4);
             
             for ( j = y - q; j <= (y + q); j++ ) {
                   for ( k = x - q; k <= (x + q); k++ ) {
                         if ( (k > -1 && k < image->width) && (j > -1 && j < image->height) ) {         
                              m[0] += (int64_t) image->pixel[j][k].red;
                              m[1] += (int64_t) image->pixel[j][k].green;
                              m[2] += (int64_t) image->pixel[j][k].blue;    
                                                    
                              if ( image->pixel[j][k].level > -1 )
                                   m[3] += 0xff + image->pixel[j][k].level;
                              else m[3] += (int64_t) image->pixel[j][k].level;
                              
                              z++;
                         }
                   }
             }
             
             z = z ? z : 1;
             imel_pixel_set (&(l_image->pixel[y][x]), m[0] / z, m[1] / z, m[2] / z, 
                             (image->pixel[y][x].level > -1) ? image->pixel[y][x].level : m[3] / z);
       }
 }
 
 for ( y = 0; y < image->height; y++ )
       for ( x = 0; x < image->width; x++ )
             imel_pixel_set_from_pixel (&(image->pixel[y][x]), l_image->pixel[y][x]);
 
 imel_image_free (l_image);
}

void imel_effect_direct_antialias (ImelImagePtr image, ImelGenericPtr data)
{
 long int x, y, j, k, z;
 int64_t m[4];
 int q = ((int) data) >> 1;
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = z = 0; x < image->width; x++, z = 0 ) {
             memset (m, 0, sizeof (int64_t) * 4);
             
             for ( j = y - q; j <= (y + q); j++ ) {
                   for ( k = x - q; k <= (x + q); k++ ) {
                         if ( (k > -1 && k < image->width) && (j > -1 && j < image->height) ) {
                              m[0] += (int64_t) image->pixel[j][k].red;
                              m[1] += (int64_t) image->pixel[j][k].green;
                              m[2] += (int64_t) image->pixel[j][k].blue;
                              if ( image->pixel[j][k].level > -1 ) 
                                   m[3] += 0xff + image->pixel[j][k].level;
                              else m[3] += (int64_t) image->pixel[j][k].level;
                              
                              z++;
                         }
                   }
             }
             
             image->pixel[y][x].red   = m[0] / (z ? z : 1);
             image->pixel[y][x].green = m[1] / (z ? z : 1);
             image->pixel[y][x].blue  = m[2] / (z ? z : 1);
             image->pixel[y][x].level = m[3] / (z ? z : 1);
       }
 }
}

void imel_effect_image_add (ImelImagePtr image, ImelGenericPtr data)
{
 static ImelPixel result;
 ImelImage *add_img = (ImelImage *) data;
 ImelSize x, y;
 
 for ( y = 0; y < image->height && y < add_img->height; y++ ) {
       for ( x = 0; x < image->width && x < add_img->width; x++ ) {
             result.red = imel_color_sum (image->pixel[y][x].red, add_img->pixel[y][x].red);
             result.green = imel_color_sum (image->pixel[y][x].green, add_img->pixel[y][x].green);
             result.blue = imel_color_sum (image->pixel[y][x].blue, add_img->pixel[y][x].blue);
             result.level = image->pixel[y][x].level + add_img->pixel[y][x].level;
             
             imel_pixel_set_from_pixel (&(image->pixel[y][x]), result);
       }
 }
}

void imel_effect_image_subtract (ImelImagePtr image, ImelGenericPtr data)
{
 static ImelPixel result;
 ImelImage *add_img = (ImelImage *) data;
 ImelSize x, y;
 
 for ( y = 0; y < image->height && y < add_img->height; y++ ) {
       for ( x = 0; x < image->width && x < add_img->width; x++ ) {
             result.red = imel_color_subtract (image->pixel[y][x].red, add_img->pixel[y][x].red);
             result.green = imel_color_subtract (image->pixel[y][x].green, add_img->pixel[y][x].green);
             result.blue = imel_color_subtract (image->pixel[y][x].blue, add_img->pixel[y][x].blue);
             result.level = image->pixel[y][x].level - add_img->pixel[y][x].level;
             
             imel_pixel_set_from_pixel (&(image->pixel[y][x]), result);
       }
 }
}

void imel_effect_color_to_alpha (ImelImagePtr image, ImelGenericPtr data)
{
 ImelSize x, y;
 ImelPixel *_c = (ImelPixel *) data, *p;
 double src[4], alpha[4], c[4];
 
 /** Thank you Gimp's Developers for the your
     code that i could readjust **/
    
 c[0] = ((double) _c->red) / 255.f;
 c[1] = ((double) _c->green) / 255.f;
 c[2] = ((double) _c->blue) / 255.f;
 c[3] = (_c->level >= 0) ? 1.f : (_c->level < -255) ? 0.f : 
        -1 * (((double) _c->level) / 255.f);
 
 for ( y = 0; y < image->height; y++ ) {
       for ( x = 0; x < image->width; x++ ) {
             p = &(image->pixel[y][x]);
             
             if ( p->level <= -255 )
                  continue;
                  
             src[0] = ((double) p->red) / 255;
             src[1] = ((double) p->green) / 255;
             src[2] = ((double) p->blue) / 255;
             src[3] = (p->level >= 0) ? 1.f : (p->level < -255) ? 0.f : 
                      -1 * (((double) p->level) / 255.f);
             
             alpha[3] = src[3];

                  if (c[0] < 0.0001)
                      alpha[0] = src[0];
             else if (src[0] > c[0])
                      alpha[0] = (src[0] - c[0]) / (1.f - c[0]);
             else if (src[0] < c[0])
                      alpha[0] = (c[0] - src[0]) / c[0];
             else     alpha[0] = 0;

                  if (c[1] < 0.0001)
                      alpha[1] = src[1];
             else if (src[1] > c[1])
                      alpha[1] = (src[1] - c[1]) / (1.f - c[1]);
             else if (src[1] < c[1])
                      alpha[1] = (c[1] - src[1]) / c[1];
             else     alpha[1] = 0;

                  if (c[2] < 0.0001)
                      alpha[2] = src[2];
             else if (src[2] > c[2])
                      alpha[2] = (src[2] - c[2]) / (1.f - c[2]);
             else if (src[2] < c[2])
                      alpha[2] = (c[2] - src[2]) / c[2];
             else     alpha[2] = 0.f;
             
                  if (alpha[0] > alpha[1]) {
                      if ( alpha[0] > alpha[2] )
                           src[3] = alpha[0];
                      else src[3] = alpha[2];
                  }
             else if (alpha[1] > alpha[2])
                      src[3] = alpha[1];
             else     src[3] = alpha[2];

             if (src[3] < 0.0001) {
                 p->level = -255;
                 continue;
             }
             
             src[0]   = (src[0] - c[0]) / src[3] + c[0];
             src[1] = (src[1] - c[1]) / src[3] + c[1];
             src[2]  = (src[2] - c[2]) / src[3] + c[2];

             src[3] *= alpha[3];
             
             
             p->red   = (ImelColor) (255 * src[0]);
             p->green = (ImelColor) (255 * src[1]);
             p->blue  = (ImelColor) (255 * src[2]);
             p->level = (ImelLevel) (-255 * (1.f - src[3]));
       }
 }
}
