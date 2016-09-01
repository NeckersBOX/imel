/*
 * "info_cut.c" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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
 * @file info_cut.c
 * @author Davide Francesco Merico
 * @brief This file contains functions to operate with guide lines.
 */

/**
 * @brief Make a new guide
 * 
 * This function make a new guide line with a chosen @p orientation 
 * and @p position.
 * 
 * @param orientation Guide line orientation
 * @param position Guide line position on chosen axis
 * @return A new guide line or NULL on error.
 * 
 * @see ImelOrientation
 * @see imel_info_cut_add
 * @see imel_info_cut_free
 */
ImelInfoCut *imel_info_cut_new (ImelOrientation orientation, ImelSize position)
{
 ImelInfoCut *info_cut;
 
 info_cut = (ImelInfoCut *) malloc (sizeof (ImelInfoCut));
 return_var_if_fail (info_cut, NULL);
 
 info_cut->index = 0;
 info_cut->orientation = orientation;
 info_cut->position = position;
 info_cut->next = NULL;
 
 return info_cut;
}

/**
 * @brief Add a guide line to existed list
 * 
 * This function add a new guide line to an existed list.
 * 
 * @param cut_info Guide lines list
 * @param orientation Guide line orientation
 * @param position Guide line position on chosen axis
 * @return Pointer to new start of @p cut_info list or NULL on error
 * 
 * @see ImelOrientation
 * @see imel_info_cut_new
 * @see imel_info_cut_remove_element
 */
ImelInfoCut *imel_info_cut_add (ImelInfoCut *cut_info, ImelOrientation orientation, 
                                ImelSize position)
{
 ImelInfoCut *new_cut_info;
 
 return_var_if_fail (cut_info, NULL);
 
 new_cut_info = imel_info_cut_new (orientation, position);
 new_cut_info->index = cut_info->index + 1;
 new_cut_info->next = cut_info;
 
 return new_cut_info;
}

/**
 * @brief Get the element to a chosen index
 * 
 * This function get the element in @p info_cut which its index is 
 * equal to @p index.
 * 
 * @param info_cut Guide lines list
 * @param index Chosen element index
 * @return A pointer to the element if found it or NULL on error
 */
ImelInfoCut *imel_info_cut_get_index (ImelInfoCut *info_cut, ImelSize index)
{
 ImelInfoCut *p;
 
 return_var_if_fail (info_cut, NULL);
 return_var_if_fail (index <= info_cut->index, NULL);
 
 for ( p = info_cut; p->index != index; p = p->next );
 
 return p;
}

/**
 * @brief Copy the element to a chosen index
 * 
 * This function copy the element in @p info_cut which its index is
 * equal to @p index.
 * 
 * @param info_cut Guide lines list
 * @param index Chosen element index
 * @return A new ImelInfoCut element or NULL on error.
 * 
 * @see imel_info_cut_get_index
 * @see imel_info_cut_free 
 */
ImelInfoCut *imel_info_cut_copy_index (ImelInfoCut *info_cut, ImelSize index)
{
 ImelInfoCut *p;
 
 if ( !(p = imel_info_cut_get_index (info_cut, index)) )
      return NULL;
 
 return imel_info_cut_new (p->orientation, p->position);
}

/**
 * @brief Duplicate a guide lines list
 * 
 * This function duplicate the @p info_cut list passed.
 * 
 * @param info_cut Guide lines list
 * @return A copy of @p info_cut
 */
ImelInfoCut *imel_info_cut_copy (ImelInfoCut *info_cut)
{
 ImelInfoCut *copy_info_cut, *start, *p;
 
 return_var_if_fail (info_cut, NULL);
 
 copy_info_cut = imel_info_cut_new (info_cut->orientation, info_cut->position);
 copy_info_cut->index = info_cut->index;
 copy_info_cut->next = info_cut->next;
 
 start = copy_info_cut;
 for ( p = info_cut->next; p; p = p->next ) {
	   copy_info_cut->next = imel_info_cut_new (p->orientation, p->position);
	   copy_info_cut = copy_info_cut->next;
	   
	   copy_info_cut->index = p->index;
	   copy_info_cut->next = p->next;
 }
       
 return start;
}

/**
 * @brief Remove a guide line from a list
 * 
 * This function remove the element which index is equal to @p index
 * in the @p cut_info list.
 * 
 * @param cut_info Guide lines list
 * @param index Index of the element to remove
 * @return A pointer to the start of the @p cut_info passed
 * 
 * @see imel_info_cut_add
 */
ImelInfoCut *imel_info_cut_remove_element (ImelInfoCut *cut_info, ImelSize index)
{
 ImelInfoCut *p_index, *p;
 
 p_index = imel_info_cut_get_index (cut_info, index);
 return_var_if_fail (p, cut_info);
 
 if ( cut_info->index == index ) {
	  p = cut_info->next;
	  free (cut_info);
	  return p;
 }
 
 for ( p = cut_info; cut_info; cut_info = cut_info->next ) {
	   if ( cut_info->next->index == index ) {
		    cut_info->next = cut_info->next->next;
		    free (p_index);
		    break;
	   }
	   
	   cut_info->index--;
 }
 
 return p;
}

/**
 * @brief Free the memory of a guide lines list
 * 
 * @param cut_info Guide lines list to free
 * 
 * @see imel_info_cut_new
 */
void imel_info_cut_free (ImelInfoCut *cut_info)
{
 ImelInfoCut *p;
 
 return_if_fail (cut_info);
 
 for ( p = cut_info; p; cut_info = p ) {
       p = cut_info->next;
       free (cut_info);
 }
}

/**
 * @brief Swap two element of a guide lines list
 * 
 * This function swap the element at index @p index_a with
 * the element at index @p index_b.
 * 
 * @param cut_info Guide lines list
 * @param index_a Index of the first element
 * @param index_b Index of the second element
 */
void imel_info_cut_swap_index (ImelInfoCut *cut_info, 
                               ImelSize index_a, ImelSize index_b)
{
 ImelInfoCut *p_index_a, *p_index_b, *p;
 
 p_index_a = imel_info_cut_get_index (cut_info, index_a);
 p_index_b = imel_info_cut_get_index (cut_info, index_b);
 
 return_if_fail (p_index_a && p_index_b);
 
 p = imel_info_cut_new (p_index_a->orientation, p_index_a->position);
 
 p_index_a->orientation = p_index_b->orientation;
 p_index_a->position    = p_index_b->position;
 p_index_b->orientation = p->orientation;
 p_index_b->position    = p->position;
 
 imel_info_cut_free (p);
}

/**
 * @brief Count the element in a guide lines list
 * 
 * @param cut_info Guide lines list
 * @return Elements number or 0 on error.
 * 
 * @note Same as <tt>cut_info->index + 1</tt>
 */
ImelSize imel_info_cut_count (ImelInfoCut *cut_info)
{
 return_var_if_fail (cut_info, 0);
 
 return cut_info->index + 1;
}

/**
 * @brief Get the next guide line on the same axis
 * 
 * @param image Image reference for the guide lines
 * @param cut_info Guide lines list
 * @param index Index of the current guide line
 * @return A pointer to the next guide line, based on their position, or
 * NULL on error.
 * 
 * @see imel_info_cut_get_prev
 */
ImelInfoCut *imel_info_cut_get_next (ImelImage *image, ImelInfoCut *cut_info, ImelSize index)
{
 ImelInfoCut *p_index, *p, *p_next = NULL;
 
 p_index = imel_info_cut_get_index (cut_info, index);
 return_var_if_fail (p_index, NULL);
 
 if ( p_index->orientation == IMEL_ORIENTATION_HORIZONTAL 
   && p_index->position >= image->height )
      return NULL;
 
 if ( p_index->orientation == IMEL_ORIENTATION_VERTICAL 
   && p_index->position >= image->width )
      return NULL;
 
 for ( p = cut_info; p; p = p->next ) {
	   if ( p->orientation != p_index->orientation )
	        continue;
	   
	   if ( p->position <= p_index->position )
	        continue;
	   
	   if ( !p_next ) {
		    p_next = p;
		    continue;
	   }
	   
	   if ( p->position < p_next->position )
	        p_next = p;       
 } 
 
 return p_next;
}

/**
 * @brief Get the previous guide line on the same axis
 * 
 * @param image Image reference for the guide lines
 * @param cut_info Guide lines list
 * @param index Index of the current guide line
 * @return A pointer to the previous guide line, based on their position, or
 * NULL on error.
 * 
 * @see imel_info_cut_get_next
 */
ImelInfoCut *imel_info_cut_get_prev (ImelImage *image, ImelInfoCut *cut_info, ImelSize index)
{
 ImelInfoCut *p_index, *p, *p_prev = NULL;
 
 p_index = imel_info_cut_get_index (cut_info, index);
 return_var_if_fail (p_index, NULL);

 for ( p = cut_info; p; p = p->next ) {
	   if ( p->orientation != p_index->orientation )
	        continue;
	   
	   if ( p->position >= p_index->position )
	        continue;
	   
	   if ( !p_prev ) {
		    p_prev = p;
		    continue;
	   }
	   
	   if ( p->position > p_prev->position )
	        p_prev = p;       
 } 
 
 return p_prev;
}

/**
 * @brief Get the first guide line on the chosen axis
 * 
 * @param cut_info Guide lines list
 * @param orientation Reference axis
 * @return A pointer to the first guide line, based on it position or NULL on error.
 * 
 * @see imel_info_cut_get_max
 * @see imel_info_cut_get_next
 */
ImelInfoCut *imel_info_cut_get_min (ImelInfoCut *cut_info, ImelOrientation orientation)
{
 ImelInfoCut *p, *p_min = NULL;
 
 for ( p = cut_info; p; p = p->next ) {
	   if ( p->orientation != orientation )
	        continue;
	        
	   if ( !p_min ) {
	        p_min = p;
	        continue;
	   }
	   
	   if ( p->position < p_min->position )
	        p_min = p;
 }
 
 return p_min;
}

/**
 * @brief Get the last guide line on the chosen axis
 * 
 * @param cut_info Guide lines list
 * @param orientation Reference axis
 * @return A pointer to the last guide line, based on it position or NULL on error.
 * 
 * @see imel_info_cut_get_min
 * @see imel_info_cut_get_prev
 */
ImelInfoCut *imel_info_cut_get_max (ImelInfoCut *cut_info, ImelOrientation orientation)
{
 ImelInfoCut *p, *p_max = NULL;
 
 for ( p = cut_info; p; p = p->next ) {
	   if ( p->orientation != orientation )
	        continue;
	        
	   if ( !p_max ) {
	        p_max = p;
	        continue;
	   }
	   
	   if ( p->position > p_max->position )
	        p_max = p;
 }
 
 return p_max;
}

/**
 * @brief Get the split number of an image
 * 
 * This function get the number of the areas which the @p image will be splitted
 * if cutted with the guide lines in @p cut_info list.
 * 
 * @param image Image reference
 * @param cut_info Guide lines list
 * @param orientation Reference axis
 * @return The number of areas resulted by hypotetical cuts 
 */
ImelSize imel_info_cut_get_split (ImelImage *image, ImelInfoCut *cut_info, ImelOrientation orientation)
{
 ImelInfoCut *p;
 ImelSize cnt = 1;
 
 p = imel_info_cut_get_min (cut_info, orientation);
 return_var_if_fail (p, cnt);
 
 for ( ; p = imel_info_cut_get_next (image, cut_info, p->index); cnt++ );
 
 return cnt;
}
