/*
File:       icns.h
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
Copyright (C) 2002 Chenxiao Zhao <chenxiao.zhao@gmail.com>

With the exception of the limited portions mentiond, this library
is free software; you can redistribute it and/or modify it under
the terms of the GNU Library General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _ICNS_H_
#define	_ICNS_H_

/*  Compile-time variables   */
/*  These should really be set from the Makefile */
// Enable debugging messages?
// #define	ICNS_DEBUG

// Enable openjpeg for 256x256 and 512x512 support
// #define	ICNS_OPENJPEG


/*  OpenJPEG headers   */
#ifdef ICNS_OPENJPEG
#include <openjpeg.h>

/*  OpenJPEG version check   */
// OPENJPEG_VERSION seems to be a reliable test for having
// the proper openjpeg header files.
#ifndef OPENJPEG_VERSION
	#warning "libicns: Can't determine OpenJPEG version."
	#warning "libicns: 256x256 and 512x512 support will not be available."
	#undef	ICNS_OPENJPEG
#endif
#endif

/*  icns element type constants */

#define ICNS_512x512_32BIT_ARGB_DATA  0x69633039 /* ic09 */
#define ICNS_256x256_32BIT_ARGB_DATA  0x69633038 /* ic08 */

#define ICNS_128X128_32BIT_DATA	      0x69743332 /* it32 */
#define ICNS_128X128_8BIT_MASK	      0x74386D6B /* t8mk */

#define ICNS_48x48_1BIT_DATA	      0x69636823 /* ich# */
#define ICNS_48x48_4BIT_DATA          0x69636834 /* ich4 */
#define ICNS_48x48_8BIT_DATA          0x69636838 /* ich8 */
#define ICNS_48x48_32BIT_DATA	      0x69683332 /* ih32 */
#define ICNS_48x48_1BIT_MASK          0x69636823 /* ich# */
#define ICNS_48x48_8BIT_MASK          0x68386D6B /* h8mk */

#define ICNS_32x32_1BIT_DATA	      0x49434E23 /* ICN# */
#define ICNS_32x32_4BIT_DATA          0x69636C34 /* icl4 */
#define ICNS_32x32_8BIT_DATA          0x69636C38 /* icl8 */
#define ICNS_32x32_32BIT_DATA         0x696C3332 /* il32 */
#define ICNS_32x32_1BIT_MASK          0x49434E23 /* ICN# */
#define ICNS_32x32_8BIT_MASK          0x6C386D6B /* l8mk */

#define ICNS_16x16_1BIT_DATA	      0x69637323 /* ics# */
#define ICNS_16x16_4BIT_DATA          0x69637334 /* ics4 */
#define ICNS_16x16_8BIT_DATA          0x69637338 /* ics8 */
#define ICNS_16x16_32BIT_DATA         0x69733332 /* is32 */
#define ICNS_16x16_1BIT_MASK          0x69637323 /* ics# */
#define ICNS_16x16_8BIT_MASK          0x73386D6B /* s8mk */

#define ICNS_16x12_1BIT_DATA          0x69636D23 /* icm# */
#define ICNS_16x12_4BIT_DATA          0x69636D34 /* icm4 */
#define ICNS_16x12_1BIT_MASK          0x69636D23 /* icm# */
#define ICNS_16x12_8BIT_DATA          0x69636D38 /* icm8 */

/* other mac icon types */

#define ICNS_32x32_1BIT_ICON	      0x49434F4E /* ICON */

#define ICNS_INVALID_DATA             0x00000000
#define ICNS_INVALID_MASK             0x00000000

#define ICNS_FAMILY_TYPE              0x69636E73 /* icns */


/* icns data types */

typedef unsigned char   icns_bool_t;

typedef unsigned char   icns_uint8_t;
typedef signed char     icns_sint8_t;
typedef unsigned short  icns_uint16_t;
typedef signed short    icns_sint16_t;
typedef unsigned int    icns_uint32_t;
typedef signed int      icns_sint32_t;
typedef unsigned long   icns_uint64_t;
typedef signed long     icns_sint64_t;

typedef icns_uint32_t   icns_size_t;
typedef icns_uint32_t   icns_type_t;

typedef struct icns_element_t {
  icns_type_t           elementType;    /* 'ICN#', 'icl8', etc...- BIG ENDIAN! */
  icns_size_t           elementSize;    /* Total size of element - BIG ENDIAN! */
  unsigned char         elementData[1];
} icns_element_t;

typedef struct icns_family_t {
  icns_type_t           resourceType;	/* Always should be 'icns'- BIG ENDIAN! */
  icns_size_t           resourceSize;	/* Total size of resource - BIG ENDIAN! */
  icns_element_t        elements[1];
} icns_family_t;

typedef struct icns_image_t
{
	long                imageDataSize;  // bytes = width * height * depth / bits-per-pixel
	int                 imageWidth;     // width of image in pixels
	int                 imageHeight;    // height of image in pixels
	short               imageChannels;  // number of channels in data
	short               pixel_depth;    // number of bits-per-pixel
	unsigned char       *imageData;     // pointer to base address of uncompressed raw image data
} icns_image_t;

typedef struct icns_pixel32_t
{
	unsigned char	 alpha;
	unsigned char	 red;
	unsigned char	 green;
	unsigned char	 blue;
} icns_pixel32_t;

typedef struct icns_pixel32_swap_t
{
	unsigned char	 blue;
	unsigned char	 green;
	unsigned char	 red;
	unsigned char	 alpha;
} icns_pixel32_swap_t;

typedef struct icns_rgb_t
{
	unsigned char	 red;
	unsigned char	 green;
	unsigned char	 blue;
} icns_rgb_t;


/* icns constants */

#define			icns_byte_bits	8


/* icns macros */

/*
These macros will will on systems that support unaligned
accesses, as well as those that don't support it
*/

#define ICNS_WRITE_UNALIGNED(addr, val, type)       memcpy(addr, &val, sizeof(type))
#define ICNS_READ_UNALIGNED(val, addr, type)        memcpy(&val, addr, sizeof(type))

/*
If we find a way to determine whether we have aligned
access or not at compile-time, this could also be defined for
systems that don't care instead of the above...

#define ICNS_WRITE_UNALIGNED(addr, val, type)       ( *((type *)(addr)) = val )
#define ICNS_READ_UNALIGNED(val, addr, type)        ( val = *((type *)(addr)) )

*/

/* icns function prototypes */

// icns_debug.c
void bin_print_byte(int x);
void bin_print_int(int x);

// icns_image.c
icns_type_t icns_get_mask_type_for_icon_type(icns_type_t);
int icns_get_image32_with_mask_from_family(icns_family_t *iconFamily,icns_type_t sourceType,icns_image_t *imageOut);
int icns_get_image32_from_element(icns_element_t *iconElement,icns_image_t *imageOut);
int icns_get_image_from_element(icns_element_t *iconElement,icns_image_t *imageOut);
int icns_init_image_for_type(icns_type_t iconType,icns_image_t *imageOut);
int icns_init_image(unsigned int iconWidth,unsigned int iconHeight,unsigned int iconChannels,unsigned int iconPixelDepth,icns_image_t *imageOut);
int icns_free_image(icns_image_t *imageIn);

// icns_rle24.c
int icns_decode_rle24_data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long *dataOutSize, icns_sint32_t **dataOutPtr);
int icns_encode_rle24_data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long *dataOutSize, icns_sint32_t **dataOutPtr);

// icns_jp2.c
#ifdef ICNS_OPENJPEG
void icns_opj_error_callback(const char *msg, void *client_data);
void icns_opj_warning_callback(const char *msg, void *client_data);
void icns_opj_info_callback(const char *msg, void *client_data);
int icns_opj_to_image(opj_image_t *image, icns_image_t *outIcon);
opj_image_t * jp2dec(unsigned char *bufin, int len);
#endif

// icns_element.c
int icns_new_element_from_image(icns_image_t *imageIn,icns_type_t iconType,icns_element_t **iconElementOut);
int icns_get_element_from_family(icns_family_t *iconFamily,icns_type_t iconType,icns_element_t **iconElementOut);
int icns_set_element_in_family(icns_family_t **iconFamilyRef,icns_element_t *newIconElement);
int icns_remove_element_in_family(icns_family_t **iconFamilyRef,icns_type_t iconType);

// icns_family.c
int icns_create_family(icns_family_t **iconFamilyOut);

// icns_io.c
int icns_write_family_to_file(FILE *dataFile,icns_family_t *iconFamilyIn);
int icns_read_family_from_file(FILE *dataFile,icns_family_t **iconFamilyOut);
int icns_family_from_file_data(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int icns_family_from_mac_resource(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int icns_parse_macbinary_resource_fork(unsigned long dataSize,unsigned char *data,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,unsigned long *parsedResSizeOut,unsigned char **parsedResDataOut);

#endif
