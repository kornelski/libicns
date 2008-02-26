/*
File:       icns_element.c
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
              2007 Thomas Lübking <thomas.luebking@web.de>
              2002 Chenxiao Zhao <chenxiao.zhao@gmail.com>

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

#include "icns.h"

#include "endianswap.h"


//***************************** icns_new_element_from_image **************************//
// Creates a new icon element from an image
int icns_new_element_from_image(icns_image_t *imageIn,icns_type_t iconType,icns_element_t **iconElementOut)
{
	int		error = 0;
	unsigned int	iconTypeWidth = 0;
	unsigned int	iconTypeHeight = 0;
	unsigned int	iconTypeChannels = 0;
	unsigned int	iconTypeBitDepth = 0;
	
	if(imageIn == NULL)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: image in is NULL!\n");
		return -1;
	}
	
	if(iconElementOut == NULL)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: element out is NULL!\n");
		return -1;
	}
	else
	{
		*iconElementOut = NULL;
	}
	
	switch(iconType)
	{
		// 32-Bit ARGB Image Data Types
		case ICNS_512x512_32BIT_ARGB_DATA:
			iconTypeWidth = 512;
			iconTypeHeight = 512;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;
		case ICNS_256x256_32BIT_ARGB_DATA:
			iconTypeWidth = 512;
			iconTypeHeight = 512;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;

		// 32-Bit Icon Image Data Types
		case ICNS_128X128_32BIT_DATA:
			iconTypeWidth = 128;
			iconTypeHeight = 128;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;
		case ICNS_48x48_32BIT_DATA:
			iconTypeWidth = 48;
			iconTypeHeight = 48;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;
		case ICNS_32x32_32BIT_DATA:
			iconTypeWidth = 32;
			iconTypeHeight = 32;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;
		case ICNS_16x16_32BIT_DATA:
			iconTypeWidth = 16;
			iconTypeHeight = 16;
			iconTypeChannels = 4;
			iconTypeBitDepth = 32;
			break;
			
		// 8-Bit Icon Mask Data Types
		case ICNS_128X128_8BIT_MASK:
			iconTypeWidth = 128;
			iconTypeHeight = 128;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_48x48_8BIT_MASK:
			iconTypeWidth = 48;
			iconTypeHeight = 48;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_32x32_8BIT_MASK:
			iconTypeWidth = 32;
			iconTypeHeight = 32;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_16x16_8BIT_MASK:
			iconTypeWidth = 16;
			iconTypeHeight = 16;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
			
		// 8-Bit Icon Image Data Types
		case ICNS_48x48_8BIT_DATA:
			iconTypeWidth = 48;
			iconTypeHeight = 48;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_32x32_8BIT_DATA:
			iconTypeWidth = 32;
			iconTypeHeight = 32;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_16x16_8BIT_DATA:
			iconTypeWidth = 16;
			iconTypeHeight = 16;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;
		case ICNS_16x12_8BIT_DATA:
			iconTypeWidth = 16;
			iconTypeHeight = 12;
			iconTypeChannels = 1;
			iconTypeBitDepth = 8;
			break;

		// 4-Bit Icon Image Data Types
		case ICNS_48x48_4BIT_DATA:
			iconTypeWidth = 48;
			iconTypeHeight = 48;
			iconTypeChannels = 1;
			iconTypeBitDepth = 4;
			break;
		case ICNS_32x32_4BIT_DATA:
			iconTypeWidth = 32;
			iconTypeHeight = 32;
			iconTypeChannels = 1;
			iconTypeBitDepth = 4;
			break;
		case ICNS_16x16_4BIT_DATA:
			iconTypeWidth = 16;
			iconTypeHeight = 16;
			iconTypeChannels = 1;
			iconTypeBitDepth = 4;
			break;
		case ICNS_16x12_4BIT_DATA:
			iconTypeWidth = 16;
			iconTypeHeight = 12;
			iconTypeChannels = 1;
			iconTypeBitDepth = 4;
			break;

		// 1-Bit Icon Image/Mask Data Types (Data is the same)
		case ICNS_48x48_1BIT_DATA:  // Also ICNS_48x48_1BIT_MASK
			iconTypeWidth = 48;
			iconTypeHeight = 48;
			iconTypeChannels = 1;
			iconTypeBitDepth = 1;
			break;
		case ICNS_32x32_1BIT_DATA: // Also ICNS_32x32_1BIT_MASK
			iconTypeWidth = 32;
			iconTypeHeight = 32;
			iconTypeChannels = 1;
			iconTypeBitDepth = 1;
			break;
		case ICNS_16x16_1BIT_DATA: // Also ICNS_16x16_1BIT_MASK
			iconTypeWidth = 16;
			iconTypeHeight = 16;
			iconTypeChannels = 1;
			iconTypeBitDepth = 1;
			break;
		case ICNS_16x12_1BIT_DATA: // Also ICNS_16x12_1BIT_MASK
			iconTypeWidth = 16;
			iconTypeHeight = 12;
			iconTypeChannels = 1;
			iconTypeBitDepth = 1;
			break;
			
		default:
			fprintf(stderr,"libicns: icns_new_element_from_image: Unable to parse icon type 0x%8X\n",iconType);
			return -1;
			break;
	}
	
	if(imageIn->imageDataSize == 0)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: invalid input image data size: %d\n",(int)imageIn->imageDataSize);
		return -1;
	}
	
	if(imageIn->imageData == NULL)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: invalid input image data\n");
		return -1;
	}
	
	if(imageIn->imageWidth != iconTypeWidth)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: invalid input image width: %d\n",imageIn->imageWidth);
		return -1;
	}
	
	if(imageIn->imageWidth != iconTypeWidth)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: invalid input image width: %d\n",imageIn->imageWidth);
		return -1;
	}
	
	if(imageIn->imageHeight != iconTypeHeight)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: invalid input image height: %d\n",imageIn->imageHeight);
		return -1;
	}
	
	if(imageIn->pixel_depth != (iconTypeBitDepth/iconTypeChannels))
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: libicns does't support bit depth conversion yet.\n");
		return -1;
	}
	
	// Finally, done with all the preliminary checks
	unsigned char	*imageDataPtr = NULL;
	unsigned long	imageDataSize = 0;
	
	// For use to easily track deallocation if we use rle24
	unsigned char	*rle24DataPtr = NULL;
	
	if( (iconType == ICNS_256x256_32BIT_ARGB_DATA) || (iconType == ICNS_512x512_32BIT_ARGB_DATA) )
	{
		// Future openjpeg routines to go here
		fprintf(stderr,"libicns: icns_new_element_from_image: libicns does't support importing large icons yet.\n");
		return -1;
	}
	else
	{
		if(iconType == ICNS_128X128_32BIT_DATA)
		{
			// Note: icns_encode_rle24_data allocates memory that must be freed later
			if((error = icns_encode_rle24_data(imageIn->imageDataSize,(icns_sint32_t*)imageIn->imageData,&imageDataSize,(icns_sint32_t**)&imageDataPtr)))
			{
				if(imageDataPtr != NULL)
				{
					free(imageDataPtr);
					imageDataPtr = NULL;
					fprintf(stderr,"libicns: icns_new_element_from_image: error rle encoding image data.\n");
					return -1;
				}
			}
			
			rle24DataPtr = imageDataPtr;
		}
		else
		{
			imageDataSize = imageIn->imageDataSize;
			imageDataPtr = imageIn->imageData;
		}
	}
	
	if(imageDataSize == 0)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: Can't work with size 0 image data\n");
		return -1;
	}
	
	if(imageDataPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: Can't work with NULL image data\n");
		return -1;
	}
	
	icns_element_t	*newElement = NULL;
	icns_size_t	newElementSize = 0;
	icns_type_t	newElementType = 0x00000000;
	icns_size_t	newElementOffset = 0;
	
	// Set up and create the new element
	newElementOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	newElementSize = sizeof(icns_type_t) + sizeof(icns_size_t) + imageDataSize;
	newElementType = iconType;
	newElement = (icns_element_t *)malloc(newElementSize);
	if(newElement == NULL)
	{
		fprintf(stderr,"libicns: icns_new_element_from_image: Unable to allocate memory block of size: %d!\n",(int)newElementSize);
		// We might have allocated new memory earlier...
		if(rle24DataPtr != NULL)
		{
			free(rle24DataPtr);
			rle24DataPtr = NULL;
		}
		return -1;
	}
	
	newElement->elementSize = EndianSwapNtoB(newElementSize,sizeof(icns_size_t));
	newElement->elementType = EndianSwapNtoB(newElementType,sizeof(icns_type_t));
	
	// Copy in the image data
	memcpy(newElement+newElementOffset,imageDataPtr,imageDataSize);
	
	// We might have allocated new memory earlier...
	if(rle24DataPtr != NULL)
	{
		free(rle24DataPtr);
		rle24DataPtr = NULL;
	}
	
	return error;
}

//***************************** icns_get_element_from_family **************************//
// Parses requested data from an icon family - puts it into a icon element

int icns_get_element_from_family(icns_family_t *iconFamily,icns_type_t iconType,icns_element_t **iconElementOut)
{
	int		error = 0;
	int		foundData = 0;
	icns_type_t	iconFamilyType = 0x00000000;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(iconFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_get_element_from_family: icns family is NULL!\n");
		return -1;
	}
	
	if(iconElementOut == NULL)
	{
		fprintf(stderr,"libicns: icns_get_element_in_family: icns element out is NULL!\n");
		return -1;
	}
	else
	{
		*iconElementOut = NULL;
	}
	
	if(iconFamily->resourceType != EndianSwapBtoN(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE)) ){
		fprintf(stderr,"libicns: icns_get_element_from_family: Invalid icns family!\n");
		return -1;
	}

	// Retrieve first, then swap. May help with alignement errors an some arch
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType), icns_type_t);
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize), icns_size_t);
	iconFamilyType = EndianSwapBtoN(iconFamilyType,sizeof(icns_type_t));
	iconFamilySize = EndianSwapBtoN(iconFamilySize,sizeof(icns_size_t));
	
	#ifdef ICNS_DEBUG
	printf("Icon family resource size: %d\n",iconFamilySize);
	printf("Icon family resource type: 0x%8X\n",(unsigned int)iconFamilyType);
	printf("Looking for icon element of type: 0x%8X\n",(unsigned int)iconType);
	#endif
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		
		if( iconFamilySize < (dataOffset+sizeof(icns_type_t)+sizeof(icns_size_t)) )
		{
			fprintf(stderr,"libicns: icns_get_element_from_family: Corrupted icns family!\n");
			return -1;		
		}
		
		// Retrieve first, then swap. May help with some alignment errors on some arch
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType), icns_type_t);
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize), icns_size_t);
		elementType = EndianSwapBtoN(elementType,sizeof(icns_type_t));
		elementSize = EndianSwapBtoN(elementSize,sizeof(icns_size_t));
	
		#ifdef ICNS_DEBUG
		printf("Found element data...%s\n",((elementType == iconType) ? " match!" : " "));
		printf("  type: 0x%8X\n",(unsigned int)elementType);
		printf("  size: %d\n",(unsigned int)elementSize);
		#endif
		
		if( (elementSize == 0) || ((dataOffset+elementSize) > iconFamilySize) )
		{
			fprintf(stderr,"libicns: icns_get_element_from_family: Invalid element size!\n");
			return -1;
		}
		
		if (elementType == iconType)
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(foundData)
	{
		*iconElementOut = malloc(elementSize);
		if(*iconElementOut == NULL)
		{
			fprintf(stderr,"libicns: icns_get_element_from_family: Unable to allocate memory block of size: %d!\n",elementSize);
			return -1;
		}
		memcpy( *iconElementOut, iconElement, elementSize);
	}
	else
	{
		fprintf(stderr,"libicns: icns_get_element_from_family: Unable to find requested icon data!\n");
		error = -1;
	}
	
	return error;
}

//***************************** icns_set_element_in_family **************************//
// Adds/updates the icns element of it's type in the icon family

int icns_set_element_in_family(icns_family_t **iconFamilyRef,icns_element_t *newIconElement)
{
	int		error = 0;
	int		foundData = 0;
	int		copiedData = 0;
	icns_family_t	*iconFamily = NULL;
	icns_type_t	iconFamilyType = 0x00000000;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	newElementType = 0x00000000;
	icns_size_t	newElementSize = 0;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	icns_size_t	newIconFamilySize = 0;
	icns_family_t	*newIconFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	if(iconFamilyRef == NULL)
	{
		fprintf(stderr,"libicns:icns_set_element_in_family: icns family reference is NULL!\n");
		return -1;
	}
	
	iconFamily = *iconFamilyRef;
	
	if(iconFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: icns family is NULL!\n");
		return -1;
	}
	
	if(iconFamily->resourceType != EndianSwapBtoN(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE)) ) {
		fprintf(stderr,"libicns: icns_set_element_in_family: Invalid icns family!\n");
		error = -1;
	}
	
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType), icns_type_t);
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize), icns_size_t);
	iconFamilyType = EndianSwapBtoN(iconFamilyType,sizeof(icns_type_t));
	iconFamilySize = EndianSwapBtoN(iconFamilySize,sizeof(icns_size_t));

	if(newIconElement == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: icns element is NULL!\n");
		return -1;
	}

	// Retrieve first, then swap. May help with problems on some arch	
	ICNS_READ_UNALIGNED(newElementType, &(newIconElement->elementType), icns_type_t);
	ICNS_READ_UNALIGNED(newElementSize, &(newIconElement->elementSize), icns_size_t);
	newElementType = EndianSwapNtoB(newElementType,sizeof(icns_type_t));
	newElementSize = EndianSwapNtoB(newElementSize,sizeof(icns_size_t));
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType), icns_type_t);
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize), icns_size_t);
		elementType = EndianSwapBtoN(elementType,sizeof(icns_type_t));
		elementSize = EndianSwapBtoN(elementSize,sizeof(icns_size_t));
		
		if (elementType == newElementType)
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(foundData)
		newIconFamilySize = iconFamilySize - elementSize + newElementSize;
	else
		newIconFamilySize = iconFamilySize + newElementSize;

	newIconFamily = malloc(newIconFamilySize);
	
	if(newIconFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: Unable to allocate memory block of size: %d!\n",newIconFamilySize);
		return -1;
	}

	newIconFamily->resourceType = EndianSwapNtoB(ICNS_FAMILY_TYPE,sizeof(icns_type_t));
	newIconFamily->resourceSize = EndianSwapNtoB(newIconFamilySize,sizeof(icns_size_t));
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	copiedData = 0;
	
	while ( dataOffset < iconFamilySize )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType), icns_type_t);
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize), icns_size_t);
		elementType = EndianSwapBtoN(elementType,sizeof(icns_type_t));
		elementSize = EndianSwapBtoN(elementSize,sizeof(icns_size_t));
		
		if (elementType != newElementType)
		{
			memcpy( ((char *)(newIconFamily))+newDataOffset , ((char *)(iconFamily))+dataOffset, elementSize);
			newDataOffset += elementSize;
		}
		else
		{
			memcpy( ((char *)(newIconFamily))+newDataOffset , (char *)newIconElement, newElementSize);
			newDataOffset += newElementSize;
			copiedData = 1;
		}

		dataOffset += elementSize;
	}
	
	if(!copiedData)
	{
		memcpy( ((char *)(newIconFamily))+newDataOffset , (char *)newIconElement, newElementSize);
		newDataOffset += newElementSize;
	}
	
	*iconFamilyRef = newIconFamily;
	
	free(iconFamily);
	
	return error;
}

//***************************** icns_remove_element_in_family **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

int icns_remove_element_in_family(icns_family_t **iconFamilyRef,icns_type_t iconElementType)
{
	int		error = 0;
	int		foundData = 0;
	icns_family_t	*iconFamily = NULL;
	icns_type_t	iconFamilyType = 0x00000000;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(iconFamilyRef == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: icns family reference is NULL!\n");
		return -1;
	}
	
	iconFamily = *iconFamilyRef;
	
	if(iconFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: icns family is NULL!\n");
		return -1;
	}
	
	if(iconFamily->resourceType != EndianSwapBtoN(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE)) ) {
		fprintf(stderr,"libicns: icns_remove_element_in_family: Invalid icns family!\n");
		error = -1;
	}
	
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType), icns_type_t);
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize), icns_size_t);
	iconFamilyType = EndianSwapBtoN(iconFamilyType,sizeof(int));
	iconFamilySize = EndianSwapBtoN(iconFamilySize,sizeof(int));
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType), icns_type_t);
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize), icns_size_t);
		elementType = EndianSwapBtoN(elementType,sizeof(icns_type_t));
		elementSize = EndianSwapBtoN(elementSize,sizeof(icns_size_t));
		
		if (elementType == iconElementType)
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(!foundData)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: Unable to find requested icon data for removal!\n");
		return -1;
	}
	
	icns_size_t	newIconFamilySize = 0;
	icns_family_t	*newIconFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	newIconFamilySize = iconFamilySize - elementSize;
	newIconFamily = malloc(newIconFamilySize);
	
	if(newIconFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: Unable to allocate memory block of size: %d!\n",newIconFamilySize);
		return -1;
	}
	
	newIconFamily->resourceType = EndianSwapNtoB(ICNS_FAMILY_TYPE,sizeof(icns_type_t));
	newIconFamily->resourceSize = EndianSwapNtoB(newIconFamilySize,sizeof(icns_size_t));
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( dataOffset < iconFamilySize )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType), icns_type_t);
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize), icns_size_t);
		elementType = EndianSwapBtoN(elementType,sizeof(icns_type_t));
		elementSize = EndianSwapBtoN(elementSize,sizeof(icns_size_t));
		
		if (elementType != iconElementType)
		{
			memcpy( ((char *)(newIconFamily))+newDataOffset , ((char *)(iconFamily))+dataOffset, elementSize);
			newDataOffset += elementSize;
		}
		
		dataOffset += elementSize;
	}
	
	*iconFamilyRef = newIconFamily;

	free(iconFamily);
	
	return error;
}

