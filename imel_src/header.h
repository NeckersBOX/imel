/*
 * "header.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

/**
 * @file header.h
 * @author Davide Francesco Merico
 * @brief This file contains types and macros used by Imel library.
 *
 */
#ifndef imel_header
#define imel_header

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#ifdef debug_enable
#include <sys/types.h>
#include <unistd.h>
#endif
#include "error.h"
#include "image_enum.h"

#define IMEL_VERSION_MAJOR 3 /**< Imel version major */
#define IMEL_VERSION_MINOR 0 /**< Imel version minor */

#define RGBA_R_MASK(val) (((val) & 0xff000000) >> 24) /**< Mask for red channel in a RGBA value */
#define RGBA_G_MASK(val) (((val) & 0xff0000) >> 16) /**< Mask for green channel in a RGBA value */
#define RGBA_B_MASK(val) (((val) & 0xff00) >> 8) /**< Mask for blue channel in a RGBA value */
#define RGBA_A_MASK(val)  ((val) & 0xff) /**< Mask for alpha channel in a RGBA value */

#define DEG_TO_RAD(val) (0.017453292 * (val)) /**< Convert degrees to radians */
#define RAD_TO_DEG(val) (57.29577951 * (val)) /**< Convert radians to degrees */

#ifdef debug_enable
#define __memory_buffer 0x00 /* byte */
#else
#define __memory_buffer 128 /* byte*/
#endif

#ifndef __cplusplus
typedef enum _bool_type { false = 0, true = 1 } bool; /**< Boolean type */
#endif

#ifdef IMEL_BROKEN_TYPE
typedef unsigned int size;
typedef size ImelSize;
typedef unsigned char color;
typedef color ImelColor;
#else
typedef uint32_t ImelSize; /**< ImelSize type */
typedef uint8_t  ImelColor; /**< ImelColor type */
#endif

typedef int32_t ImelLevel; /**< ImelLivel type */

/**
 * Effects to apply through #imel_image_apply_effect or some
 * other function.
 * 
 * @note Enum values starts from 0 
 * @see imel_image_apply_effect
 */
typedef enum _imel_effect {
	         /**
	          * Grayscale effect to an image.
	          */
             IMEL_EFFECT_WHITE_BLACK = 0,     
             /**
              * Antique effect ( less bright colors ).
              */
             IMEL_EFFECT_ANTIQUE,
             /**
              * Invert colors.
              */              
             IMEL_EFFECT_INVERT,    
             /**
              * Calculates the average of colors of the image and apply it to RGB channel 
              * selected. When used as a parameter for #imel_image_apply_effect function 
              * takes as a parameter a value of ImelMask type (with the option to combine 
              * multiple channels with the OR operator).
              */       
             IMEL_EFFECT_NORMALIZE,  
             /**
              * Affect the image brightness. When used as a parameter for #imel_image_apply_effect
              * function takes as a parameter a value between -100 and +100.
              */          
             IMEL_EFFECT_BRIGHTNESS,
             /**
              * Auto contrast enhancement.
              */
             IMEL_EFFECT_CONTRAST_STRETCHING,
             /**
              * Affect the image contrast. When used as a parameter for #imel_image_apply_effect 
              * function takes as a parameter a number between -127 and +128.
              */
             IMEL_EFFECT_CONTRAST,
             /**
              * Rasterizes the image, which divides the image into squares of size chosen, 
              * colored with the average of pixels that are inside each square. When used 
              * as a parameter for the #imel_image_apply_effect function it takes as a 
              * parameter the size in pixels of the side of the square.
              */
             IMEL_EFFECT_RASTERIZE,
             /**
              * Apply antialiasing effect. First calculating the value that takes each 
              * pixel of the image and then sets it. When used as a parameter for 
              * #imel_image_apply_effect function it takes as a parameter the number of 
              * pixels of the side of the square to be analyzed to calculate the value of 
              * the current pixel.
              */
             IMEL_EFFECT_ANTIALIAS,
             /**
              * As IMEL_EFFECT_ANTIALIAS except for the fact that the value of the current 
              * pixel applies it immediately after it is calculated.
              */
             IMEL_EFFECT_DIRECT_ANTIALIAS,
             /**
              * Sum the colors of an image to the colors of another. When used as a parameter 
              * for the function #imel_image_apply_effect requires as a parameter the image 
              * to be added.
              */
             IMEL_EFFECT_IMAGE_ADD,
             /**
              * Subtract the colors of an image to the colors of another. When used as a parameter 
              * for the function #imel_image_apply_effect requires as a parameter the image 
              * to be subtracted.
              */
             IMEL_EFFECT_IMAGE_SUBTRACT,
             /**
              * It allows to bring a shade of color to transparency to creating the right shades so 
              * that the resulting image can be retrofit on a different background. It receives as 
              * a parameter the color to be eliminated in the form of pointer to #ImelPixel variable.
              */
             IMEL_EFFECT_COLOR_TO_ALPHA
        } ImelEffect;

/**
 * ImelMask type. Specifies one or more channels of an image to which 
 * to apply a given operation.
 * 
 * @code
 * ImelMask red_blue = IMEL_MASK_RED | IMEL_MASK_BLUE;
 * ImelMask green = IMEL_MASK_GREEN;
 * ImelMask rgb = red_blue | green;
 * @endcode
 * @see imel_image_remove_noise
 * @see imel_image_apply_noise
 * @see imel_image_apply_filter
 * @see imel_image_remove_base_color
 */
typedef enum _imel_mask {
             IMEL_MASK_RED   = 1 << 0, /**< Red Mask   ( Value: 1 ) */
             IMEL_MASK_GREEN = 1 << 1, /**< Green Mask ( Value: 2 ) */
             IMEL_MASK_BLUE  = 1 << 2, /**< Blue Mask  ( Value: 4 ) */
             IMEL_MASK_LEVEL = 1 << 3  /**< Level Mask ( Value: 8 ) */
        } ImelMask;


/**
 * ImelLogicOperation type. Specifies which logic operation to use.
 * 
 * @note Enum values starts from 0.
 * @see imel_image_apply_logic_operation
 */ 
typedef enum _imel_logic_operation {
               IMEL_LOGIC_AND = 0, /**< Logic AND */
               IMEL_LOGIC_OR,      /**< Logic OR  */
               IMEL_LOGIC_XOR      /**< Logic XOR */
        } ImelLogicOperation;

/**
 * ImelFontSize type. Enumerator created to facilitate the insertion of 
 * standard sizes for the internal fonts in Imel whose size is 14px or 
 * IMEL_FONT_SIZE_MEDIUM, other values have been incorporated to avoid 
 * as much as possible the presence of artifacts that degrade the image.
 * 
 * @see imel_font_write_string
 * @see imel_font_write_vstring
 */
typedef enum _imel_font_size {
             IMEL_FONT_SIZE_SMALL = 7,   /**< Small Size: 7px **/
             IMEL_FONT_SIZE_MEDIUM = 14, /**< Medium\\Original Size: 14px */
             IMEL_FONT_SIZE_LARGE = 28   /**< Large Size: 28px **/
        } ImelFontSize;

/**
 * ImelHistogram type. Specifies which histogram to get from an image.
 * 
 * @see imel_image_get_histogram
 * @see imel_image_get_histogram_image
 */
typedef enum _imel_histogram {
             IMEL_HISTOGRAM_RED = 0, /**< It produces only the histogram for the Red channel. */
             IMEL_HISTOGRAM_GREEN,   /**< It produces only the histogram for the Green channel. */
             IMEL_HISTOGRAM_BLUE,    /**< It produces only the histogram for the Blue channel. */
             IMEL_HISTOGRAM_COMPLETE /**< It produces only the histogram for the image brightness. */
        } ImelHistogram;

/**
 * ImelhistogramLayout type. Specifies the histogram layout.
 * 
 * @see imel_image_get_histograms_image
 */
typedef enum _imel_histogram_layout {
             IMEL_HISTOGRAM_LAYOUT_VERTICAL,   /**< It realizes the histograms with vertical layout */
             IMEL_HISTOGRAM_LAYOUT_HORIZONTAL, /**< It realizes the histograms with horizontal layout */
             IMEL_HISTOGRAM_LAYOUT_PANELS      /**< It realizes the histograms with panel layour */
        } ImelHistogramLayout;

/**
 * ImelOrientation type. Specifies vertical or horizontal orientation.
 * 
 * @note Enum values starts from 0
 * @see imel_draw_gradient
 * @see imel_image_perspective
 * @see imel_image_slant
 * @see imel_image_shift
 * @see imel_image_shift_lines
 * @see imel_info_cut_add
 */
typedef enum _imel_orientation {
             IMEL_ORIENTATION_HORIZONTAL = 0, /**< Horizontal Orientation */
             IMEL_ORIENTATION_VERTICAL        /**< Vertical Orientation */
} ImelOrientation;

/**
 * ImelLevelOperation type. Specifies operations with levels.
 * 
 * @note Enum values starts from 0
 * @see imel_image_change_color_level
 * @see imel_image_change_level
 */
typedef enum _imel_level_operation {
             IMEL_LEVEL_OPERATION_SET = 0, /**< Set the value */
             IMEL_LEVEL_OPERATION_ADD      /**< Add the value an existed one */
} ImelLevelOperation;

/**
 * ImelNoiseOperation type. Specifies operations with the noise.
 * 
 * @note Enum values starts from 0
 * @see imel_image_apply_noise
 */
typedef enum _imel_noise_operation {
             IMEL_NOISE_OPERATION_SUM = 0,  /**< Sum Operation */
             IMEL_NOISE_OPERATION_SUBTRACT, /**< Subtract Operation */
             IMEL_NOISE_OPERATION_MULTIPLY, /**< Multiply Operation */
             IMEL_NOISE_OPERATION_DIVIDE,   /**< Divide Operation */
             IMEL_NOISE_OPERATION_RANDOM    /**< Random Operation ( between the four above ) */
} ImelNoiseOperation;

/**
 * ImelPatternOperation type. Specifies operations with patterns.
 * 
 * @note Enum values starts from 0
 * @see imel_image_apply_pattern
 */
typedef enum _imel_pattern_operation { 
             IMEL_PATTERN_OPERATION_INSERT = 0, /**< Insert pattern over the original image */
             IMEL_PATTERN_OPERATION_SUM,        /**< Sum pattern colors to original image colors */
             IMEL_PATTERN_OPERATION_SUBTRACT,   /**< Subtract pattern colors from original image colors */
} ImelPatternOperation;


/**
 * ImelAlignment type. Specifies the image alignment.
 * 
 * @see imel_image_union
 */
typedef enum _imel_alignment {
             IMEL_ALIGNMENT_TL, /**< Alignment: Top Left */
             IMEL_ALIGNMENT_TR, /**< Alignment: Top Right */
             IMEL_ALIGNMENT_BL, /**< Alignment: Bottom Left */
             IMEL_ALIGNMENT_BR  /**< Alignment: Bottom Right */
} ImelAlignment;

/**
 * ImelRef type. Specifies the reference.
 * 
 * @see imel_image_auto_cut
 */
typedef enum _imel_ref {
             IMEL_REF_COLOR = 0, /**< Reference to color */
             IMEL_REF_LEVEL      /**< Reference to level */
} ImelRef;

/**
 * ImelValue type. Specifies the units of measure.
 * 
 * @see imel_point_get_from_line
 * @see imel_value_convert
 */
typedef enum _imel_value {
	         IMEL_VALUE_PERCENTAGE = 0, /**< Percentage */
	         IMEL_VALUE_PIXEL           /**< Pixel */
} ImelValue;

/**
 * @brief Rappresentation of a pixel in Imel library.
 * 
 * This type is the fundamental one, includes all the information needed to 
 * create a pixel composed of the three basic colors and its importance or 
 * transparency.
 * 
 * @code
 * ImelPixel m1_color = { 255, 255, 255, 0 };
 * ImelPixel m2_color;
 * 
 * m2_color = imel_pixel_new_from_string ('#ffffff', 0);
 * @endcode
 * 
 * @see imel_pixel_new
 * @see imel_pixel_new_from_string
 * @see imel_pixel_new_from_rgba
 * @see imel_pixel_new_from_hsl
 * @see imel_pixel_copy
 * 
 * @note If two pixels have different level values when they are used in
 * functions like #imel_pixel_copy the first one can be replaced only if 
 * the second one has the same or greater level value.
 */
typedef struct _imel_pixel {
	           /*@{*/
               ImelColor red;   /**< Red channel. Values from 0 to 255. */
               ImelColor green; /**< Green channel. Values from 0 to 255. */
               ImelColor blue;  /**< Blue channel. Values from 0 to 255. */
               ImelLevel level; /**< Alpha for values from 0 to -255, else level */ 
               /*@}*/
        } ImelPixel;

/**
 * @brief Rappresentation of an image in Imel library
 * 
 * The most important type of Imel with #ImelPixel is this. This type, in Imel, 
 * is an image which contains its resolution and its colors.
 * 
 * @see imel_image_new
 * @see imel_image_new_from
 */
typedef struct _imel_image {
	           /*@{*/
               ImelSize width;    /**< Image width */
               ImelSize height;   /**< Image height */
               ImelPixel **pixel; /**< 2-dimensional array in [y][x] format. */
               /*@}*/
        } ImelImage;

/**
 * @brief Rappresentation of a point in Imel library
 * 
 * This type can rappresent a point with coordinates, color and level.
 * 
 * @see imel_point_new
 */
typedef struct _imel_point {
	           /*@{*/
               ImelSize x;      /**< Coordinate x */
               ImelSize y;      /**< Coordinate y */
               ImelPixel pixel; /**< Color and level of the point */
	           /*@}*/
        } ImelPoint;

/**
 * @brief Specialized type in reporting errors inside Imel.
 * 
 * This type report an error if occurred when doing operations in Imel.
 * Each error have a code and a description.
 * 
 * @note Usually it can be used in this way:
 * @code
 * ImelImage *image;
 * ImelError error;
 * 
 * image = imel_image_new_from ("image.png", 0, &error);
 * if ( !image ) {
 *      fprintf (stderr, "Error %d: %s\n", error.code, error.description);
 *      return 1;
 * }
 * @endcode
 */
typedef struct _imel_error {
	           /*@{*/
               int code;          /**< Error code */
               char *description; /**< Error description */
	           /*@}*/
        } ImelError;

/**
 * This type collect information about the hypothetical lines that cut an 
 * image vertically or horizontally at a specific point.
 * 
 * @see imel_image_cut_grid
 * @see imel_info_cut_new
 * @see imel_info_cut_add
 * @see imel_info_cut_free
 */
typedef struct _imel_info_cut {
	           /*@{*/
	           ImelSize index;              /**< Element index */
               ImelOrientation orientation; /**< Line orientation */
               ImelSize position;           /**< Line position */
               struct _imel_info_cut *next; /**< Next element */
               /*@}*/
        } ImelInfoCut;

/**
 * This type can identify a color through HSL values.
 * 
 * @see imel_pixel_new_from_hsl
 * @see imel_pixel_get_hsl
 */
typedef struct _imel_hsl {
	           /*@{*/
	           double h; /**< Hue value */
	           double s; /**< Saturation value */
	           double l; /**< Luminosity value */
	           /*@}*/
	    } ImelHSL;

/**
 * Pointer to an #ImelImage
 * 
 * @note Used internally.
 * @see ImelImage
 */
typedef ImelImage *ImelImagePtr;
/**
 * Generic pointer
 * @note Used internally.
 */
typedef void *ImelGenericPtr;
/**
 * Generic function to operate with image and data.
 * @note Used internally.
 */
typedef void (*ImelGenericFuncPtr)(ImelImagePtr,ImelGenericPtr);

/** 
 * imel_debug_printf are foundamental for macro #return_if_fail 
 * and #return_var_if_fail. It print the message only if compiled
 * with debug=true.
 * 
 * @param function Name of the function where it's called or NULL
 * @param filename Name of the file or NULL
 * @param error_level Type of error ( Warning, Error, Info, ... ) or NULL
 * @param __format Message
 * @param ... Parameters requested by @p __format
 * @return TRUE if debug=true, else FALSE
 */

#ifndef HAVE_imel_printf_debug_FUNC
extern bool imel_printf_debug (const char *function, const char *filename, 
                               const char *error_level, char *__format, ...);
#endif

#ifdef HAVE_imel_printf_debug_FUNC
bool imel_printf_debug (const char *function, const char *filename, 
                        const char *error_level, char *__format, ...);
#endif

/**
 * Return @p var if @p condition is TRUE
 */
#define return_var_if_fail(condition, var) \
        if ( !(condition) ) { \
             imel_printf_debug (NULL, NULL, "warning", "condition failed.\n"); \
             return (var); \
        }

/**
 * Return if @p condition is TRUE
 */
#define return_if_fail(condition) \
        if ( !(condition) ) { \
             imel_printf_debug (NULL, NULL, "warning", "condition failed.\n"); \
             return; \
        }
        
#ifdef __cplusplus
 }
#endif

#endif
