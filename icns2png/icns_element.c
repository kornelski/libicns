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
#include "icns_internals.h"


//***************************** icns_get_element_from_family **************************//
// Parses requested data from an icon family - puts it into a icon element

int icns_get_element_from_family(icns_family_t *iconFamily,icns_type_t iconType,icns_element_t **iconElementOut)
{
	int		error = ICNS_STATUS_OK;
	int		foundData = 0;
	icns_type_t	iconFamilyType = ICNS_NULL_TYPE;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	elementType = ICNS_NULL_TYPE;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(iconFamily == NULL)
	{
		icns_print_err("icns_get_element_from_family: icns family is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(iconElementOut == NULL)
	{
		icns_print_err("icns_get_element_in_family: icns element out is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	else
	{
		*iconElementOut = NULL;
	}
	
	if(icns_types_not_equal(iconFamily->resourceType,ICNS_FAMILY_TYPE))
	{
		icns_print_err("icns_get_element_from_family: Invalid icns family!\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType),sizeof( icns_type_t));
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize),sizeof( icns_size_t));
	
	#ifdef ICNS_DEBUG
	printf("Looking for icon element of type: '%c%c%c%c'\n",iconType.c[0],iconType.c[1],iconType.c[2],iconType.c[3]);
	printf("  icon family type check: '%c%c%c%c'\n",iconFamilyType.c[0],iconFamilyType.c[1],iconFamilyType.c[2],iconFamilyType.c[3]);
	printf("  icon family size check: %d\n",iconFamilySize);
	#endif
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		
		if( iconFamilySize < (dataOffset+sizeof(icns_type_t)+sizeof(icns_size_t)) )
		{
			icns_print_err("icns_get_element_from_family: Corrupted icns family!\n");
			return ICNS_STATUS_INVALID_DATA;		
		}
		
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType),sizeof( icns_type_t));
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize),sizeof( icns_size_t));
		
		#ifdef ICNS_DEBUG
		printf("element data...\n");
		printf("  type: '%c%c%c%c'%s\n",elementType.c[0],elementType.c[1],elementType.c[2],elementType.c[3],(icns_types_equal(elementType,iconType) ? " - match!" : " "));
		printf("  size: %d\n",(int)elementSize);
		#endif
		
		if( (elementSize < 8) || ((dataOffset+elementSize) > iconFamilySize) )
		{
			icns_print_err("icns_get_element_from_family: Invalid element size! (%d)\n",elementSize);
			return ICNS_STATUS_INVALID_DATA;
		}
		
		if (icns_types_equal(elementType,iconType))
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(foundData)
	{
		*iconElementOut = malloc(elementSize);
		if(*iconElementOut == NULL)
		{
			icns_print_err("icns_get_element_from_family: Unable to allocate memory block of size: %d!\n",elementSize);
			return ICNS_STATUS_NO_MEMORY;
		}
		memcpy( *iconElementOut, iconElement, elementSize);
	}
	else
	{
		icns_print_err("icns_get_element_from_family: Unable to find requested icon data!\n");
		error = ICNS_STATUS_DATA_NOT_FOUND;
	}
	
	return error;
}

//***************************** icns_set_element_in_family **************************//
// Adds/updates the icns element of it's type in the icon family

int icns_set_element_in_family(icns_family_t **iconFamilyRef,icns_element_t *newIconElement)
{
	int		error = ICNS_STATUS_OK;
	int		foundData = 0;
	int		copiedData = 0;
	icns_family_t	*iconFamily = NULL;
	icns_type_t	iconFamilyType = ICNS_NULL_TYPE;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	newElementType = ICNS_NULL_TYPE;
	icns_size_t	newElementSize = 0;
	icns_type_t	elementType = ICNS_NULL_TYPE;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	icns_size_t	newIconFamilySize = 0;
	icns_family_t	*newIconFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	if(iconFamilyRef == NULL)
	{
		icns_print_err("icns_set_element_in_family: icns family reference is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	iconFamily = *iconFamilyRef;
	
	if(iconFamily == NULL)
	{
		icns_print_err("icns_set_element_in_family: icns family is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(icns_types_not_equal(iconFamily->resourceType,ICNS_FAMILY_TYPE))
	{
		icns_print_err("icns_set_element_in_family: Invalid icns family!\n");
		error = ICNS_STATUS_INVALID_DATA;
	}
	
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType),sizeof( icns_type_t));
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize),sizeof( icns_size_t));
	
	if(newIconElement == NULL)
	{
		icns_print_err("icns_set_element_in_family: icns element is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	// Retrieve first, then swap. May help with problems on some arch	
	ICNS_READ_UNALIGNED(newElementType, &(newIconElement->elementType),sizeof( icns_type_t));
	ICNS_READ_UNALIGNED(newElementSize, &(newIconElement->elementSize),sizeof( icns_size_t));
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType),sizeof( icns_type_t));
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize),sizeof( icns_size_t));
		
		if(icns_types_equal(elementType,newElementType))
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
		icns_print_err("icns_set_element_in_family: Unable to allocate memory block of size: %d!\n",newIconFamilySize);
		return ICNS_STATUS_NO_MEMORY;
	}
	
	newIconFamily->resourceType = ICNS_FAMILY_TYPE;
	newIconFamily->resourceSize = newIconFamilySize;
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	copiedData = 0;
	
	while ( dataOffset < iconFamilySize )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType),sizeof( icns_type_t));
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize),sizeof( icns_size_t));
		
		if(icns_types_not_equal(elementType,newElementType))
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

//***************************** icns_add_element_in_family **************************//
// Adds/updates the icns element of it's type in the icon family
// A convenience alias to icns_set_element_in_family

int icns_add_element_in_family(icns_family_t **iconFamilyRef,icns_element_t *newIconElement)
{
	if(iconFamilyRef == NULL)
	{
		icns_print_err("icns_add_element_in_family: icns family reference is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(newIconElement == NULL)
	{
		icns_print_err("icns_add_element_in_family: icon element is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	return icns_set_element_in_family(iconFamilyRef,newIconElement);
}

//***************************** icns_remove_element_in_family **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

int icns_remove_element_in_family(icns_family_t **iconFamilyRef,icns_type_t iconElementType)
{
	int		error = ICNS_STATUS_OK;
	int		foundData = 0;
	icns_family_t	*iconFamily = NULL;
	icns_type_t	iconFamilyType = ICNS_NULL_TYPE;
	icns_size_t	iconFamilySize = 0;
	icns_element_t	*iconElement = NULL;
	icns_type_t	elementType = ICNS_NULL_TYPE;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(iconFamilyRef == NULL)
	{
		icns_print_err("icns_remove_element_in_family: icon family reference is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	iconFamily = *iconFamilyRef;
	
	if(iconFamily == NULL)
	{
		icns_print_err("icns_remove_element_in_family: icon family is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(icns_types_not_equal(iconFamily->resourceType,ICNS_FAMILY_TYPE))
	{
		icns_print_err("icns_remove_element_in_family: Invalid icon family!\n");
		error = ICNS_STATUS_INVALID_DATA;
	}
	
	ICNS_READ_UNALIGNED(iconFamilyType, &(iconFamily->resourceType),sizeof( icns_type_t));
	ICNS_READ_UNALIGNED(iconFamilySize, &(iconFamily->resourceSize),sizeof( icns_size_t));
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < iconFamilySize) )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType),sizeof( icns_type_t));
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize),sizeof( icns_size_t));
		
		if (icns_types_equal(elementType,iconElementType))
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(!foundData)
	{
		icns_print_err("icns_remove_element_in_family: Unable to find requested icon data for removal!\n");
		return ICNS_STATUS_DATA_NOT_FOUND;
	}
	
	icns_size_t	newIconFamilySize = 0;
	icns_family_t	*newIconFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	newIconFamilySize = iconFamilySize - elementSize;
	newIconFamily = malloc(newIconFamilySize);
	
	if(newIconFamily == NULL)
	{
		icns_print_err("icns_remove_element_in_family: Unable to allocate memory block of size: %d!\n",newIconFamilySize);
		return ICNS_STATUS_NO_MEMORY;
	}
	
	newIconFamily->resourceType = ICNS_FAMILY_TYPE;
	newIconFamily->resourceSize = newIconFamilySize;
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( dataOffset < iconFamilySize )
	{
		iconElement = ((icns_element_t*)(((char*)iconFamily)+dataOffset));
		ICNS_READ_UNALIGNED(elementType, &(iconElement->elementType),sizeof( icns_type_t));
		ICNS_READ_UNALIGNED(elementSize, &(iconElement->elementSize),sizeof( icns_size_t));
		
		if(icns_types_not_equal(elementType,iconElementType))
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



//***************************** icns_new_element_from_image **************************//
// Creates a new icon element from an image
int icns_new_element_from_icon_image(icns_image_t *imageIn,icns_type_t iconType,icns_element_t **iconElementOut)
{
	return icns_new_element_from_image(imageIn,iconType,0,iconElementOut);
}

//***************************** icns_new_element_from_mask **************************//
// Creates a new icon element from an image
int icns_new_element_from_mask_image(icns_image_t *imageIn,icns_type_t iconType,icns_element_t **iconElementOut)
{
	return icns_new_element_from_image(imageIn,iconType,1,iconElementOut);
}

//***************************** icns_new_element_from_image_data **************************//
// Creates a new icon element from an image
int icns_new_element_from_image(icns_image_t *imageIn,icns_type_t iconType,icns_bool_t isMask,icns_element_t **iconElementOut)
{
	int		        error = ICNS_STATUS_OK;
	icns_icon_image_info_t  iconInfo;
	
	if(imageIn == NULL)
	{
		icns_print_err("icns_new_element_from_image: Icon image is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(iconElementOut == NULL)
	{
		icns_print_err("icns_new_element_from_image: Icon element reference is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	else
	{
		*iconElementOut = NULL;
	}
	
	if(icns_types_equal(iconType,ICNS_NULL_DATA)) {
		icns_print_err("icns_new_element_from_image: Invalid icon type!\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	// Determine what the height and width ought to be, to check the incoming image
	
	iconInfo = icns_get_image_info_for_type(iconType);
	
	// Check the image width, height, and pixel depth
	
	if(imageIn->imageWidth != iconInfo.iconWidth)
	{
		icns_print_err("icns_new_element_from_image: Invalid input image width: %d\n",imageIn->imageWidth);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	if(imageIn->imageWidth != iconInfo.iconWidth)
	{
		icns_print_err("icns_new_element_from_image: Invalid input image width: %d\n",imageIn->imageWidth);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	if(imageIn->imageHeight != iconInfo.iconHeight)
	{
		icns_print_err("icns_new_element_from_image: Invalid input image height: %d\n",imageIn->imageHeight);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	if(imageIn->imagePixelDepth != (iconInfo.iconBitDepth/iconInfo.iconChannels))
	{
		icns_print_err("icns_new_element_from_image: libicns does't support bit depth conversion yet.\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	// Check the image data size and data pointer
	
	if(imageIn->imageDataSize == 0)
	{
		icns_print_err("icns_new_element_from_image: Invalid input image data size: %d\n",(int)imageIn->imageDataSize);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	if(imageIn->imageData == NULL)
	{
		icns_print_err("icns_new_element_from_image: Invalid input image data\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	// Finally, done with all the preliminary checks
	icns_byte_t	*imageDataPtr = NULL;
	unsigned long	imageDataSize = 0;
	
	// For use to easily track deallocation if we use rle24
	icns_byte_t	*rle24DataPtr = NULL;
	
	if( (icns_types_equal(iconType,ICNS_256x256_32BIT_ARGB_DATA)) || (icns_types_equal(iconType,ICNS_512x512_32BIT_ARGB_DATA)) )
	{
		// Future openjpeg routines to go here
		icns_print_err("icns_new_element_from_image: libicns does't support importing giant icons yet.\n");
		return ICNS_STATUS_UNSUPPORTED;
	}
	else
	{
		if(icns_types_equal(iconType,ICNS_128X128_32BIT_DATA))
		{
			// Note: icns_encode_rle24_data allocates memory that must be freed later
			if((error = icns_encode_rle24_data(imageIn->imageDataSize,(icns_sint32_t*)imageIn->imageData,&imageDataSize,(icns_sint32_t**)&imageDataPtr)))
			{
				if(imageDataPtr != NULL)
				{
					free(imageDataPtr);
					imageDataPtr = NULL;
					icns_print_err("icns_new_element_from_image: Error rle encoding image data.\n");
					return ICNS_STATUS_INVALID_DATA;
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
		icns_print_err("icns_new_element_from_image: Can't work with size 0 image data\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	if(imageDataPtr == NULL)
	{
		icns_print_err("icns_new_element_from_image: Can't work with NULL image data\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	icns_element_t	*newElement = NULL;
	icns_size_t	newElementSize = 0;
	icns_type_t	newElementType = ICNS_NULL_DATA;
	icns_size_t	newElementOffset = 0;
	
	// Set up and create the new element
	newElementOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	newElementSize = sizeof(icns_type_t) + sizeof(icns_size_t) + imageDataSize;
	newElementType = iconType;
	newElement = (icns_element_t *)malloc(newElementSize);
	if(newElement == NULL)
	{
		icns_print_err("icns_new_element_from_image: Unable to allocate memory block of size: %d!\n",(int)newElementSize);
		// We might have allocated new memory earlier...
		if(rle24DataPtr != NULL)
		{
			free(rle24DataPtr);
			rle24DataPtr = NULL;
		}
		return ICNS_STATUS_NO_MEMORY;
	}
	
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

