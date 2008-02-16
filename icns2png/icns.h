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

/*
Section:       apple_iconstorage.h

The following section is based on information found in the file
IconStorage.h as developed by Apple Computer, Inc.

Modifications have been made to make it more compatible with the
GNU gcc and g++ compilers, and to limit the information to that
necessary for the icns format. To be clear, bug reports on this
file should NOT be filed to Apple Computer, Inc. This is a work
in derivative, not the original work.

All data types and naming conventions in this file were created and
Copyright: (c) 1985-2007 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __APPLE_ICONSTORAGE__
#define __APPLE_ICONSTORAGE__

#define kIconServices512PixelDataARGB 0x69633039 /* ic09 */
#define kIconServices256PixelDataARGB 0x69633038 /* ic08 */

#endif /* __APPLE_ICONSTORAGE__ */

/*
Section:       apple_icons.h

The following section is based on information found in the Icons.h
file originating in the 2002, Quicktime 6.0.2 developer's kit as
developed by Apple Computer, Inc.

Modifications have been made to make it more compatible with the
GNU gcc and g++ compilers, and to limit the information to that
necessary for the icns format. To be clear, bug reports on this
file should NOT be filed to Apple Computer, Inc. This is a work
in derivative, not the original work.

All data types and naming conventions in this file were created and
Copyright: (c) 1985-2001 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __APPLE_ICONS__
#define __APPLE_ICONS__

/* The following icon types can only be used as an icon element */
/* inside a 'icns' icon family */
enum {
	kThumbnail32BitData	= 0x69743332,	/* it32 */
	kThumbnail8BitMask	= 0x74386D6B	/* t8mk */
};

enum {
	kHuge1BitMask		= 0x69636823, 	/* ich# */
	kHuge4BitData		= 0x69636834, 	/* ich4 */
	kHuge8BitData		= 0x69636838, 	/* ich8 */
	kHuge32BitData		= 0x69683332, 	/* ih32 */
	kHuge8BitMask		= 0x68386D6B	/* h8mk */
};

/* The following icon types can be used as a resource type */
/* or as an icon element type inside a 'icns' icon family */
enum {
	kLarge1BitMask		= 0x49434E23, 	/* ICN# */
	kLarge4BitData		= 0x69636C34, 	/* icl4 */
	kLarge8BitData		= 0x69636C38, 	/* icl8 */
	kLarge32BitData		= 0x696C3332, 	/* il32 */
	kLarge8BitMask		= 0x6C386D6B, 	/* l8mk */
	kSmall1BitMask		= 0x69637323, 	/* ics# */
	kSmall4BitData		= 0x69637334, 	/* ics4 */
	kSmall8BitData		= 0x69637338, 	/* ics8 */
	kSmall32BitData		= 0x69733332, 	/* is32 */
	kSmall8BitMask		= 0x73386D6B, 	/* s8mk */
	kMini1BitMask		= 0x69636D23, 	/* icm# */
	kMini4BitData		= 0x69636D34, 	/* icm4 */
	kMini8BitData		= 0x69636D38	/* icm8 */
};

enum {
    kHuge1BitData		= kHuge1BitMask,
    kLarge1BitData		= kLarge1BitMask,
    kSmall1BitData		= kSmall1BitMask,
    kMini1BitData		= kMini1BitMask
};

/* Selector mask values. */
enum {
  kSelectorLarge1Bit            = 0x00000001,
  kSelectorLarge4Bit            = 0x00000002,
  kSelectorLarge8Bit            = 0x00000004,
  kSelectorLarge32Bit           = 0x00000008,
  kSelectorLarge8BitMask        = 0x00000010,
  kSelectorSmall1Bit            = 0x00000100,
  kSelectorSmall4Bit            = 0x00000200,
  kSelectorSmall8Bit            = 0x00000400,
  kSelectorSmall32Bit           = 0x00000800,
  kSelectorSmall8BitMask        = 0x00001000,
  kSelectorMini1Bit             = 0x00010000,
  kSelectorMini4Bit             = 0x00020000,
  kSelectorMini8Bit             = 0x00040000,
  kSelectorHuge1Bit             = 0x01000000,
  kSelectorHuge4Bit             = 0x02000000,
  kSelectorHuge8Bit             = 0x04000000,
  kSelectorHuge32Bit            = 0x08000000,
  kSelectorHuge8BitMask         = 0x10000000,
  kSelectorAllLargeData         = 0x000000FF,
  kSelectorAllSmallData         = 0x0000FF00,
  kSelectorAllMiniData          = 0x00FF0000,
  kSelectorAllHugeData          = (long)0xFF000000,
  kSelectorAll1BitData          = kSelectorLarge1Bit | kSelectorSmall1Bit | kSelectorMini1Bit | kSelectorHuge1Bit,
  kSelectorAll4BitData          = kSelectorLarge4Bit | kSelectorSmall4Bit | kSelectorMini4Bit | kSelectorHuge4Bit,
  kSelectorAll8BitData          = kSelectorLarge8Bit | kSelectorSmall8Bit | kSelectorMini8Bit | kSelectorHuge8Bit,
  kSelectorAll32BitData         = kSelectorLarge32Bit | kSelectorSmall32Bit | kSelectorHuge32Bit,
  kSelectorAllAvailableData     = (long)0xFFFFFFFF
};

enum {
  kIconFamilyType		= 0x69636E73	/* icns */
};

#endif /* __APPLE_ICONS__ */

/*
Section:       icns.h
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

#ifndef _ICNS_H_
#define	_ICNS_H_	1

#define			icns_byte_bits	8

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

int GetICNSImage32FromICNSElement(icns_element_t *iconElement, icns_bool_t byteSwap,icns_image_t *imageOut);
int GetICNSImageFromICNSElement(icns_element_t *iconElement, icns_bool_t byteSwap,icns_image_t *imageOut);
int GetICNSElementFromICNSFamily(icns_family_t *iconFamily,icns_type_t iconType, icns_bool_t *byteSwap,icns_element_t **iconElementOut);
int GetICNSFamilyFromFileData(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int GetICNSFamilyFromMacResource(unsigned long dataSize,unsigned char *data,icns_family_t **iconFamilyOut);
int ParseMacBinaryResourceFork(unsigned long dataSize,unsigned char *data,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,unsigned long *parsedResSizeOut,unsigned char **parsedResDataOut);

#endif
