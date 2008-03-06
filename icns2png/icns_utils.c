/*
File:       icns_utils.c
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>

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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>


#include "icns.h"
#include "icns_internals.h"


/********* This variable is intentionally global ************/
/********* scope is the internals of the icns library *******/
#ifdef ICNS_DEBUG
icns_bool_t	gShouldPrintErrors = 1;
#else
icns_bool_t	gShouldPrintErrors = 0;
#endif


icns_type_t icns_get_mask_type_for_icon_type(icns_type_t iconType)
{
	// 32-bit image types > 256x256 - no mask (mask is already in image)
	if(icns_types_equal(iconType,ICNS_512x512_32BIT_ARGB_DATA)) {
		return ICNS_NULL_DATA;			
	} else if(icns_types_equal(iconType,ICNS_256x256_32BIT_ARGB_DATA)) {
		return ICNS_NULL_DATA;		
	}
	
	// 32-bit image types - 8-bit mask type
	else if(icns_types_equal(iconType,ICNS_128X128_32BIT_DATA)) {
		return ICNS_128X128_8BIT_MASK;	
	} else if(icns_types_equal(iconType,ICNS_48x48_32BIT_DATA)) {
		return ICNS_48x48_8BIT_MASK;	
	} else if(icns_types_equal(iconType,ICNS_32x32_32BIT_DATA)) {
		return ICNS_32x32_8BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x16_32BIT_DATA)) {
		return ICNS_16x16_8BIT_MASK;
	}
	
	// 8-bit image types - 1-bit mask types
	else if(icns_types_equal(iconType,ICNS_48x48_8BIT_DATA)) {
		return ICNS_48x48_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_32x32_8BIT_DATA)) {
		return ICNS_32x32_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x16_8BIT_DATA)) {
		return ICNS_16x16_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x12_8BIT_DATA)) {
		return ICNS_16x12_1BIT_MASK;
	}
	
	// 4 bit image types - 1-bit mask types
	else if(icns_types_equal(iconType,ICNS_48x48_4BIT_DATA)) {
		return ICNS_48x48_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_32x32_4BIT_DATA)) {
		return ICNS_32x32_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x16_4BIT_DATA)) {
		return ICNS_16x16_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x12_4BIT_DATA)) {
		return ICNS_16x12_1BIT_MASK;
	}
	
	// 1 bit image types - 1-bit mask types
	else if(icns_types_equal(iconType,ICNS_48x48_1BIT_DATA)) {
		return ICNS_48x48_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_32x32_1BIT_DATA)) {
		return ICNS_32x32_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x16_1BIT_DATA)) {
		return ICNS_16x16_1BIT_MASK;
	} else if(icns_types_equal(iconType,ICNS_16x12_1BIT_DATA)) {
		return ICNS_16x12_1BIT_MASK;
	}
	else
	{
		return ICNS_NULL_DATA;
	}
	
	return ICNS_NULL_DATA;
}


icns_icon_image_info_t icns_get_image_info_for_type(icns_type_t iconType)
{
	icns_icon_image_info_t iconInfo;
	
	memset(&iconInfo,0,sizeof(iconInfo));
	
	if(icns_types_equal(iconType,ICNS_NULL_TYPE))
	{
		icns_print_err("icns_get_image_info_for_type: Unable to parse NULL type!\n");
		return iconInfo;
	}
	
	#ifdef ICNS_DEBUG
	printf("Retrieving info for type '%c%c%c%c'...\n",iconType.c[0],iconType.c[1],iconType.c[2],iconType.c[3]);
	#endif
	
	iconInfo.iconType = iconType;
	
	// 32-bit image types
	if(icns_types_equal(iconType,ICNS_512x512_32BIT_ARGB_DATA)) {
		iconInfo.iconWidth = 512;
		iconInfo.iconHeight = 512;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;			
	} else if(icns_types_equal(iconType,ICNS_256x256_32BIT_ARGB_DATA)) {
		iconInfo.iconWidth = 256;
		iconInfo.iconHeight = 256;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;		
	} else if(icns_types_equal(iconType,ICNS_128X128_32BIT_DATA)) {
		iconInfo.iconWidth = 128;
		iconInfo.iconHeight = 128;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;	
	} else if(icns_types_equal(iconType,ICNS_48x48_32BIT_DATA)) {
		iconInfo.iconWidth = 48;
		iconInfo.iconHeight = 48;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;	
	} else if(icns_types_equal(iconType,ICNS_32x32_32BIT_DATA)) {
		iconInfo.iconWidth = 32;
		iconInfo.iconHeight = 32;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;
	} else if(icns_types_equal(iconType,ICNS_16x16_32BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 16;
		iconInfo.iconChannels = 4;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 32;
	}
	
	// 8-bit mask types
	else if(icns_types_equal(iconType,ICNS_128X128_8BIT_MASK)) {
		iconInfo.iconWidth = 128;
		iconInfo.iconHeight = 128;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_48x48_8BIT_MASK)) {
		iconInfo.iconWidth = 48;
		iconInfo.iconHeight = 48;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_32x32_8BIT_MASK)) {
		iconInfo.iconWidth = 32;
		iconInfo.iconHeight = 32;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_16x16_8BIT_MASK)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 16;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	}
	
	
	// 8-bit image types
	else if(icns_types_equal(iconType,ICNS_48x48_8BIT_DATA)) {
		iconInfo.iconWidth = 48;
		iconInfo.iconHeight = 48;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_32x32_8BIT_DATA)) {
		iconInfo.iconWidth = 32;
		iconInfo.iconHeight = 32;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_16x16_8BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 16;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	} else if(icns_types_equal(iconType,ICNS_16x12_8BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 12;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 8;
		iconInfo.iconBitDepth = 8;
	}
	
	// 4 bit image types
	 else if(icns_types_equal(iconType,ICNS_48x48_4BIT_DATA)) {
		iconInfo.iconWidth = 48;
		iconInfo.iconHeight = 48;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 4;
		iconInfo.iconBitDepth = 4;
	} else if(icns_types_equal(iconType,ICNS_32x32_4BIT_DATA)) {
		iconInfo.iconWidth = 32;
		iconInfo.iconHeight = 32;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 4;
		iconInfo.iconBitDepth = 4;
	} else if(icns_types_equal(iconType,ICNS_16x16_4BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 16;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 4;
		iconInfo.iconBitDepth = 4;
	} else if(icns_types_equal(iconType,ICNS_16x12_4BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 12;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 4;
		iconInfo.iconBitDepth = 4;
	}
	
	// 1 bit image types - same as mask typess
	else if(icns_types_equal(iconType,ICNS_48x48_1BIT_DATA)) {
		iconInfo.iconWidth = 48;
		iconInfo.iconHeight = 48;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 1;
		iconInfo.iconBitDepth = 1;
	} else if(icns_types_equal(iconType,ICNS_32x32_1BIT_DATA)) {
		iconInfo.iconWidth = 32;
		iconInfo.iconHeight = 32;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 1;
		iconInfo.iconBitDepth = 1;
	} else if(icns_types_equal(iconType,ICNS_16x16_1BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 16;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 1;
		iconInfo.iconBitDepth = 1;
	} else if(icns_types_equal(iconType,ICNS_16x12_1BIT_DATA)) {
		iconInfo.iconWidth = 16;
		iconInfo.iconHeight = 12;
		iconInfo.iconChannels = 1;
		iconInfo.iconPixelDepth = 1;
		iconInfo.iconBitDepth = 1;
	}
	else
	{
		icns_print_err("icns_get_image_info_for_type: Unable to parse icon type '%c%c%c%c'\n",iconType.c[0],iconType.c[1],iconType.c[2],iconType.c[3]);
		iconInfo.iconType = ICNS_NULL_TYPE;
	}
	
	iconInfo.iconRawDataSize = iconInfo.iconHeight * iconInfo.iconWidth * iconInfo.iconBitDepth / ICNS_BYTE_BITS;
	
	#ifdef ICNS_DEBUG
	printf("  type is: '%c%c%c%c'\n",iconInfo.iconType.c[0],iconInfo.iconType.c[1],iconInfo.iconType.c[2],iconInfo.iconType.c[3]);
	printf("  width is: %d\n",iconInfo.iconWidth);
	printf("  height is: %d\n",iconInfo.iconHeight);
	printf("  channels are: %d\n",iconInfo.iconChannels);
	printf("  pixel depth is: %d\n",iconInfo.iconPixelDepth);
	printf("  bit depth is: %d\n",iconInfo.iconBitDepth);
	printf("  data size is: %d\n",(int)iconInfo.iconRawDataSize);
	#endif
	
	return iconInfo;
}

icns_bool_t icns_types_equal(icns_type_t typeA,icns_type_t typeB)
{
	if(memcmp(&typeA, &typeB, sizeof(icns_type_t)) == 0)
		return 1;
	else
		return 0;
}

// This is is largely for conveniance and readability
icns_bool_t icns_types_not_equal(icns_type_t typeA,icns_type_t typeB)
{
	if(memcmp(&typeA, &typeB, sizeof(icns_type_t)) != 0)
		return 1;
	else
		return 0;
}

void icns_set_print_errors(icns_bool_t shouldPrint)
{
	#ifdef ICNS_DEBUG
		if(shouldPrint == 0) {
			icns_print_err("Debugging enabled - error message status cannot be disabled!\n");
		}
	#else
		gShouldPrintErrors = shouldPrint;
	#endif
}

void icns_print_err(const char *template, ...)
{
	va_list ap;
	
	#ifdef ICNS_DEBUG
	printf ( "libicns: ");
	va_start (ap, template);
	vprintf (template, ap);
	va_end (ap);
	#else
	if(gShouldPrintErrors)
	{
		fprintf (stderr, "libicns: ");
		va_start (ap, template);
		vfprintf (stderr, template, ap);
		va_end (ap);
	}
	#endif
}

