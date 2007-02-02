/*

File: iconvert.cpp
Copyright (C) 2006 Paul Wise

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

*/

#ifndef _COMMON_H_
#define _COMMON_H_ 1


#include <stdint.h>

typedef int bool;
#define true 1
#define false 0

typedef char * Ptr;
typedef Ptr * Handle;

typedef uint32_t FourCharCode;
typedef FourCharCode OSType;
typedef FourCharCode ResType;

typedef	OSType* OSTypePtr;

struct IconFamilyElement {
	OSType elementType; /* ICN#, icl8, ...*/
	size_t elementSize;
	unsigned char elementData[1];
};
typedef struct IconFamilyElement IconFamilyElement;
typedef IconFamilyElement * IconFamilyElementPtr;

struct IconFamilyResource {
	OSType resourceType; /* icns */
	size_t resourceSize;
	IconFamilyElement elements[1];
};

typedef struct IconFamilyResource IconFamilyResource;
typedef IconFamilyResource * IconFamilyPtr;
typedef IconFamilyPtr* IconFamilyHandle;

#define kIconFamilyType 0x69636E73 /* icns */

#define kThumbnail32BitData 0x69743332 /* it32 */
#define kThumbnail8BitMask 0x74386D6B /* t8mk */

#define kHuge1BitMask 0x69636823 /* ich# */
#define kHuge4BitData 0x69636834 /* ich4 */
#define kHuge8BitData 0x69636838 /* ich8 */
#define kHuge32BitData 0x69683332 /* ih32 */
#define kHuge8BitMask 0x68386D6B /* h8mk */

#define kLarge1BitMask 0x49434E23 /* ICN# */
#define kLarge4BitData 0x69636C34 /* icl4 */
#define kLarge8BitData 0x69636C38 /* icl8 */
#define kLarge32BitData 0x696C3332 /* il32 */
#define kLarge8BitMask 0x6C386D6B /* l8mk */
#define kSmall1BitMask 0x69637323 /* ics# */
#define kSmall4BitData 0x69637334 /* ics4 */
#define kSmall8BitData 0x69637338 /* ics8 */
#define kSmall32BitData 0x69733332 /* is32 */
#define kSmall8BitMask 0x73386D6B /* s8mk */
#define kMini1BitMask 0x69636D23 /* icm# */
#define kMini4BitData 0x69636D34 /* icm4 */
#define kMini8BitData 0x69636D38 /* icm8 */

#define kHuge1BitData kHuge1BitMask
#define kLarge1BitData kLarge1BitMask
#define kSmall1BitData kSmall1BitMask
#define kMini1BitData kMini1BitMask

#define kSelectorLarge1Bit 0x00000001
#define kSelectorLarge4Bit 0x00000002
#define kSelectorLarge8Bit 0x00000004
#define kSelectorLarge32Bit 0x00000008
#define kSelectorLarge8BitMask 0x00000010
#define kSelectorSmall1Bit 0x00000100
#define kSelectorSmall4Bit 0x00000200
#define kSelectorSmall8Bit 0x00000400
#define kSelectorSmall32Bit 0x00000800
#define kSelectorSmall8BitMask 0x00001000
#define kSelectorMini1Bit 0x00010000
#define kSelectorMini4Bit 0x00020000
#define kSelectorMini8Bit 0x00040000
#define kSelectorHuge1Bit 0x01000000
#define kSelectorHuge4Bit 0x02000000
#define kSelectorHuge8Bit 0x04000000
#define kSelectorHuge32Bit 0x08000000
#define kSelectorHuge8BitMask 0x10000000
#define kSelectorAllLargeData 0x000000FF
#define kSelectorAllSmallData 0x0000FF00
#define kSelectorAllMiniData 0x00FF0000
#define kSelectorAllHugeData (long)0xFF000000
#define kSelectorAll1BitData kSelectorLarge1Bit | kSelectorSmall1Bit | kSelectorMini1Bit | kSelectorHuge1Bit
#define kSelectorAll4BitData kSelectorLarge4Bit | kSelectorSmall4Bit | kSelectorMini4Bit | kSelectorHuge4Bit
#define kSelectorAll8BitData kSelectorLarge8Bit | kSelectorSmall8Bit | kSelectorMini8Bit | kSelectorHuge8Bit
#define kSelectorAll32BitData kSelectorLarge32Bit | kSelectorSmall32Bit | kSelectorHuge32Bit
#define kSelectorAllAvailableData (long)0xFFFFFFFF


#endif /* _COMMON_H_ */
