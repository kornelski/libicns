/*
File:       icns.h
Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
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

/*
Section:       apple_mactypes.h

The following section is based on information found in the file
MacTypes.h originating in the 2002, Quicktime 6.0.2 developer's
kit as developed by Apple Computer, Inc.

Modifications have been made to make it more compatible with the
GNU gcc and g++ compilers, and to limit the information to that
necessary for the icns format. To be clear, bug reports on this
file should NOT be filed to Apple Computer, Inc. This is a work
in derivative, not the original work.

All data types and naming conventions in this file were created and
Copyright: (c) 1985-2001 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __APPLE_MACTYPES__
#define	__APPLE_MACTYPES__	1

#define FOUR_CHAR_CODE(x)           	(unsigned long)(x)

typedef unsigned char                   UInt8;
typedef signed char                     SInt8;
typedef unsigned short                  UInt16;
typedef signed short                    SInt16;
typedef unsigned long                   UInt32;
typedef signed long                     SInt32;

typedef char *                          Ptr;
typedef Ptr *                           Handle;
typedef long                            Size;

typedef unsigned long                   FourCharCode;
typedef FourCharCode                    OSType;
typedef FourCharCode                    ResType;

typedef	OSType *			OSTypePtr;

#endif

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

struct IconFamilyElement {
  OSType              elementType;		/* 'ICN#', 'icl8', etc...*/
  Size                elementSize;		/* Size of this element*/
  unsigned char       elementData[1];
};
typedef struct IconFamilyElement        IconFamilyElement;
typedef IconFamilyElement *		IconFamilyElementPtr;

struct IconFamilyResource {
	OSType              resourceType;	/* Always seems to be 'icns' */
	Size                resourceSize;	/* Total size of this resource*/
	IconFamilyElement   elements[1];
};

typedef struct IconFamilyResource       IconFamilyResource;
typedef IconFamilyResource *            IconFamilyPtr;
typedef IconFamilyPtr *                 IconFamilyHandle;


#endif /* __APPLE_ICONS__ */

/*
Section:       icns.h
Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
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

#define	kICNS_ByteSize	8

typedef struct ICNS_IconData {
	OSType type;
	size_t size;
	unsigned char *data;
} ICNS_IconData;

typedef struct ICNS_ImageData
{
	int		width;		// width of image
	int		height;		// height of image
	short		channels;	// number of channels in data
	short		depth;		// bits-per-pixel * channels
	long		dataSize;	// bytes = width * height * depth / bits-per-pixel
	unsigned char	*iconData;
} ICNS_ImageData, *ICNS_ImageDataPtr;

int GetIconFamilyFromFileData(long dataSize,char *dataPtr,IconFamilyPtr *iconFamily);
int GetIconFamilyFromMacResourceFork(long dataSize,char *dataPtr,IconFamilyPtr *iconFamily);
int ParseMacBinaryResourceFork(long dataSize,char *dataPtr,OSType *dataType, OSType *dataCreator,long *parsedResSize,char **parsedResData);
int GetIconDataFromIconFamily(IconFamilyPtr inPtr,ResType iconType,ICNS_IconData *outIcon, int *byteswap);
int ParseIconData(ResType iconType,Ptr rawDataPtr,long rawDataLength,ICNS_ImageDataPtr outIcon, int byteSwap);

#endif
