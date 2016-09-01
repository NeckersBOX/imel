/*
 * "value.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
 * @file value.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to convert values
 */
 
#ifndef DOXYGEN_IGNORE_DOC

typedef double (*ImelValueFunc)(double value, double opt_value);

#endif

double imel_value_percentage_to_generic     (double value, double opt_value);
double imel_value_pixel_to_percentage       (double value, double opt_value);

/**
 * @brief Convert a value in another one
 * 
 * @param from_value Type of value you to convert in @p to_value
 * @param value Value to convert in @p to_value
 * @param to_value Type of value to convert from @p from_value
 * @param ... Value to convert from @p from_value
 * @return Value converted
 */
double imel_value_convert (ImelValue from_value, double value, ImelValue to_value, ...)
{
 va_list list;
 double convert_value, opt_value;
 static ImelValueFunc value_func[2][2] = 
                       {
                       	{
 	                     NULL, 
 	                     imel_value_percentage_to_generic,
 	                    },
 	                    {
 	                     imel_value_pixel_to_percentage,
 	                     NULL,
 	                    },
 	                   };

 va_start (list, to_value);             
 opt_value = va_arg (list, double);
 va_end (list);
 
 if ( value_func[from_value][to_value] )
 	  return value_func[from_value][to_value] (value, opt_value);
 
 return value;
}

/**
 * Convert a percentage in a generic value
 * 
 * @param value Percentage value
 * @param opt_value Generic value equal to 100%
 * @return Percentage value converted
 */
double imel_value_percentage_to_generic (double value, double opt_value)
{
 return ((opt_value * value) / 100.f);
}

/**
 * Convert a pixel value to percentage
 * 
 * @param value Value to convert
 * @param opt_value Generic value equal to 100%
 * @return Value converted
 */
double imel_value_pixel_to_percentage (double value, double opt_value)
{
 return ((100.f * value) / opt_value);
}
