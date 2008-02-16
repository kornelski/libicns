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
#include <png.h>
#include <openjpeg.h>

#ifndef _ICNS_H_
#define	_ICNS_H_	1

/*  icns element type constants */

#define ICNS_512x512_32BIT_ARGB_DATA  0x69633039 /* ic09 */
#define ICNS_256x256_32BIT_ARGB_DATA  0x69633038 /* ic08 */

#define ICNS_128X128_32BIT_DATA	      0x69743332 /* it32 */
#define ICNS_128X128_8BIT_MASK	      0x74386D6B /* t8mk */

#define ICNS_64x64_1BIT_MASK          0x69636823 /* ich# */
#define ICNS_64x64_1BIT_DATA	      ICNS_64x64_1BIT_MASK
#define ICNS_64x64_4BIT_DATA          0x69636834 /* ich4 */
#define ICNS_64x64_8BIT_DATA          0x69636838 /* ich8 */
#define ICNS_64x64_32BIT_DATA	      0x69683332 /* ih32 */
#define ICNS_64x64_8BIT_MASK          0x68386D6B /* h8mk */

#define ICNS_32x32_1BIT_MASK          0x49434E23 /* ICN# */
#define ICNS_32x32_1BIT_DATA	      ICNS_32x32_1BIT_MASK
#define ICNS_32x32_4BIT_DATA          0x69636C34 /* icl4 */
#define ICNS_32x32_8BIT_DATA          0x69636C38 /* icl8 */
#define ICNS_32x32_32BIT_DATA         0x696C3332 /* il32 */
#define ICNS_32x32_8BIT_MASK          0x6C386D6B /* l8mk */

#define ICNS_16x16_1BIT_MASK          0x69637323 /* ics# */
#define ICNS_16x16_1BIT_DATA	      ICNS_16x16_1BIT_MASK
#define ICNS_16x16_4BIT_DATA          0x69637334 /* ics4 */
#define ICNS_16x16_8BIT_DATA          0x69637338 /* ics8 */
#define ICNS_16x16_32BIT_DATA         0x69733332 /* is32 */
#define ICNS_16x16_8BIT_MASK          0x73386D6B /* s8mk */

#define ICNS_16x12_1BIT_MASK          0x69636D23 /* icm# */
#define ICNS_16x12_1BIT_DATA          ICNS_16x12_1BIT_MASK
#define ICNS_16x12_4BIT_DATA          0x69636D34 /* icm4 */
#define ICNS_16x12_8BIT_DATA          0x69636D38 /* icm8 */


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
  icns_type_t           elementType;    /* 'ICN#', 'icl8', etc...*/
  icns_size_t           elementSize;    /* Size of this element*/
  unsigned char         elementData[1];
} icns_element_t;

typedef struct icns_family_t {
  icns_type_t           resourceType;	/* Always seems to be 'icns' */
  icns_size_t           resourceSize;	/* Total size of this resource*/
  icns_element_t        elements[1];
} icns_family_t;

typedef struct icns_image_t
{
	long		imageDataSize;	// bytes = width * height * depth / bits-per-pixel
	int		imageWidth;	// width of image in pixels
	int		imageHeight;	// height of image in pixels
	short		imageChannels;	// number of channels in data
	short		imageDepth;	// channels * bits-per-pixel
	unsigned char	*imageData;	// pointer to base address of uncompressed raw image data
} icns_image_t;

typedef struct icns_pixel32_t
{
	unsigned char	 alpha;
	unsigned char	 red;
	unsigned char	 green;
	unsigned char	 blue;
} icns_pixel32_t;

/* icns constants */

#define			icns_byte_bits	8

/* icns function prototypes */

int GetICNSImage32FromICNSElement(icns_element_t *iconElement, icns_bool_t byteSwap,icns_image_t *imageOut);
int GetICNSImageFromICNSElement(icns_element_t *iconElement, icns_bool_t byteSwap,icns_image_t *imageOut);
int GetICNSElementFromICNSFamily(icns_family_t *iconFamily,icns_type_t iconType, icns_bool_t *byteSwap,icns_element_t **iconElementOut);
int GetICNSFamilyFromFileData(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int GetICNSFamilyFromMacResource(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int ParseMacBinaryResourceFork(unsigned long dataSize,unsigned char *data,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,unsigned long *parsedResSizeOut,unsigned char **parsedResDataOut);

#endif
