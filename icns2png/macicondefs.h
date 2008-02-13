/*
    File:       icondefs.h

	Types / constants defined within this file are Copyright (c) 2003 Apple Computer Inc. 
*/

#ifndef __ICONDEFS__
#define __ICONDEFS__

#include "mactypes.h"

/* The following icon types can only be used as an icon element */
/* inside a 'icns' icon family */
enum {
  kThumbnail32BitData           = FOUR_CHAR_CODE('it32'),
  kThumbnail8BitMask            = FOUR_CHAR_CODE('t8mk')
};

enum {
  kHuge1BitMask                 = FOUR_CHAR_CODE('ich#'),
  kHuge4BitData                 = FOUR_CHAR_CODE('ich4'),
  kHuge8BitData                 = FOUR_CHAR_CODE('ich8'),
  kHuge32BitData                = FOUR_CHAR_CODE('ih32'),
  kHuge8BitMask                 = FOUR_CHAR_CODE('h8mk')
};

/* The following icon types can be used as a resource type */
/* or as an icon element type inside a 'icns' icon family */
enum {
  kLarge1BitMask                = FOUR_CHAR_CODE('ICN#'),
  kLarge4BitData                = FOUR_CHAR_CODE('icl4'),
  kLarge8BitData                = FOUR_CHAR_CODE('icl8'),
  kLarge32BitData               = FOUR_CHAR_CODE('il32'),
  kLarge8BitMask                = FOUR_CHAR_CODE('l8mk'),
  kSmall1BitMask                = FOUR_CHAR_CODE('ics#'),
  kSmall4BitData                = FOUR_CHAR_CODE('ics4'),
  kSmall8BitData                = FOUR_CHAR_CODE('ics8'),
  kSmall32BitData               = FOUR_CHAR_CODE('is32'),
  kSmall8BitMask                = FOUR_CHAR_CODE('s8mk'),
  kMini1BitMask                 = FOUR_CHAR_CODE('icm#'),
  kMini4BitData                 = FOUR_CHAR_CODE('icm4'),
  kMini8BitData                 = FOUR_CHAR_CODE('icm8')
};

enum {
    kHuge1BitData					= kHuge1BitMask,
    kLarge1BitData					= kLarge1BitMask,
    kSmall1BitData					= kSmall1BitMask,
    kMini1BitData					= kMini1BitMask
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
  kIconFamilyType               = FOUR_CHAR_CODE('icns')
};

struct IconFamilyElement {
  OSType              elementType;            /* 'ICN#', 'icl8', etc...*/
  Size                elementSize;            /* Size of this element*/
  unsigned char       elementData[1];
};
typedef struct IconFamilyElement        IconFamilyElement;
typedef IconFamilyElement *				IconFamilyElementPtr;

struct IconFamilyResource {
  OSType              resourceType;           /* Always 'icns'*/
  Size                resourceSize;           /* Total size of this resource*/
  IconFamilyElement   elements[1];

};

typedef struct IconFamilyResource       IconFamilyResource;
typedef IconFamilyResource *            IconFamilyPtr;
typedef IconFamilyPtr *                 IconFamilyHandle;


#endif /* __ICONDEFS__ */
