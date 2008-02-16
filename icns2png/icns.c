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
#include <openjpeg.h>

#include "icns.h"
#include "jp2dec.h"
#include "endianswap.h"
#include "colormaps.h"

int DecodeRLE24Data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long dataOutSize, icns_sint32_t *dataOutPtr);

int GetIcnsImage32FromIcnsElement(icns_element_t *iconElement, icns_bool_t swapBytes,icns_image_t *imageOut)
{
	int	error = 0;
	icns_type_t	icnsType = 0x00000000;
	
	if(iconElement == NULL)
	{
		fprintf(stderr,"Icon element is NULL!\n");
		return -1;
	}
	
	if(imageOut == NULL)
	{
		fprintf(stderr,"Icon image structure is NULL!\n");
		return -1;
	}
	
	error = GetIcnsImageFromIcnsElement(iconElement,swapBytes,imageOut);
	
	if(error)
		return -1;
	
	icnsType = EndianSwap(iconElement->elementType,sizeof(icns_type_t),swapBytes);
	
	if(imageOut->imageDepth < 32)
	{
		unsigned char	*oldData = NULL;
		unsigned char	*newData = NULL;
		unsigned int	oldDepth = 0;
		unsigned int	oldDataSize = 0;
		unsigned long	newBlockSize = 0;
		unsigned long	newDataSize = 0;
		unsigned long	pixelCount = 0;
		unsigned long	pixelID = 0;
		unsigned long	dataCount = 0;
		unsigned char	dataValue = 0;
		unsigned char	colorIndex = 0;
		icns_colormap_rgb_t	colorRGB;
		
		oldDepth = imageOut->imageDepth;
		oldDataSize = imageOut->imageDataSize;
		
		pixelCount = imageOut->imageWidth * imageOut->imageHeight;
		
		newBlockSize = imageOut->imageWidth * 32;
		newDataSize = newBlockSize * imageOut->imageHeight;
		
		oldData = imageOut->imageData;
		newData = (unsigned char *)malloc(newDataSize);
		
		if(newData == NULL)
			return -1;
		
		dataCount = 0;
		
		switch(icnsType)
		{
			// 8-Bit Icon Mask Data Types
			case ICNS_128X128_8BIT_MASK:
			case ICNS_64x64_8BIT_MASK:
			case ICNS_32x32_8BIT_MASK:
			case ICNS_16x16_8BIT_MASK:
				if(oldDepth != 8)
				{
					fprintf(stderr,"Bit depth type mismatch!\n");
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
			case ICNS_64x64_8BIT_DATA:
			case ICNS_32x32_8BIT_DATA:
			case ICNS_16x16_8BIT_DATA:
			case ICNS_16x12_8BIT_DATA:
				if(oldDepth != 8)
				{
					fprintf(stderr,"Bit depth type mismatch!\n");
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
			case ICNS_64x64_4BIT_DATA:
			case ICNS_32x32_4BIT_DATA:
			case ICNS_16x16_4BIT_DATA:
			case ICNS_16x12_4BIT_DATA:
				if(oldDepth != 4)
				{
					fprintf(stderr,"Bit depth type mismatch!\n");
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
			case ICNS_64x64_1BIT_DATA:  // Also ICNS_64x64_1BIT_MASK
			case ICNS_32x32_1BIT_DATA: // Also ICNS_32x32_1BIT_MASK
			case ICNS_16x16_1BIT_DATA: // Also ICNS_16x16_1BIT_MASK
			case ICNS_16x12_1BIT_DATA:  // Also ICNS_16x12_1BIT_MASK
				if(oldDepth != 1)
				{
					fprintf(stderr,"Bit depth type mismatch!\n");
					free(newData);
					return -1;
				}
				for(pixelID = 0; pixelID < pixelCount; pixelID++)
				{
					if(pixelID % 8 == 0)
						dataValue = oldData[dataCount++];
					colorIndex = (dataValue & 0x80) ? 255 : 0;
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
		
		imageOut->imageDepth = 32;
		imageOut->imageChannels = 4;
		imageOut->imageDataSize = newDataSize;
		imageOut->imageData = newData;
		
		free(oldData);
	}
	return 0;
}

//***************************** GetIcnsImage32FromIcnsElement **************************//
// Actual conversion of the icon data into uncompressed raw pixels

int GetIcnsImageFromIcnsElement(icns_element_t *iconElement, icns_bool_t swapBytes,icns_image_t *imageOut)
{
	int		error = 0;
	unsigned int	iconWidth = 0;
	unsigned int	iconHeight = 0;
	unsigned int	iconChannels = 0;
	unsigned int	iconDepth = 0;
	unsigned long	iconDataSize = 0;

	unsigned long	dataCount = 0;
	unsigned long	blockSize = 0;
	
	icns_type_t	icnsType = 0x00000000;
	unsigned long	rawDataLength = 0;
	unsigned char	*rawDataPtr = NULL;
	
	if(iconElement == NULL)
	{
		fprintf(stderr,"Icon element is NULL!\n");
		return -1;
	}
	
	if(imageOut == NULL)
	{
		fprintf(stderr,"Icon image structure is NULL!\n");
		return -1;
	}
	
	icnsType = EndianSwap(iconElement->elementType,sizeof(icns_type_t),swapBytes);
	rawDataLength = EndianSwap(iconElement->elementSize,sizeof(icns_size_t),swapBytes);
	rawDataPtr = (unsigned char*)&(iconElement->elementData[0]);
	
	/*
	printf("Icon element type is: 0x%8X\n",(unsigned int)icnsType);
	printf("Icon element size is: %d\n",(int)rawDataLength);	
	*/
	
	// We use the jp2 processor for these two
	if((icnsType == ICNS_512x512_32BIT_ARGB_DATA) || (icnsType == ICNS_256x256_32BIT_ARGB_DATA))
	{
		opj_image_t* image = NULL;

		image = jp2dec((unsigned char *)rawDataPtr, (int)rawDataLength);
		if(!image)
			return -1;
		
		error = opj2icns_image_t(image,imageOut);
		
		opj_image_destroy(image);
		
		return error;
	}
	
	switch(icnsType)
	{
		// 32-Bit Icon Image Data Types
		case ICNS_128X128_32BIT_DATA:
			iconWidth = 128;
			iconHeight = 128;
			iconChannels = 4;
			iconDepth = 32;
			break;
		case ICNS_64x64_32BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 4;
			iconDepth = 32;
			break;
		case ICNS_32x32_32BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 4;
			iconDepth = 32;
			break;
		case ICNS_16x16_32BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 4;
			iconDepth = 32;
			break;
			
		// 8-Bit Icon Mask Data Types
		case ICNS_128X128_8BIT_MASK:
			iconWidth = 128;
			iconHeight = 128;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_64x64_8BIT_MASK:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_32x32_8BIT_MASK:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_16x16_8BIT_MASK:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconDepth = 8;
			break;
			
		// 8-Bit Icon Image Data Types
		case ICNS_64x64_8BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_32x32_8BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_16x16_8BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconDepth = 8;
			break;
		case ICNS_16x12_8BIT_DATA:
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconDepth = 8;
			break;

		// 4-Bit Icon Image Data Types
		case ICNS_64x64_4BIT_DATA:
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconDepth = 4;
			break;
		case ICNS_32x32_4BIT_DATA:
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconDepth = 4;
			break;
		case ICNS_16x16_4BIT_DATA:
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconDepth = 4;
			break;
		case ICNS_16x12_4BIT_DATA:
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconDepth = 4;
			break;

		// 1-Bit Icon Image/Mask Data Types (Data is the same)
		case ICNS_64x64_1BIT_DATA:  // Also ICNS_64x64_1BIT_MASK
			iconWidth = 48;
			iconHeight = 48;
			iconChannels = 1;
			iconDepth = 1;
			break;
		case ICNS_32x32_1BIT_DATA: // Also ICNS_32x32_1BIT_MASK
			iconWidth = 32;
			iconHeight = 32;
			iconChannels = 1;
			iconDepth = 1;
			break;
		case ICNS_16x16_1BIT_DATA: // Also ICNS_16x16_1BIT_MASK
			iconWidth = 16;
			iconHeight = 16;
			iconChannels = 1;
			iconDepth = 1;
			break;
		case ICNS_16x12_1BIT_DATA: // Also ICNS_16x12_1BIT_MASK
			iconWidth = 16;
			iconHeight = 12;
			iconChannels = 1;
			iconDepth = 1;
			break;
			
		default:
			fprintf(stderr,"Unable to parse icon type 0x%8X\n",icnsType);
			return -1;
			break;
	}
	
	blockSize = iconWidth * iconDepth / icns_byte_bits;
	iconDataSize = iconHeight * blockSize;
	
	/*
	printf("Icon image width is: %d\n",iconWidth);
	printf("Icon image height is: %d\n",iconHeight);
	printf("Icon image channels are: %d\n",iconChannels);
	printf("Icon image bit depth is: %d\n",iconDepth);
	printf("Icon image data size is: %d\n",(int)iconDataSize);
	
	printf("Image data size >= element size check: %d >= %d\n",(int)iconDataSize,(int)rawDataLength);
	*/
	
	imageOut->imageWidth = iconWidth;
	imageOut->imageHeight = iconHeight;
	imageOut->imageChannels = iconChannels;
	imageOut->imageDepth = iconDepth;
	imageOut->imageDataSize = iconDataSize;
	imageOut->imageData = (unsigned char *)malloc(iconDataSize);
	if(!imageOut->imageData) {
		fprintf(stderr,"Out of memory!\n");
		return -1;
	}
	memset(imageOut->imageData,0,iconDataSize);
	
	switch(iconDepth)
	{
	case 32:
		if(rawDataLength < (iconHeight * blockSize))
		{
			DecodeRLE24Data(rawDataLength,(icns_sint32_t*)rawDataPtr,iconDataSize,(icns_sint32_t*)(imageOut->imageData));
		}
		else
		{
			for(dataCount = 0; dataCount < iconHeight; dataCount++)
				memcpy(&(((char*)(imageOut->imageData))[dataCount*blockSize]),&(((char*)(rawDataPtr))[dataCount*blockSize]),blockSize);
		}
		if(swapBytes)
		{
			int	packBytes = iconDepth / icns_byte_bits;
			char	*swapPtr = (char*)imageOut->imageData;
			int	*byte4Ptr = NULL;
			
			for(dataCount = 0; dataCount < iconDataSize; dataCount+=packBytes)
			{
				byte4Ptr = (int *)(swapPtr + dataCount);
				*( byte4Ptr ) = EndianSwap( *( byte4Ptr ) , packBytes, 1);
			}
		}
		break;
	case 8:
	case 4:
	case 1:
		for(dataCount = 0; dataCount < iconHeight; dataCount++)
			memcpy(&(((char*)(imageOut->imageData))[dataCount*blockSize]),&(((char*)(rawDataPtr))[dataCount*blockSize]),blockSize);
		break;
	default:
		fprintf(stderr,"Unknown bit depth!\n");
		return -1;
		break;
	}
	
	return error;
}

int DecodeRLE24Data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long dataOutSize, icns_sint32_t *dataOutPtr)
{
	unsigned int	myshift = 0;
	unsigned int	mymask = 0;
	unsigned int	length = 0;
	unsigned int	value = 0;
	long		r = 0;
	long		y = 0;
	long		i = 0;
	unsigned char	*rawDataPtr = NULL;
	long		destIconLength = 0;
	unsigned int	*destIconData = NULL;	// Decompressed Raw Icon Data
	unsigned int	*destIconDataBaseAddr = NULL;	// Decompressed Raw Icon Data Base Address
	
	rawDataPtr = (unsigned char *)dataInPtr;
	destIconData = (unsigned int *)dataOutPtr;
	destIconDataBaseAddr = (unsigned int *)dataOutPtr;
	
	destIconLength = dataOutSize / 4;
	
	myshift = 24;
	mymask = 0xFFFFFFFF;
	
	// What's this??? In the 128x128 icons, we need to start 4 bytes
	// ahead. There see to be a NULL padding here for some reason. If
	// we don't, the red channel will be off by 2 pixels
	r = 4;
	
	for(i = 0; i < destIconLength; i++)
		destIconData[i] = 0x00000000;
	
	// Data is stored in red...run, green...run,blue...run
	// Red, Green, Blue
	// 24,  16,    8   
	while(myshift > 0)
	{
		// Next Color Byte
		myshift -= 8;
		
		// Right shift mask 8 bits to prevent overwriting our other colors
		mymask >>= 8;
		
		y = 0;
		while(y < destIconLength)
		{
			if( (rawDataPtr[r] & 0x80) == 0)
			{
				// Top bit is clear - run of various values to follow
				length = (int)(0xFF & rawDataPtr[r++]) + 1; // 1 <= len <= 128
				
				for(i = 0; i < (int)length; i++)
					destIconData[y++] |= ( ((int)rawDataPtr[r++]) << myshift) & mymask;
			}
			else
			{
				// Top bit is set - run of one value to follow
				length = (int)(0xFF & rawDataPtr[r++]) - 125; // 3 <= len <= 130
				
				// Set the value to the color shifted to the correct bit offset
				value = ( ((int)rawDataPtr[r++]) << myshift) & mymask;
				
				for(i = 0; i < (int)length; i++)
					destIconData[y++] |= value;
			}
		}
	}
	
	destIconDataBaseAddr = NULL;
	destIconData = NULL;
	
	return 0;
}


//***************************** GetIconDataFromIcnsFamily **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

int GetIcnsElementFromIcnsFamily(icns_family_t *icnsFamily,icns_type_t icnsType, icns_bool_t *swapBytes,icns_element_t **iconElementOut)
{
	int		error = 0;
	int		foundData = 0;
	icns_bool_t	bigEndian = ES_IS_BIG_ENDIAN;
	icns_type_t	icnsFamilyDataType = 0x00000000;
	icns_size_t	icnsFamilyDataSize = 0;
	icns_element_t	*icnsElement = NULL;
	icns_type_t	elementDataType = 0x00000000;
	icns_size_t	elementDataSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(icnsFamily == NULL)
	{
		fprintf(stderr,"icns family is NULL!\n");
		return -1;
	}
	
	if(icnsFamily->resourceType == EndianSwap(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE), 1)) {
		*swapBytes = 1;
	} else if(icnsFamily->resourceType == ICNS_FAMILY_TYPE) {
		*swapBytes = 0;
	} else {
		fprintf(stderr,"Invalid icns family!\n");
		error = -1;
	}
	
	if(*swapBytes == bigEndian) {
		printf("Warning: endian not as expected.\n");
	}
	
	icnsFamilyDataType = EndianSwap(icnsFamily->resourceType,sizeof(int),*swapBytes);
	icnsFamilyDataSize = EndianSwap(icnsFamily->resourceSize,sizeof(int),*swapBytes);
	
	/*
	printf("Bytes are swapped in file: %s\n",(*swapBytes ? "yes" : "no"));
	printf("Resource size: %d\n",icnsFamilyDataSize);
	printf("Resource type: 0x%8X ('%c%c%c%c')\n",(unsigned int)icnsFamilyDataType);
	printf("Looking for icon of type: 0x%8X ('%c%c%c%c')\n",(unsigned int)icnsType));
	*/
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < icnsFamilyDataSize) )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementDataType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementDataSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		/*
		printf("Found data...\n");
		printf("  type: 0x%8X\n",(unsigned int)readDataType);
		printf("  size: %d\n",(unsigned int)readDataSize);
		*/

		if (elementDataType == icnsType)
			foundData = 1;
		else
			dataOffset += elementDataSize;
	}
	
	if(foundData)
	{
		*iconElementOut = malloc(elementDataSize);
		if(*iconElementOut == NULL) {
			fprintf(stderr,"Out of Memory\n");
			return -1;
		}
		memcpy( *iconElementOut, icnsElement, elementDataSize);
	}
	else
	{
		fprintf(stderr,"Unable to find requested icon data!\n");
		error = -1;
	}
	
	return error;
}

//***************************** RemoveIcnsElementFromIcnsFamily **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

int RemoveIcnsElementFromIcnsFamily(icns_family_t **icnsFamilyRef,icns_type_t icnsType, icns_bool_t *swapBytes)
{
	int		error = 0;
	int		foundData = 0;
	icns_bool_t	bigEndian = ES_IS_BIG_ENDIAN;
	icns_family_t	*icnsFamily = NULL;
	icns_type_t	icnsFamilyDataType = 0x00000000;
	icns_size_t	icnsFamilyDataSize = 0;
	icns_element_t	*icnsElement = NULL;
	icns_type_t	elementDataType = 0x00000000;
	icns_size_t	elementDataSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(icnsFamilyRef == NULL)
	{
		fprintf(stderr,"icns family reference is NULL!\n");
		return -1;
	}
	
	icnsFamily = *icnsFamilyRef;
	
	if(icnsFamily == NULL)
	{
		fprintf(stderr,"icns family is NULL!\n");
		return -1;
	}
	
	if(icnsFamily->resourceType == EndianSwap(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE), 1)) {
		*swapBytes = 1;
	} else if(icnsFamily->resourceType == ICNS_FAMILY_TYPE) {
		*swapBytes = 0;
	} else {
		fprintf(stderr,"Invalid icns family!\n");
		error = -1;
	}
	
	if(*swapBytes == bigEndian) {
		printf("Warning: endian not as expected.\n");
	}
	
	icnsFamilyDataType = EndianSwap(icnsFamily->resourceType,sizeof(int),*swapBytes);
	icnsFamilyDataSize = EndianSwap(icnsFamily->resourceSize,sizeof(int),*swapBytes);
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < icnsFamilyDataSize) )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementDataType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementDataSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		if (elementDataType == icnsType)
			foundData = 1;
		else
			dataOffset += elementDataSize;
	}
	
	if(foundData)
	{
		icns_size_t	newIcnsFamilySize = 0;
		icns_family_t	*newIcnsFamily = NULL;
		icns_uint32_t	newDataOffset = 0;
		
		newIcnsFamilySize = icnsFamilyDataSize - elementDataSize;
		newIcnsFamily = malloc(newIcnsFamilySize);
		
		if(newIcnsFamily == NULL) {
			fprintf(stderr,"Out of Memory\n");
			return -1;
		}
			
		newIcnsFamilySize = sizeof(icns_type_t) + sizeof(icns_size_t);
		
		newIcnsFamily->resourceType = EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),*swapBytes);
		newIcnsFamily->resourceSize = EndianSwap(newIcnsFamilySize,sizeof(icns_size_t),*swapBytes);
		
		newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
		dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
		
		while ( dataOffset < icnsFamilyDataSize )
		{
			icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
			elementDataType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
			elementDataSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
			
			if (elementDataType != icnsType)
			{
				memcpy( ((char *)(newIcnsFamily))+newDataOffset , ((char *)(icnsFamily))+dataOffset, elementDataSize);
				dataOffset += elementDataSize;
				newDataOffset += elementDataSize;
			}
			else
			{
				dataOffset += elementDataSize;
			}
		}
	}
	else
	{
		fprintf(stderr,"Unable to find requested icon data for removal!\n");
		error = -1;
	}
	
	return error;
}

/***************************** CreateIcnsFamily **************************/

int CreateIcnsFamily(icns_family_t **icnsFamilyOut)
{
	icns_bool_t	swapBytes = 0;
	icns_family_t	*newIcnsFamily = NULL;
	icns_size_t	newIcnsFamilySize = 0;

	if(icnsFamilyOut == NULL)
	{
		fprintf(stderr,"icns family reference is NULL!\n");
		return -1;
	}
	
	swapBytes = ES_IS_LITTLE_ENDIAN;
	
	*icnsFamilyOut = NULL;
	
	newIcnsFamily = malloc(sizeof(icns_type_t) + sizeof(icns_size_t));
		
	if(newIcnsFamily == NULL)
	{
		fprintf(stderr,"Unable to allocate memory for new icns family!\n");
		return -1;
	}
	
	newIcnsFamilySize = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	newIcnsFamily->resourceType = EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),swapBytes);
	newIcnsFamily->resourceSize = EndianSwap(newIcnsFamilySize,sizeof(icns_size_t),swapBytes);
	
	*icnsFamilyOut = newIcnsFamily;
	
	return 0;
}

/***************************** GetIcnsFamilyFromFileData **************************/

int GetIcnsFamilyFromFileData(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **icnsFamilyOut)
{
	int		error = 0;
	icns_bool_t	swapBytes = 0;
	unsigned char	*iconDataPtr = NULL;
	unsigned long	dataOffset = 0;
	
	if(dataSize == 0)
	{
		fprintf(stderr,"File size is 0 - no data!\n");
		return -1;
	}
	
	if(icnsFamilyOut == NULL)
	{
		fprintf(stderr,"icnsFamilyOut is NULL!\n");
		return -1;
	}
	
	// According to Apple Developer Documentation, the icns
	// format is always big endian. So we swap if we are little endian.
	swapBytes = ES_IS_LITTLE_ENDIAN;

	// search for icns entry, NG icns haf various offsets!
	// Note by Mathew 02/13/2008
	// IMHO, this is hackish and should be fixed
	// Are there not specs for the NG format??
	iconDataPtr = dataPtr;
	while ( (dataOffset < dataSize-sizeof(icns_type_t)) && (*((icns_type_t*)(iconDataPtr)) != EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),swapBytes)) ) {
		++dataOffset;
		++iconDataPtr;
	}
	
	// Copy the data to a new block of memory
	if((dataSize-dataOffset) > 0) {
		iconDataPtr = (unsigned char *)malloc(dataSize-dataOffset);
		memcpy (iconDataPtr,dataPtr+dataOffset,dataSize-dataOffset);
	} else {
		iconDataPtr = (unsigned char *)malloc(dataSize);
		memcpy (iconDataPtr,dataPtr,dataSize);
	}
	
	if(*((icns_type_t*)(iconDataPtr)) != EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),swapBytes))
	{
		// Might be embedded in an rsrc file
		if((error = GetIcnsFamilyFromMacResource(dataSize,iconDataPtr,icnsFamilyOut)))
		{
			fprintf(stderr,"Error parsing X Icon resource!\n");
			*icnsFamilyOut = NULL;
		}
	}
	else
	{
		// Data is an X Icon file - no parsing needed at this point
		*icnsFamilyOut = (icns_family_t*)iconDataPtr;
	}
	
	return error;
}

/***************************** GetIcnsFamilyFromMacResourceFork **************************/

int GetIcnsFamilyFromMacResource(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **icnsFamilyOut)
{
	icns_bool_t	swapBytesped = ES_IS_LITTLE_ENDIAN;
	icns_bool_t	error = 0;
	
	icns_bool_t	found = 0;
	unsigned int	count = 0;
	
	unsigned long	parsedSize = 0;
	unsigned char	*parsedData = NULL;
	
	icns_sint32_t	resHeadDataOffset = 0;
	icns_sint32_t	resHeadMapOffset = 0;
	icns_sint32_t	resHeadDataLength = 0;
	icns_sint32_t	resHeadMapLength = 0;
	
	icns_sint16_t	resMapAttributes = 0;
	icns_sint16_t	resMapTypeOffset = 0;
	icns_sint16_t	resMapNameOffset = 0;
	icns_sint16_t	resMapNumTypes = 0;
	
	if(dataSize < 16)
	{
		// rsrc header is 128 bytes - We cannot have a file of a smaller size.
		fprintf(stderr,"Unable to decode rsrc data! - Data size too small.\n");
		return -1;
	}

	// Load Resource Header to if we are dealing with a raw resource fork.
	resHeadDataOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+0)),sizeof(icns_sint32_t),swapBytesped);
	resHeadMapOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+4)),sizeof(icns_sint32_t),swapBytesped);
	resHeadDataLength = EndianSwap(*((icns_sint32_t*)(dataPtr+8)),sizeof(icns_sint32_t),swapBytesped);
	resHeadMapLength = EndianSwap(*((icns_sint32_t*)(dataPtr+12)),sizeof(icns_sint32_t),swapBytesped);
	
	// Check to see if file is not a raw resource file
	if( (resHeadMapOffset+resHeadMapLength != dataSize) || (resHeadDataOffset+resHeadDataLength != resHeadMapOffset) )
	{
		// If not, try reading data as MacBinary file
		error = ParseMacBinaryResourceFork(dataSize,dataPtr,NULL,NULL,&parsedSize,&parsedData);
		if(!error)
		{
			// Reload Actual Resource Header.
			resHeadDataOffset = EndianSwap(*((icns_sint32_t*)(parsedData+0)),sizeof(icns_sint32_t),swapBytesped);
			resHeadMapOffset = EndianSwap(*((icns_sint32_t*)(parsedData+4)),sizeof(icns_sint32_t),swapBytesped);
			resHeadDataLength = EndianSwap(*((icns_sint32_t*)(parsedData+8)),sizeof(icns_sint32_t),swapBytesped);
			resHeadMapLength = EndianSwap(*((icns_sint32_t*)(parsedData+12)),sizeof(icns_sint32_t),swapBytesped);
			
			dataSize = parsedSize;
			dataPtr = parsedData;
		}
	}
	
	if(!error)
	{
		// Load Resource Map
		resMapAttributes = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+0+22)), sizeof(icns_sint16_t),swapBytesped);
		resMapTypeOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+2+22)), sizeof(icns_sint16_t),swapBytesped);
		resMapNameOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+4+22)), sizeof(icns_sint16_t),swapBytesped);
		resMapNumTypes = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+6+22)), sizeof(icns_sint16_t),swapBytesped)+1;
		
		for(count = 0; count < resMapNumTypes && found == 0; count++)
		{
			icns_type_t	resType;
			short	resNumItems = 0;
			short	resOffset = 0;
			
			resType = EndianSwap(*((icns_type_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+2+(count*8))),sizeof(icns_type_t),swapBytesped);
			resNumItems = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+6+(count*8))),sizeof(icns_sint16_t),swapBytesped);
			resOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+8+(count*8))),sizeof(icns_sint16_t),swapBytesped);
			
			if(resType == 0x69636E73) /* 'icns' */
			{
				icns_sint16_t	resID = 0;
				icns_sint8_t	resAttributes = 0;
				icns_sint16_t	resNameOffset = 0;
				icns_sint32_t	resDataOffset = 0;
				icns_sint8_t	resNameLength = 0;
				icns_sint32_t	resDataLength = 0;
				char	resName[256] = {0};
				unsigned char	*resData = NULL;
				
				resID = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset)),sizeof(icns_sint16_t),swapBytesped);
				resNameOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+2)),sizeof(icns_sint16_t),swapBytesped);
				resAttributes = *((icns_sint8_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4));

				// Read three byte int starting at resHeadMapOffset+resMapTypeOffset+resOffset+5
				// Load as long, and then cut off extra inital byte.
				resDataOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4)),sizeof(icns_sint32_t),swapBytesped);
				resDataOffset &= 0x00FFFFFF;

				resDataLength = EndianSwap(*((icns_sint32_t*)(dataPtr+resHeadDataOffset+resDataOffset)),sizeof(icns_sint32_t),swapBytesped);

				if(resNameOffset != -1)
				{
					resNameLength = *((icns_sint8_t*)(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset));

					if(resNameLength > 0)
					{
						memcpy(&resName[0],(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset+1),resNameLength);
					}
				}
				
				if(resDataLength > 0)
				{
					resData = (unsigned char*)malloc(resDataLength);

					if(resData != NULL)
					{
						memcpy(resData,(dataPtr+resHeadDataOffset+resDataOffset+4),resDataLength);
						*icnsFamilyOut = (icns_family_t*)resData;
						found = 1;
					}
					else
					{
						fprintf(stderr,"Error allocating %d bytes of memory!\n",(int)resDataLength);
						*icnsFamilyOut = NULL;
						error = -1;
					}
				}
				else
				{
					fprintf(stderr,"Resource icns id# %d of size 0!\n",resID);
					error = -1;
				}
			}
		}
	}
	
	if(parsedData != NULL)
	{
		free(parsedData);
		parsedData = NULL;
	}
	
	if(!found)
	{
		fprintf(stderr,"Unable to find icon data in file!\n");
		error = -1;
	}

	return error;
}

//**************** ParseMacBinaryResourceFork *******************//
// Parses a MacBinary file resource fork
// Returns the resource fork type, creator, size, and data

int ParseMacBinaryResourceFork(unsigned long dataSize,unsigned char *dataPtr,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,unsigned long *parsedResSizeOut,unsigned char **parsedResDataOut)
{
	// This code is based off information from the MacBinaryIII specification at
	// http://web.archive.org/web/*/www.lazerware.com/formats/macbinary/macbinary_iii.html
	
	int		error = 0;
	int		isValid = 0;
	int		swapBytesped = ES_IS_LITTLE_ENDIAN;
	short		secondHeaderLength = 0;
	long		fileDataPadding = 0;
	long		resourceDataPadding = 0;
	long		fileDataLength = 0;
	long		resourceDataLength = 0;
	long		fileDataStart = 0;
	long		resourceDataStart = 0;
	unsigned char	*resourceDataPtr = NULL;
	
	*dataTypeOut = 0x00000000;
	*dataCreatorOut = 0x00000000;
	*parsedResSizeOut = 0;
	*parsedResDataOut = NULL;
	
	// Begin by checking for valid MacBinary data
	
	if(dataSize < 128)
	{
		// MacBinary header is 128 bytes - We cannot have a file of a smaller size.
		fprintf(stderr,"Unable to decode MacBinary data! - Data size too small.\n");
		return -1;
	}
	
	                                         /* 'mBIN' */
	if(*((icns_type_t*)(dataPtr+65)) == EndianSwap(0x6D42494E,4,swapBytesped))
	{
		// Valid MacBinary III file
		isValid = 1;
	}
	else
	{
		// Bytes 0 and 74 should both be zero in a valid MacBinary file
		if( ( *(dataPtr) == 0 ) && ( *(dataPtr+74) == 0 ) )
		{
			isValid = 1;
		}
		
		// There are other checks beyond this, but in this code,
		// by the time this function is called, chances of getting
		// a match on a non MacBinary file are pretty low...
	}
	
	if( !isValid ) return -1;
	
	// Start MacBinary Parsing routines
	
	// If mac file type is requested, pass it up
	if(dataTypeOut != NULL)
		*dataTypeOut = EndianSwap( *((icns_type_t*)(dataPtr+65)), sizeof(icns_type_t), swapBytesped );

	// If mac file creator is requested, pass it up
	if(dataCreatorOut != NULL)
		*dataCreatorOut = EndianSwap( *((icns_type_t*)(dataPtr+69)), sizeof(icns_type_t), swapBytesped );
	
	// Load up the data lengths
	secondHeaderLength = EndianSwap( *((icns_sint16_t *)(dataPtr+120)), sizeof(icns_sint16_t), swapBytesped );
	fileDataLength = EndianSwap( *((icns_sint32_t *)(dataPtr+83)), sizeof(icns_sint32_t), swapBytesped );
	resourceDataLength = EndianSwap( *((icns_sint32_t *)(dataPtr+87)), sizeof(icns_sint32_t), swapBytesped );

	// Calculate extra padding length for forks
	fileDataPadding = (((fileDataLength + 127) >> 7) << 7) - fileDataLength;
	resourceDataPadding = (((resourceDataLength + 127) >> 7) << 7) - resourceDataLength;

	// Calculate starting offsets for data
	fileDataStart = 128;
	resourceDataStart = fileDataLength + fileDataPadding + 128;

	// Check that we are not reading invalid memory
	if( resourceDataStart < 0 ) return -1;
	if( resourceDataLength < 0 ) return -1;
	if( resourceDataStart < dataSize ) return -1;
	if( resourceDataLength < dataSize ) return -1;
	if( resourceDataStart+resourceDataLength < 0 ) return -1;
	if( resourceDataStart+resourceDataLength < dataSize ) return -1;
	
	resourceDataPtr = (unsigned char *)malloc(resourceDataLength);
	
	if(resourceDataPtr != NULL)
	{
		memcpy(resourceDataPtr,(dataPtr+resourceDataStart),resourceDataLength);
		
		*parsedResSizeOut = resourceDataLength;
		*parsedResDataOut = resourceDataPtr;
	}
	else
	{
		fprintf(stderr,"Error allocating memory for parsed resource!\n");
		error = -1;
	}

	return error;
}
