/*
File:       icns.cpp
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
#include "colormaps.h"

icns_type_t icns_get_mask_type_for_icon_type(icns_type_t icnsType)
{
	switch(icnsType)
	{
		// Mask is already in ARGB 32-Bit icons
		case ICNS_512x512_32BIT_ARGB_DATA:
		case ICNS_256x256_32BIT_ARGB_DATA:
			return ICNS_INVALID_MASK;
		// 8-Bit masks for 32-Bit icons
		case ICNS_128X128_32BIT_DATA:
			return ICNS_128X128_8BIT_MASK;
		case ICNS_48x48_32BIT_DATA:
			return ICNS_48x48_8BIT_MASK;
		case ICNS_32x32_32BIT_DATA:
			return ICNS_32x32_8BIT_MASK;
		case ICNS_16x16_32BIT_DATA:
			return ICNS_16x16_8BIT_MASK;
		// 1-Bit masks for 1,4,8-Bit icons
		case ICNS_48x48_8BIT_DATA:
		case ICNS_48x48_4BIT_DATA:
		case ICNS_48x48_1BIT_DATA:
			return ICNS_48x48_1BIT_MASK;
		case ICNS_32x32_8BIT_DATA:
		case ICNS_32x32_4BIT_DATA:
		case ICNS_32x32_1BIT_DATA:
			return ICNS_32x32_1BIT_MASK;
		case ICNS_16x16_8BIT_DATA:
		case ICNS_16x16_4BIT_DATA:
		case ICNS_16x16_1BIT_DATA:
			return ICNS_16x16_1BIT_MASK;
		case ICNS_16x12_8BIT_DATA:
		case ICNS_16x12_4BIT_DATA:
		case ICNS_16x12_1BIT_DATA:
			return ICNS_16x12_1BIT_MASK;
		default:
			break;
	}
	return ICNS_INVALID_MASK;
}


int icns_get_image32_from_element(icns_element_t *iconElement,icns_image_t *imageOut)
{
	int	error = 0;
	icns_type_t	icnsType = 0x00000000;
	
	if(iconElement == NULL)
	{
		fprintf(stderr,"libicns: icns_get_image32_from_element: Icon element is NULL!\n");
		return -1;
	}
	
	if(imageOut == NULL)
	{
		fprintf(stderr,"libicns: icns_get_image32_from_element: Icon image structure is NULL!\n");
		return -1;
	}
	
	error = icns_get_image_from_element(iconElement,imageOut);
	
	if(error)
		return -1;
	
	icnsType = EndianSwapBtoN(iconElement->elementType,sizeof(icns_type_t));
	
	if((imageOut->pixel_depth * imageOut->imageChannels) < 32)
	{
		unsigned char	*oldData = NULL;
		unsigned char	*newData = NULL;
		unsigned int	oldBitDepth = 0;
		unsigned int	oldDataSize = 0;
		unsigned long	newBlockSize = 0;
		unsigned long	newDataSize = 0;
		unsigned long	pixelCount = 0;
		unsigned long	pixelID = 0;
		unsigned long	dataCount = 0;
		unsigned char	dataValue = 0;
		unsigned char	colorIndex = 0;
		icns_colormap_rgb_t	colorRGB;
		
		oldBitDepth = (imageOut->pixel_depth * imageOut->imageChannels);
		oldDataSize = imageOut->imageDataSize;
		
		pixelCount = imageOut->imageWidth * imageOut->imageHeight;
		
		newBlockSize = imageOut->imageWidth * 32;
		newDataSize = newBlockSize * imageOut->imageHeight;
		
		oldData = imageOut->imageData;
		newData = (unsigned char *)malloc(newDataSize);
		
		if(newData == NULL)
		{
			fprintf(stderr,"libicns: icns_get_image32_from_element: Unable to allocate memory block of size: %d!\n",(int)newDataSize);
			return -1;
		}
		
		dataCount = 0;
		
		switch(icnsType)
		{
			// 8-Bit Icon Mask Data Types
			case ICNS_128X128_8BIT_MASK:
			case ICNS_48x48_8BIT_MASK:
			case ICNS_32x32_8BIT_MASK:
			case ICNS_16x16_8BIT_MASK:
				if(oldBitDepth != 8)
				{
					fprintf(stderr,"libicns: icns_get_image32_from_element: Bit depth type mismatch!\n");
					free(newData);
					return -1;
				}
				for(pixelID = 0; pixelID < pixelCount; pixelID++)
				{
					colorIndex = oldData[dataCount++];
					newData[pixelID * 4 + 0] = colorIndex;
					newData[pixelID * 4 + 1] = 0x00;
					newData[pixelID * 4 + 2] = 0x00;
					newData[pixelID * 4 + 3] = 0x00;
				}
				break;
			// 8-Bit Icon Image Data Types
			case ICNS_48x48_8BIT_DATA:
			case ICNS_32x32_8BIT_DATA:
			case ICNS_16x16_8BIT_DATA:
			case ICNS_16x12_8BIT_DATA:
				if(oldBitDepth != 8)
				{
					fprintf(stderr,"libicns: icns_get_image32_from_element: Bit depth type mismatch!\n");
					free(newData);
					return -1;
				}
				for(pixelID = 0; pixelID < pixelCount; pixelID++)
				{
					colorIndex = oldData[dataCount++];
					colorRGB = icns_colormap_8[colorIndex];
					newData[pixelID * 4 + 0] = 0xff;
					newData[pixelID * 4 + 1] = colorRGB.r;
					newData[pixelID * 4 + 2] = colorRGB.g;
					newData[pixelID * 4 + 3] = colorRGB.b;
				}
				break;
			// 4-Bit Icon Image Data Types
			case ICNS_48x48_4BIT_DATA:
			case ICNS_32x32_4BIT_DATA:
			case ICNS_16x16_4BIT_DATA:
			case ICNS_16x12_4BIT_DATA:
				if(oldBitDepth != 4)
				{
					fprintf(stderr,"libicns: icns_get_image32_from_element: Bit depth type mismatch!\n");
					free(newData);
					return -1;
				}
				for(pixelID = 0; pixelID < pixelCount; pixelID++)
				{
					if(pixelID % 2 == 0)
						dataValue = oldData[dataCount++];
					colorIndex = (dataValue & 0xF0) >> 4;
					dataValue = dataValue << 4;
					colorRGB = icns_colormap_4[colorIndex];
					newData[pixelID * 4 + 0] = 0xFF;
					newData[pixelID * 4 + 1] = colorRGB.r;
					newData[pixelID * 4 + 2] = colorRGB.g;
					newData[pixelID * 4 + 3] = colorRGB.b;
				}
				break;
			// 1-Bit Icon Image/Mask Data Types (Data is the same)
			case ICNS_48x48_1BIT_DATA:  // Also ICNS_48x48_1BIT_MASK
			case ICNS_32x32_1BIT_DATA: // Also ICNS_32x32_1BIT_MASK
			case ICNS_16x16_1BIT_DATA: // Also ICNS_16x16_1BIT_MASK
			case ICNS_16x12_1BIT_DATA:  // Also ICNS_16x12_1BIT_MASK
				if(oldBitDepth != 1)
				{
					fprintf(stderr,"libicns: icns_get_image32_from_element: Bit depth type mismatch!\n");
					free(newData);
					return -1;
				}
				for(pixelID = 0; pixelID < pixelCount; pixelID++)
				{
					if(pixelID % 8 == 0)
						dataValue = oldData[dataCount++];
					colorIndex = (dataValue & 0x80) ? 0x00 : 0xFF;
					dataValue = dataValue << 1;
					newData[pixelID * 4 + 0] = colorIndex;
					newData[pixelID * 4 + 1] = colorIndex;
					newData[pixelID * 4 + 2] = colorIndex;
					newData[pixelID * 4 + 3] = colorIndex;
				}
				break;
			default:
				break;
		}
		
		imageOut->pixel_depth = 8;
		imageOut->imageChannels = 4;
		imageOut->imageDataSize = newDataSize;
		imageOut->imageData = newData;
		
		free(oldData);
	}
	return 0;
}


//***************************** icns_get_image_from_element **************************//
// Actual conversion of the icon data into uncompressed raw pixels

int icns_get_image_from_element(icns_element_t *iconElement,icns_image_t *imageOut)
{
	int		error = 0;
	unsigned long	dataCount = 0;	
	icns_type_t	icnsType = 0x00000000;
	unsigned long	rawDataSize = 0;
	unsigned char	*rawDataPtr = NULL;
	unsigned int	iconBitDepth = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	iconDataRowSize = 0;
	
	int		packBytes = 1;
	char		*swapPtr = NULL;
	int		*byte4Ptr = NULL;
			
	if(iconElement == NULL)
	{
		fprintf(stderr,"libicns: icns_get_image_from_element: Icon element is NULL!\n");
		return -1;
	}
	
	if(imageOut == NULL)
	{
		fprintf(stderr,"libicns: icns_get_image_from_element: Icon image structure is NULL!\n");
		return -1;
	}
	
	icnsType = EndianSwapBtoN(iconElement->elementType,sizeof(icns_type_t));
	rawDataSize = EndianSwapBtoN(iconElement->elementSize,sizeof(icns_size_t));
	rawDataSize = rawDataSize - sizeof(icns_type_t) - sizeof(icns_size_t);
	rawDataPtr = (unsigned char*)&(iconElement->elementData[0]);
	
	#if ICNS_DEBUG
	printf("Icon element type is: 0x%8X\n",(unsigned int)icnsType);
	printf("Icon element size is: %d\n",(int)rawDataSize);	
	#endif
	
	// We use the jp2 processor for these two
	if((icnsType == ICNS_512x512_32BIT_ARGB_DATA) || (icnsType == ICNS_256x256_32BIT_ARGB_DATA))
	{
		#ifdef ICNS_OPENJPEG
		
		opj_image_t* image = NULL;
		
		image = jp2dec((unsigned char *)rawDataPtr, (int)rawDataSize);
		if(!image)
			return -1;
		
		error = icns_opj_to_image(image,imageOut);
		
		opj_image_destroy(image);
		
		return error;
		
		#else
		
		fprintf(stderr,"libicns: icns_get_image_from_element: libicns requires openjpeg for this data type!\n");
		return -1;
		
		#endif
	}
	
	error = icns_init_image_for_type(icnsType,imageOut);
	
	if(error)
	{
		fprintf(stderr,"libicns: icns_get_image_from_element: Error allocating new icns image!\n");
		return -1;
	}
	
	iconBitDepth = imageOut->pixel_depth * imageOut->imageChannels;
	iconDataSize = imageOut->imageDataSize;
	iconDataRowSize = imageOut->imageWidth * iconBitDepth / icns_byte_bits;
	
	switch(iconBitDepth)
	{
	case 32:
		if(rawDataSize < imageOut->imageDataSize)
		{
			icns_decode_rle24_data(rawDataSize,(icns_sint32_t*)rawDataPtr,iconDataSize,(icns_sint32_t*)(imageOut->imageData));
		}
		else
		{
			for(dataCount = 0; dataCount < imageOut->imageHeight; dataCount++)
				memcpy(&(((char*)(imageOut->imageData))[dataCount*iconDataRowSize]),&(((char*)(rawDataPtr))[dataCount*iconDataRowSize]),iconDataRowSize);
		}

		packBytes = iconBitDepth / icns_byte_bits;
		swapPtr = (char*)imageOut->imageData;
		byte4Ptr = NULL;
		
		for(dataCount = 0; dataCount < iconDataSize; dataCount+=packBytes)
		{
			byte4Ptr = (int *)(swapPtr + dataCount);
			*( byte4Ptr ) = EndianSwapBtoL32( *( byte4Ptr ) );
		}
		break;
	case 8:
	case 4:
	case 1:
		for(dataCount = 0; dataCount < imageOut->imageHeight; dataCount++)
			memcpy(&(((char*)(imageOut->imageData))[dataCount*iconDataRowSize]),&(((char*)(rawDataPtr))[dataCount*iconDataRowSize]),iconDataRowSize);
		break;
	default:
		fprintf(stderr,"libicns: icns_get_image_from_element: Unknown bit depth!\n");
		return -1;
		break;
	}
	
	return error;
}


//***************************** icns_init_image_for_type **************************//
// Initialize a new image structure for holding the data
// using the information for the specified type

int icns_init_image_for_type(icns_type_t icnsType,icns_image_t *imageOut)
{
	unsigned int	iconWidth = 0;
	unsigned int	iconHeight = 0;
	unsigned int	iconChannels = 0;
	unsigned int	iconBitDepth = 0;
	
	if(imageOut == NULL)
	{
		fprintf(stderr,"libicns: icns_init_image_for_type: Icon image structure is NULL!\n");
		return -1;
	}
	
	switch(icnsType)
	{
		// 32-Bit Icon Image Data Types
		case ICNS_128X128_32BIT_DATA:
			iconWidth = 128;
			iconHeight = 128;
			iconChannels = 4;
			iconBitDepth = 32;
			break;
		case ICNS_48x48_32BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 4;
			iconBitDepth = 32;
			break;
		case ICNS_32x32_32BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 4;
			iconBitDepth = 32;
			break;
		case ICNS_16x16_32BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 4;
			iconBitDepth = 32;
			break;
			
		// 8-Bit Icon Mask Data Types
		case ICNS_128X128_8BIT_MASK:
			iconWidth = 128;
			iconHeight = 128;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_48x48_8BIT_MASK:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_32x32_8BIT_MASK:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_16x16_8BIT_MASK:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
			
		// 8-Bit Icon Image Data Types
		case ICNS_48x48_8BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_32x32_8BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_16x16_8BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconBitDepth = 8;
			break;
		case ICNS_16x12_8BIT_DATA:
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconBitDepth = 8;
			break;

		// 4-Bit Icon Image Data Types
		case ICNS_48x48_4BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconBitDepth = 4;
			break;
		case ICNS_32x32_4BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconBitDepth = 4;
			break;
		case ICNS_16x16_4BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconBitDepth = 4;
			break;
		case ICNS_16x12_4BIT_DATA:
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconBitDepth = 4;
			break;

		// 1-Bit Icon Image/Mask Data Types (Data is the same)
		case ICNS_48x48_1BIT_DATA:  // Also ICNS_48x48_1BIT_MASK
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconBitDepth = 1;
			break;
		case ICNS_32x32_1BIT_DATA: // Also ICNS_32x32_1BIT_MASK
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconBitDepth = 1;
			break;
		case ICNS_16x16_1BIT_DATA: // Also ICNS_16x16_1BIT_MASK
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconBitDepth = 1;
			break;
		case ICNS_16x12_1BIT_DATA: // Also ICNS_16x12_1BIT_MASK
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconBitDepth = 1;
			break;
			
		default:
			fprintf(stderr,"libicns: icns_init_image_for_type: Unable to parse icon type 0x%8X\n",icnsType);
			return -1;
			break;
	}
	
	return icns_init_image(iconWidth,iconHeight,iconChannels,(iconBitDepth / iconChannels),imageOut);

}

//***************************** icns_init_image **************************//
// Initialize a new image structure for holding the data

int icns_init_image(unsigned int iconWidth,unsigned int iconHeight,unsigned int iconChannels,unsigned int iconPixelDepth,icns_image_t *imageOut)
{
	unsigned int	iconBitDepth = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	iconDataRowSize = 0;

	iconBitDepth = iconPixelDepth * iconChannels;
	iconDataRowSize = iconWidth * iconBitDepth / icns_byte_bits;
	iconDataSize = iconHeight * iconDataRowSize;
	
	#ifdef ICNS_DEBUG
	printf("Initializing new image...\n");
	printf("Icon image width is: %d\n",iconWidth);
	printf("Icon image height is: %d\n",iconHeight);
	printf("Icon image channels are: %d\n",iconChannels);
	printf("Icon image bit depth is: %d\n",iconBitDepth);
	printf("Icon image data size is: %d\n",(int)iconDataSize);
	#endif
	
	imageOut->imageWidth = iconWidth;
	imageOut->imageHeight = iconHeight;
	imageOut->imageChannels = iconChannels;
	imageOut->pixel_depth = (iconBitDepth / iconChannels);
	imageOut->imageDataSize = iconDataSize;
	imageOut->imageData = (unsigned char *)malloc(iconDataSize);
	if(!imageOut->imageData)
	{
		fprintf(stderr,"libicns: icns_init_image: Unable to allocate memory block of size: %d ($s:%m)!\n",(int)iconDataSize);
		return -1;
	}
	memset(imageOut->imageData,0,iconDataSize);
	
	return 0;
}

//***************************** icns_free_image **************************//
// Deallocate data in an image structure

int icns_free_image(icns_image_t *imageIn)
{
	imageIn->imageWidth = 0;
	imageIn->imageHeight = 0;
	imageIn->imageChannels = 0;
	imageIn->pixel_depth = 0;
	imageIn->imageDataSize = 0;
	
	if(imageIn->imageData != NULL)
	{
		free(imageIn->imageData);
		imageIn->imageData = NULL;
	}
	
	return 0;
}

