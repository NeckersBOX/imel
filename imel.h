/*
 * "imel.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef imel
#define imel

#include "imel_src/header.h"
#include "imel_src/freetype_export_types.h"

/** function @ file: src/miscellaneous.c **/
extern bool             imel_enable_brush                          (ImelImage *brush);
extern bool             imel_disable_brush                         (void);
                        
/** function @ file: src/image.c **/ 
extern void             imel_image_apply_color                     (ImelImage *image, ImelColor red, ImelColor green, ImelColor blue,
                                                                    bool mono);
extern void             imel_image_apply_color_from_string         (ImelImage *image, const char *string, bool mono);
extern void             imel_image_apply_convolution               (ImelImage *image, double **filter, int width, int height,
                                                                    double factor, double bias);
extern void             imel_image_apply_effect                    (ImelImage *image, ImelEffect effect, ...);
extern void             imel_image_apply_filter                    (ImelImage *image, ImelMask mask);
extern ImelImage       *imel_image_apply_logic_operation           (ImelImage *img1, ImelImage *img2, ImelLogicOperation logic_operation);
extern void             imel_image_apply_noise                     (ImelImage *image, ImelColor noise_range, ImelSize noise_quantity, 
                                                                    ImelMask mask, ImelNoiseOperation operation, bool nepc);
extern void             imel_image_apply_pattern                   (ImelImage *image, ImelImage *pattern, ImelPatternOperation operation);
extern ImelImage       *imel_image_auto_cut                        (ImelImage *image, ImelSize tollerance, ImelRef reference, ...);
extern void             imel_image_change_color_level              (ImelImage *image, ImelLevelOperation level_operation,
                                                                    ImelLevel level, ImelPixel color_pxl, ImelColor tollerance);
extern void             imel_image_change_level                    (ImelImage *image, ImelLevelOperation level_operation, ImelLevel level);
extern ImelImage       *imel_image_copy                            (ImelImage *image);
extern ImelImage       *imel_image_cut                             (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey);
extern ImelImage      **imel_image_cut_grid                        (ImelImage *image, ImelInfoCut *cut_info);
extern void             imel_image_free                            (ImelImage *image);
extern ImelSize         imel_image_get_height                      (ImelImage *image);
extern int             *imel_image_get_histogram                   (ImelImage *image, ImelHistogram histogram_type);
extern ImelImage       *imel_image_get_histogram_image             (ImelImage *image, int *__histogram, ImelHistogram histogram_type);
extern ImelImage       *imel_image_get_histograms_image            (ImelImage *image, ImelHistogramLayout layout);
extern ImelSize         imel_image_get_width                       (ImelImage *image);
extern void             imel_image_insert_image                    (ImelImage *dest, ImelImage *src, ImelSize sx, ImelSize sy);
extern ImelImage       *imel_image_mirror_horizontal               (ImelImage *image);
extern ImelImage       *imel_image_mirror_vertical                 (ImelImage *image);
extern ImelImage       *imel_image_new                             (ImelSize width, ImelSize height);
extern ImelImage       *imel_image_new_with_background_color       (ImelSize width, ImelSize height, ImelPixel pixel);
extern ImelImage       *imel_image_perspective                     (ImelImage *image, double rad_angle, ImelOrientation orientation);
extern void             imel_image_remove_base_color               (ImelImage *image, ImelMask mask);
extern void             imel_image_remove_noise                    (ImelImage *image, ImelSize size_q, ImelMask mask, ImelColor tollerance);
extern void             imel_image_replace_area_color              (ImelImage *image, ImelPixel src, ImelPixel dest, ImelSize tollerance,
                                                                    ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2);
extern void             imel_image_replace_color                   (ImelImage *image, ImelPixel src, ImelPixel dest, ImelSize tollerance);
extern ImelImage       *imel_image_resize                          (ImelImage *image, ImelSize width, ImelSize height);
extern ImelImage       *imel_image_rotate                          (ImelImage *image, double rotate_rad);
extern ImelImage       *imel_image_rotate_complete                 (ImelImage *image);
extern ImelImage       *imel_image_rotate_to_left                  (ImelImage *image);
extern ImelImage       *imel_image_rotate_to_right                 (ImelImage *image);
extern void             imel_image_shift                           (ImelImage *image, ImelOrientation orientation, long int move_pxl, 
                                                                    bool lengthens);
extern void             imel_image_shift_bpc                       (ImelImage *image, int bpc_shift_red, int bpc_shift_green, 
                                                                    int bpc_shift_blue);
extern bool             imel_image_shift_lines                     (ImelImage *image, ImelOrientation move_type, ImelSize line, 
                                                                    ImelSize width, long int move_pixel, bool lengthens);
extern ImelImage       *imel_image_slant                           (ImelImage *image, ImelSize x0, ImelSize y0, ImelSize x1, ImelSize y1, 
                                                                    ImelOrientation orientation, bool lengthens);
extern ImelImage       *imel_image_union                           (ImelImage *img1, ImelImage *img2, unsigned char opacity, 
                                                                    ImelAlignment alignment);

/** function @ file: src/image_fill.c **/
extern void             imel_image_fill_color_with_color           (ImelImage *image, ImelPoint *point, ImelSize tollerance);
extern void             imel_image_fill_color_with_level           (ImelImage *image, ImelPoint *point, ImelSize tollerance);
extern void             imel_image_fill_level_with_color           (ImelImage *image, ImelPoint *point, ImelSize tollerance);
extern void             imel_image_fill_level_with_level           (ImelImage *image, ImelPoint *point, ImelSize tollerance);

/** function @ file: src/image_new_from.c **/
extern ImelImage       *imel_image_new_from                        (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_bmp                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_bmp_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_bmp_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_cut                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_cut_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_cut_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_dds                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_dds_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_dds_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_exr                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_exr_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_exr_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_gif                    (const char *filename, ImelLevel level, ImelGifLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_gif_handle             (FILE *file, ImelLevel level, ImelGifLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_gif_memory             (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelGifLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_ico                    (const char *filename, ImelLevel level, ImelIcoLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_ico_handle             (FILE *file, ImelLevel level, ImelIcoLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_ico_memory             (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelIcoLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_iff                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_iff_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_iff_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_imel                   (const char *filename, ImelError *error);
extern ImelImage       *imel_image_new_from_imel_handle            (FILE *of, ImelError *error);
extern ImelImage       *imel_image_new_from_faxg3_memory           (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_j2k                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_j2k_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_j2k_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jng                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jng_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jng_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jp2                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jp2_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jp2_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_jpeg                   (const char *filename, ImelLevel level, ImelJpegLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_jpeg_handle            (FILE *file, ImelLevel level, ImelJpegLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_jpeg_memory            (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelJpegLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_koala                  (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_koala_handle           (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_koala_memory           (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_mng                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_mng_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_mng_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbm                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbm_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbm_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbmraw                 (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbmraw_handle          (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pbmraw_memory          (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pcd                    (const char *filename, ImelLevel level, ImelPcdLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_pcd_handle             (FILE *file, ImelLevel level, ImelPcdLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_pcd_memory             (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelPcdLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_pcx                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pcx_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pcx_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgm                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgm_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgm_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgmraw                 (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgmraw_handle          (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_pgmraw_memory          (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_png                    (const char *filename, ImelLevel level, ImelPngLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_png_handle             (FILE *file, ImelLevel level, ImelPngLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_png_memory             (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelPngLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_ppm                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ppm_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ppm_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ppmraw                 (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ppmraw_handle          (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ppmraw_memory          (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_psd                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_psd_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_psd_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ras                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ras_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_ras_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_raw                    (const char *filename, ImelSize width, ImelSize height, 
                                                                    int bits_red, int bits_green, int bits_blue, 
                                                                    int bits_level, ImelError *error);
extern ImelImage       *imel_image_new_from_sgi                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_sgi_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_sgi_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_targa                  (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_targa_handle           (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_targa_memory           (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_tiff                   (const char *filename, ImelLevel level, ImelTiffLoadFlags load_flags, 
                                                                    ImelError *error);
extern ImelImage       *imel_image_new_from_tiff_handle            (FILE *file, ImelLevel level, ImelTiffLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_tiff_memory            (uint8_t *memory, uint32_t length, ImelLevel level, 
                                                                    ImelTiffLoadFlags load_flags, ImelError *error);
extern ImelImage       *imel_image_new_from_wbmp                   (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_wbmp_handle            (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_wbmp_memory            (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xbm                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xbm_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xbm_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xpm                    (const char *filename, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xpm_handle             (FILE *file, ImelLevel level, ImelError *error);
extern ImelImage       *imel_image_new_from_xpm_memory             (uint8_t *memory, uint32_t length, ImelLevel level, ImelError *error);

/** function @ file: src/image_save.c **/ 
extern bool             imel_image_save_bmp                        (ImelImage *image, const char *filename, ImelBmpBits bits_per_pixel, 
                                                                    ImelError *error);
extern bool             imel_image_save_bmp_handle                 (ImelImage *image, FILE *of, ImelBmpBits bits_per_pixel, ImelError *error);
extern bool             imel_image_save_imel                       (ImelImage *image, const char *filename, ImelError *error);
extern bool             imel_image_save_j2k                        (ImelImage *image, const char *filename, ImelJ2kBits bits_per_pixel, 
                                                                    ImelError *error);
extern bool             imel_image_save_j2k_handle                 (ImelImage *image, FILE *of, ImelJ2kBits bits_per_pixel, ImelError *error);
extern bool             imel_image_save_jp2                        (ImelImage *image, const char *filename, ImelJ2kBits bits_per_pixel, 
                                                                    ImelError *error);
extern bool             imel_image_save_jp2_handle                 (ImelImage *image, FILE *of, ImelJ2kBits bits_per_pixel, ImelError *error);
extern bool             imel_image_save_jpeg                       (ImelImage *image, const char *filename, int quality, ImelError *error);
extern bool             imel_image_save_jpeg_handle                (ImelImage *image, FILE *of, int quality, ImelError *error);
extern bool             imel_image_save_png                        (ImelImage *image, const char *filename, ImelPngFlags png_flags,
                                                                    ImelError *error);
extern bool             imel_image_save_png_handle                 (ImelImage *image, FILE *of, ImelPngFlags png_flags, ImelError *error);
extern bool             imel_image_save_ppm                        (ImelImage *image, const char *filename, ImelError *error);
extern bool             imel_image_save_ppm_handle                 (ImelImage *image, FILE *of, ImelError *error);
extern bool             imel_image_save_ppmraw                     (ImelImage *image, const char *filename, ImelError *error);
extern bool             imel_image_save_ppmraw_handle              (ImelImage *image, FILE *of, ImelError *error);
extern bool             imel_image_save_tiff                       (ImelImage *image, const char *filename, ImelTiffFlags compression,
                                                                    ImelError *error);
extern bool             imel_image_save_tiff_handle                (ImelImage *image, FILE *of, ImelTiffFlags compression, ImelError *error);
extern bool             imel_image_save_wbmp                       (ImelImage *image, const char *filename, ImelError *error);
extern bool             imel_image_save_wbmp_handle                (ImelImage *image, FILE *of, ImelError *error);
extern bool             imel_image_save_xpm                        (ImelImage *image, const char *filename, ImelError *error);
extern bool             imel_image_save_xpm_handle                 (ImelImage *image, FILE *of, ImelError *error);

/** function @ file: src/color.c **/
extern ImelColor       *imel_color_get_from_pixel                  (ImelPixel pixel);
extern ImelPixel      **imel_color_get_number                      (ImelImage *image, ImelSize *number);
extern void             imel_color_set_background                  (ImelImage *image, ImelPixel pixel);
extern ImelColor        imel_color_subtract                        (ImelColor a, ImelColor b);
extern ImelColor        imel_color_sum                             (ImelColor a, ImelColor b);

/** function @ file: src/pixel.c **/
extern bool             imel_pixel_compare                         (ImelPixel a, ImelPixel b, ImelSize tollerance);
extern bool             imel_pixel_compare_level                   (ImelLevel a, ImelLevel b, ImelSize tollerance);
extern void             imel_pixel_copy                            (ImelPixel *dest, ImelPixel src);
extern double           imel_pixel_get_distance                    (ImelPixel a, ImelPixel b);
extern ImelHSL          imel_pixel_get_hsl                         (ImelPixel p);
extern uint32_t         imel_pixel_get_rgba                        (ImelPixel pxl);
extern ImelPixel        imel_pixel_new                             (ImelColor red, ImelColor green, ImelColor blue, ImelLevel level);
extern ImelPixel        imel_pixel_new_from_hsl                    (ImelHSL value);
extern ImelPixel        imel_pixel_new_from_rgba                   (uint32_t rgba);
extern ImelPixel        imel_pixel_new_from_string                 (const char *string, ImelLevel level);
extern void             imel_pixel_set                             (ImelPixel *pixel, ImelColor red, ImelColor green, ImelColor blue, 
                                                                    ImelLevel level);
extern void             imel_pixel_set_from_pixel                  (ImelPixel *pixel, ImelPixel pxl);
extern void             imel_pixel_set_from_string                 (ImelPixel *pixel, const char *string, ImelLevel level);
extern ImelPixel        imel_pixel_union                           (ImelPixel a, ImelPixel b, unsigned char opacity);

/** function @ file: src/draw.c **/
extern bool             imel_draw_arch                             (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                                                                    double start_angle, double end_angle, ImelPixel pxl);
extern void             imel_draw_circle                           (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, ImelPixel pxl);
extern void             imel_draw_contiguous_figure                (ImelImage *image, ImelSize n_points, ImelPoint **points, ImelPixel pixel);
extern void             imel_draw_curve                            (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, 
                                                                    ImelSize x3, ImelSize y3, ImelSize x4, ImelSize y4, int _p, 
                                                                    ImelPixel pixel);
extern void             imel_draw_dashed_grid                      (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey, 
                                                                    ImelSize col_space, ImelSize row_space, ImelSize size_line, 
                                                                    ImelSize space_line, bool init_from_start, ImelPixel pixel);
extern bool             imel_draw_dashed_line                      (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2,
                                                                    ImelSize size_line, ImelSize space_line, ImelPixel pixel);
extern void             imel_draw_ellipse                          (ImelImage *image, ImelSize x, ImelSize y, double a, double b, ImelPixel pxl);
extern void             imel_draw_figure                           (ImelImage *image, ImelSize n_points, ImelPoint **starts, 
                                                                    ImelPoint **ends, ImelPixel pixel);
extern bool             imel_draw_filled_arch                      (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                                                                    double start_angle, double end_angle, ImelPixel pxl);
extern void             imel_draw_filled_ellipse                   (ImelImage *image, ImelSize x, ImelSize y, double a, double b, ImelPixel pxl);
extern void             imel_draw_filled_circle                    (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, ImelPixel pxl);
extern bool             imel_draw_filled_line                      (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2, 
                                                                    ImelSize ox, ImelSize oy, ImelPixel pixel);
extern void             imel_draw_gradient                         (ImelImage *image, ImelOrientation orientation, ImelSize start, ImelSize end, 
                                                                    ImelPixel start_color, ImelPixel end_color);
extern void             imel_draw_gradient_curve                   (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, 
                                                                    ImelSize x3, ImelSize y3, ImelSize x4, ImelSize y4, int _p, 
                                                                    ImelPixel start, ImelPixel end);
extern bool             imel_draw_gradient_line                    (ImelImage *image, ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2, 
                                                                    ImelPixel start, ImelPixel end);
extern void             imel_draw_grid                             (ImelImage *image, ImelSize sx, ImelSize sy, ImelSize ex, ImelSize ey,
                                                                    ImelSize col_space, ImelSize row_space, bool init_from_start, 
                                                                    ImelPixel pixel);
extern bool             imel_draw_line                             (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, 
                                                                    ImelSize y2, ImelPixel pixel);
extern void             imel_draw_line_connecting_all_points       (ImelImage *image, ImelPoint **points, ImelPixel pxl);
extern bool             imel_draw_partial_reg_shape                (ImelImage *image, ImelSize x, ImelSize y, ImelSize r, long v, short p, 
                                                                    double start_angle, ImelPixel pxl);
extern void             imel_draw_point                            (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel);
extern void             imel_draw_point_from_array                 (ImelImage *image, ImelSize n_points, ImelPoint **points);
extern void             imel_draw_rect                             (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2, 
                                                                    ImelPixel pixel, bool fill);
extern bool             imel_draw_rect_with_rounded_angles         (ImelImage *image, ImelSize x1, ImelSize y1, ImelSize x2, ImelSize y2,
                                                                    ImelSize radius, ImelPixel pixel, bool fill);
extern bool             imel_draw_reg_shape                        (ImelImage *image, ImelSize x, ImelSize y, ImelSize r, long v, 
                                                                    double start_angle, ImelPixel pxl);
extern bool             imel_draw_spiral                           (ImelImage *image, ImelSize x, ImelSize y, ImelSize radius, 
                                                                    ImelSize distance, ImelPixel pxl);
                       
/** function @ file: src/point.c **/
extern void             imel_point_array_free                      (ImelPoint **points);
extern void             imel_point_free                            (ImelPoint *point);
extern ImelPoint       *imel_point_get_brightest_point             (ImelImage *image);
extern ImelPoint      **imel_point_get_brightest_points            (ImelImage *image);
extern ImelPoint       *imel_point_get_darkest_point               (ImelImage *image);
extern ImelPoint      **imel_point_get_darkest_points              (ImelImage *image);
extern ImelPoint      **imel_point_get_from_line                   (ImelSize _x1, ImelSize _y1, ImelSize _x2, ImelSize _y2, long int *lx, 
                                                                    long int *ly, ImelValue value_type, double value);
extern ImelPoint      **imel_point_get_from_reg_shape              (ImelSize x, ImelSize y, ImelSize r, long v, double start_angle);
extern ImelPoint       *imel_point_get_point_from_image            (ImelImage *image, ImelSize x, ImelSize y);
extern ImelPoint       *imel_point_new                             (ImelImage *image, ImelSize x, ImelSize y, ImelPixel pixel);

                                      
/** function @ file: src/font.c **/
extern void             imel_font_write_string                     (ImelImage *image, ImelSize x, ImelSize y, const char *string, 
                                                                    ImelSize px,  ImelPixel pixel);
extern void             imel_font_write_string_with_truetype_font  (ImelImage **image, char *ttf_file, ImelSize _x, ImelSize y,
                                                                    char *string, ImelSize px, ImelPixel pixel, ...);
extern void             imel_font_write_vstring                    (ImelImage *image, ImelSize x, ImelSize y, const char *string, 
                                                                    ImelSize px, ImelPixel pixel);
extern void             imel_font_write_vstring_with_truetype_font (ImelImage **image, char *ttf_file, ImelSize _x, ImelSize y,
                                                                    char *string, ImelSize px, ImelPixel pixel, ...);
                                                               
/** function @ file: src/value.c **/ 
extern double           imel_value_convert                         (ImelValue from_value, double value, ImelValue to_value, ...);
extern double           imel_value_percentage_to_generic           (double value, double opt_value);
extern double           imel_value_pixel_to_percentage             (double value, double opt_value);

/** function @ file: src/info_cut.c **/
extern ImelInfoCut     *imel_info_cut_add                          (ImelInfoCut *cut_info, ImelOrientation orientation, ImelSize position);
extern ImelInfoCut     *imel_info_cut_copy                         (ImelInfoCut *info_cut);
extern ImelInfoCut     *imel_info_cut_copy_index                   (ImelInfoCut *info_cut, ImelSize index);
extern ImelSize         imel_info_cut_count                        (ImelInfoCut *cut_info);
extern void             imel_info_cut_free                         (ImelInfoCut *cut_info);
extern ImelInfoCut     *imel_info_cut_get_index                    (ImelInfoCut *info_cut, ImelSize index);
extern ImelInfoCut     *imel_info_cut_get_max                      (ImelInfoCut *cut_info, ImelOrientation orientation);
extern ImelInfoCut     *imel_info_cut_get_min                      (ImelInfoCut *cut_info, ImelOrientation orientation);
extern ImelInfoCut     *imel_info_cut_get_next                     (ImelImage *image, ImelInfoCut *cut_info, ImelSize index);
extern ImelInfoCut     *imel_info_cut_get_prev                     (ImelImage *image, ImelInfoCut *cut_info, ImelSize index);
extern ImelSize         imel_info_cut_get_split                    (ImelImage *image, ImelInfoCut *cut_info, ImelOrientation orientation);
extern ImelInfoCut     *imel_info_cut_new                          (ImelOrientation orientation, ImelSize position);
extern ImelInfoCut     *imel_info_cut_remove_element               (ImelInfoCut *cut_info, ImelSize index);
extern void             imel_info_cut_swap_index                   (ImelInfoCut *cut_info, ImelSize index_a, ImelSize index_b);
                       
#endif
