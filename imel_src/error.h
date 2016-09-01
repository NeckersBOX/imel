/*
 * "error.h" (C) Davide Francesco "HdS619" Merico ( hds619@gmail.com )
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

#ifndef error_header
#define error_header
/**
 * @file error.h
 * @author Davide Francesco Merico
 * @brief This file contains error codes used by Imel library.
 *
 */
 
#define IMEL_ERR_LOAD             0xa0 /**< Error while loading the image */

#define IMEL_ERR_JPEG_LOAD        0x60 /**< Error while loading the jpeg image */
#define IMEL_ERR_PNG_LOAD         0x61 /**< Error while loading the png image */
#define IMEL_ERR_TIFF_LOAD        0x62 /**< Error while loading the tiff image */
#define IMEL_ERR_BMP_LOAD         0x63 /**< Error while loading the bmp image */
#define IMEL_ERR_DDS_LOAD         0x64 /**< Error while loading the dds image */
#define IMEL_ERR_FAXG3_LOAD       0x65 /**< Error while loading the faxg3 image */
#define IMEL_ERR_CUT_LOAD         0x66 /**< Error while loading the cut image */
#define IMEL_ERR_XPM_LOAD         0x67 /**< Error while loading the xpm image */
#define IMEL_ERR_XBM_LOAD         0x68 /**< Error while loading the xbm image */
#define IMEL_ERR_SGI_LOAD         0x69 /**< Error while loading the sgi image */
#define IMEL_ERR_WBMP_LOAD        0x70 /**< Error while loading the wbmp image */
#define IMEL_ERR_HDR_LOAD         0x71 /**< Error while loading the hdr image */
#define IMEL_ERR_PSD_LOAD         0x72 /**< Error while loading the psd image */
#define IMEL_ERR_IFF_LOAD         0x73 /**< Error while loading the iff image */
#define IMEL_ERR_JNG_LOAD         0x74 /**< Error while loading the jng image */
#define IMEL_ERR_KOALA_LOAD       0x75 /**< Error while loading the koala image */
#define IMEL_ERR_MNG_LOAD         0x76 /**< Error while loading the mng image */
#define IMEL_ERR_PCX_LOAD         0x77 /**< Error while loading the pcx image */
#define IMEL_ERR_PGM_LOAD         0x78 /**< Error while loading the pgm image */
#define IMEL_ERR_PGMRAW_LOAD      0x79 /**< Error while loading the pgmraw image */
#define IMEL_ERR_RAS_LOAD         0x80 /**< Error while loading the ras image */
#define IMEL_ERR_EXR_LOAD         0x81 /**< Error while loading the exr image */
#define IMEL_ERR_J2K_LOAD         0x82 /**< Error while loading the j2k image */
#define IMEL_ERR_PPM_LOAD         0x83 /**< Error while loading the ppm image */
#define IMEL_ERR_PPMRAW_LOAD      0x84 /**< Error while loading the ppmraw image */
#define IMEL_ERR_PBM_LOAD         0x85 /**< Error while loading the pbm image */
#define IMEL_ERR_PBMRAW_LOAD      0x86 /**< Error while loading the pbmraw image */
#define IMEL_ERR_TARGA_LOAD       0x87 /**< Error while loading the targa image */
#define IMEL_ERR_JP2_LOAD         0x88 /**< Error while loading the jp2 image */
#define IMEL_ERR_ICO_LOAD         0x89 /**< Error while loading the ico image */
#define IMEL_ERR_PCD_LOAD         0x90 /**< Error while loading the pcd image */
#define IMEL_ERR_GIF_LOAD         0x91 /**< Error while loading the gif image */

#define IMEL_ERR_PNG_WRITE_STRUCT 0x10 /**< Could not create a PNG write structure (out of memory?) */
#define IMEL_ERR_PNG_INFO_STRUCT  0x11 /**< Could not create PNG info structure (out of memory?) */
#define IMEL_ERR_PNG_SET_JMP      0x12 /**< Could not set PNG jump value */

#define IMEL_ERR_RAW_BPP          0x52 /**< The bpp are too big for Imel. */
#define IMEL_ERR_RAW_LENGTH       0x53 /**< The bpp aren't multiples of 8. */
#define IMEL_ERR_RAW_FILE_LENGTH  0x54 /**< The file length isn't valid. */
#define IMEL_ERR_RAW_BPP2         0x55 /**< bpp equal to zero ( not valid ). */

#endif
