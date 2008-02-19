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

#include "endianswap.h"
#include "colormaps.h"

#include "icns.h"

#ifdef ICNS_DEBUG
void bin_print_byte(int x)
{
   int n;
   for(n=0; n<8; n++)
   {
	if((x & 0x80) !=0)
	{
	printf("1");
	}
	else
	{
   	printf("0");
   	}
	if (n==3)
	{
	printf(" "); /* insert a space between nybbles */
	}
	x = x<<1;
   }
}

void bin_print_int(int x)
{
   int hi, lo;
   hi=(x>>8) & 0xff;
   lo=x&0xff;
   bin_print_byte(hi);
   printf(" ");
   bin_print_byte(lo);
}
#endif /* ifdef ICNS_DEBUG */

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


int icns_get_image32_from_element(icns_element_t *iconElement, icns_bool_t swapBytes,icns_image_t *imageOut)
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
	
	error = icns_get_image_from_element(iconElement,swapBytes,imageOut);
	
	if(error)
		return -1;
	
	icnsType = EndianSwap(iconElement->elementType,sizeof(icns_type_t),swapBytes);
	
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

int icns_get_image_from_element(icns_element_t *iconElement, icns_bool_t swapBytes,icns_image_t *imageOut)
{
	int		error = 0;
	unsigned long	dataCount = 0;	
	icns_type_t	icnsType = 0x00000000;
	unsigned long	rawDataSize = 0;
	unsigned char	*rawDataPtr = NULL;
	unsigned int	iconBitDepth = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	iconDataRowSize = 0;
	
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
	
	icnsType = EndianSwap(iconElement->elementType,sizeof(icns_type_t),swapBytes);
	rawDataSize = EndianSwap(iconElement->elementSize,sizeof(icns_size_t),swapBytes);
	rawDataSize = rawDataSize - sizeof(icns_type_t) - sizeof(icns_size_t);
	rawDataPtr = (unsigned char*)&(iconElement->elementData[0]);
	
	/*
	printf("Icon element type is: 0x%8X\n",(unsigned int)icnsType);
	printf("Icon element size is: %d\n",(int)rawDataSize);	
	*/
	
	// We use the jp2 processor for these two
	if((icnsType == ICNS_512x512_32BIT_ARGB_DATA) || (icnsType == ICNS_256x256_32BIT_ARGB_DATA))
	{
		#ifdef ICNS_OPENPJEG
		
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
		if(swapBytes)
		{
			int	packBytes = iconBitDepth / icns_byte_bits;
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

//***************************** icns_decode_rle24_data ****************************//
// Decode a rgb 24 bit rle encoded data stream into 32 bit argb (alpha is ignored)

int icns_decode_rle24_data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long dataOutSize, icns_sint32_t *dataOutPtr)
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
	
	if(dataInPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_decode_rle24_data: rle decoder data in ptr is NULL!\n");
		return -1;
	}
	
	if(dataOutPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_decode_rle24_data: rle decoder data out ptr is NULL!\n");
		return -1;
	}
	
	rawDataPtr = (unsigned char *)dataInPtr;
	destIconData = (unsigned int *)dataOutPtr;
	destIconDataBaseAddr = (unsigned int *)dataOutPtr;
	
	// There's always going to be 4 channels in this
	// so we want our counter to increment through
	// channels, not bytes....
	destIconLength = dataOutSize / 4;
	
	myshift = 24;
	mymask = 0xFFFFFFFF;
	
	// What's this??? In the 128x128 icons, we need to start 4 bytes
	// ahead. There see to be a NULL padding here for some reason. If
	// we don't, the red channel will be off by 2 pixels
	if( *((unsigned int*)dataInPtr) == 0x00000000 )
	{
		#ifdef ICNS_DEBUG
		printf("4 byte null padding found in rle data!\n");
		#endif
		r = 4;
	}
	
	for(i = 0; i < destIconLength; i++)
		destIconData[i] = 0x00000000;
		
	// Data is stored in red...run, green...run,blue...run
	// Red, Green, Blue
	// 24,  16,    8   
	while(myshift > 0)
	{
		int	runCount = 0;
		int	dataSum = 0;

		// Next Color Byte
		myshift -= 8;
		
		// Right shift mask 8 bits to prevent overwriting our other colors
		mymask >>= 8;
		
		// Start with mask 0x00FFFFFF, then 0x0000FFFF, then 0x0000FF
		
		y = 0;
		while(y < destIconLength)
		{
			if( (rawDataPtr[r] & 0x80) == 0)
			{
				// Top bit is clear - run of various values to follow
				length = (int)(0xFF & rawDataPtr[r++]) + 1; // 1 <= len <= 128
				dataSum += length;
				
				for(i = 0; i < (int)length; i++)
					destIconData[y++] |= ( ((int)rawDataPtr[r++]) << myshift) & mymask;
				
				runCount++;
			}
			else
			{
				// Top bit is set - run of one value to follow
				length = (int)(0xFF & rawDataPtr[r++]) - 125; // 3 <= len <= 130
				dataSum += 2;
				
				// Set the value to the color shifted to the correct bit offset
				value = ( ((int)rawDataPtr[r++]) << myshift) & mymask;
				
				for(i = 0; i < (int)length; i++)
					destIconData[y++] |= value;
				
				runCount++;
			}
		}
		
	}
	
	destIconDataBaseAddr = NULL;
	destIconData = NULL;
	
	return 0;
}

//***************************** icns_encode_rle24_data *******************************************//
// Encode an 32 bit argb data stream into a 24 bit rgb rle encoded data stream (alpha is ignored)

int icns_encode_rle24_data(unsigned long dataInSize, icns_sint32_t *dataInPtr,unsigned long *dataOutSize, icns_sint32_t **dataOutPtr)
{
	unsigned long	dataInCount = 0;
	unsigned long 	dataInChanSize = 0;
	icns_sint8_t	*dataTemp = NULL;
	unsigned long	dataTempCount = 0;
	icns_sint32_t	dataValue = 0;
	icns_uint8_t	dataByte = 0;
	icns_uint8_t	*dataRun = NULL;
	icns_bool_t	runType = 0;
	icns_uint8_t	runLength = 0; // Runs will never go over 130, one byte is ok
	int		runCount = 0;
	unsigned int	myshift = 0;
	unsigned int	mymask = 0;
	
	if(dataInPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_encode_rle24_data: rle encoder data in ptr is NULL!\n");
		return -1;
	}
	
	if(dataOutSize == NULL)
	{
		fprintf(stderr,"libicns: icns_encode_rle24_data: rle encoder data out size ref is NULL!\n");
		return -1;
	}
	
	if(dataOutPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_encode_rle24_data: rle encoder data out ptr ref is NULL!\n");
		return -1;
	}

	// Assumptions of what icns rle data is all about:
	// A) Each channel is encoded indepenent of the next.
	// B) An encoded channel looks like this:
	//    0xRL 0xCV 0xCV 0xRL 0xCV - RL is run-length and CV is color value.
	// C) There are two types of runs
	//    1) Run of same value - high bit of RL is set
	//    2) Run of differing values - high bit of RL is NOT set
	// D) 0xRL also has two ranges
	//    1) for set high bit RL, 3 to 130
	//    2) for clr high bit RL, 1 to 128
	// E) 0xRL byte is therefore set as follows:
	//    1) for same values, RL = RL - 1
	//    2) different values, RL = RL + 125
	//    3) both methods will automatically set the high bit appropriately
	// F) 0xCV byte are set accordingly
	//    1) for differing values, run of all differing values
	//    2) for same values, only one byte of that values
	// Estimations put the absolute worst case scenario as the
	// final compressed data being slightly LARGER. So we need to be
	// careful about allocating memory. (Did I miss something?)
	// tests seem to indicate it will never be larger than the original
	
	// This block is for the new RLE encoded data - make it 25% larger
	dataTemp = (icns_sint8_t *)malloc(dataInSize + (dataInSize / 4));
	if(dataTemp == NULL)
	{
		fprintf(stderr,"libicns: icns_encode_rle24_data: Unable to allocate memory block of size: %d!\n",(int)dataInSize);
		return -1;
	}
	memset(dataTemp,0,dataInSize);
	
	// This block is for a run of RLE encoded data
	dataRun = (icns_uint8_t *)malloc(128);
	if(dataRun == NULL)
	{
		fprintf(stderr,"libicns: icns_encode_rle24_data: Unable to allocate memory block of size: %d!\n",128);
		free(dataTemp);
		return -1;
	}
	memset(dataRun,0,128);
	
	// There's always going to be 4 channels in this
	// so we want our counter to increment through
	// channels, not bytes....
	dataInChanSize = dataInSize / 4;
	
	myshift = 24;
	mymask = 0xFF000000;
	
	// Move forward 4 bytes - who knows why this should be
	dataTempCount = 4;
	
	while(myshift > 0)
	{
		int	dataSum = 0;
		
		runCount = 0;
		
		// Next Color Byte
		myshift -= 8;
		
		// Right shift mask 8 bits to prevent overwriting our other colors
		mymask >>= 8;
		
		// Set the first byte of the run...
		dataRun[0] = (icns_uint8_t)((*dataInPtr & mymask) >> myshift);
		
		// Start with a runlength of 1 for the first byte
		runLength = 1;
		
		// Assume that the run will be different for now... We can change this later
		runType = 0; // 0 for low bit (different), 1 for high bit (same)	
		
		// Start one byte ahead
		for(dataInCount = 1; dataInCount < dataInChanSize; dataInCount++)
		{
			dataValue = *(dataInPtr+dataInCount);
			dataByte = (icns_uint8_t)((dataValue & mymask) >> myshift);	// Red Channel
			
			if(runLength < 2)
			{
				// Simply append to the current run
				dataRun[runLength++] = dataByte;
			}
			else if(runLength == 2)
			{
				// Decide here if the run should be same values or different values
				
				// If the last three values were the same, we can change to a same-type run
				if((dataByte == dataRun[runLength-1])&&(dataByte == dataRun[runLength-2]))
					runType = 1;
				else
					runType = 0;
				
				dataRun[runLength++] = dataByte;
			}
			else // Greater than or equal to 2
			{
				if(runType == 0 && runLength < 128) // Different type run
				{
					// If the new value matches both of the last two values, we have a new
					// same-type run starting with the previous two bytes
					if((dataByte == dataRun[runLength-1])&&(dataByte == dataRun[runLength-2]))
					{
						// Set the RL byte
						*(dataTemp+dataTempCount) = runLength - 1;
						dataTempCount++;
						// Copy 0 to runLength-2 bytes to the RLE data here
						memcpy( dataTemp+dataTempCount , dataRun , runLength - 2 );
						dataTempCount = dataTempCount + (runLength - 2);
						runCount++;
						
						dataSum += (runLength - 2);
						
						// Set up the new same-type run
						dataRun[0] = dataRun[runLength-2];
						dataRun[1] = dataRun[runLength-1];
						dataRun[2] = dataByte;
						runLength = 3;
						runType = 1;
					}
					else // They don't match, so we can proceed
					{
						dataRun[runLength++] = dataByte;
					}
				}
				else if(runType == 1 && runLength < 130) // Same type run
				{
					// If the new value matches both of the last two values, we
					// can safely continue
					if((dataByte == dataRun[runLength-1])&&(dataByte == dataRun[runLength-2]))
					{
						dataRun[runLength++] = dataByte;
					}
					else // They don't match, so we need to start a new run
					{
						// Set the RL byte
						*(dataTemp+dataTempCount) = runLength + 125;
						dataTempCount++;
						
						// Only copy the first byte, since all the remaining values are identical
						*(dataTemp+dataTempCount) = dataRun[0];
						dataTempCount++;
						runCount++;
						
						dataSum += 2;
						
						// Copy 0 to runLength bytes to the RLE data here
						dataRun[0] = dataByte;
						runLength = 1;
						runType = 0;
					}
				}
				else // Exceeded run limit, need to start a new one
				{
					if(runType == 0)
					{
						// Set the RL byte low
						*(dataTemp+dataTempCount) = runLength - 1;
						
						// Copy 0 to runLength bytes to the RLE data here
						memcpy( dataTemp+dataTempCount , dataRun , runLength );
						dataTempCount = dataTempCount + runLength;
						
						dataSum += runLength;
					}
					else if(runType == 1)
					{
						// Set the RL byte high
						*(dataTemp+dataTempCount) = runLength + 125;
						dataTempCount++;
						
						// Only copy the first byte, since all the remaining values are identical
						*(dataTemp+dataTempCount) = dataRun[0];
						dataTempCount++;
						
						dataSum += 2;
					}
					
					runCount++;
					
					// Copy 0 to runLength bytes to the RLE data here
					dataRun[0] = dataByte;
					runLength = 1;
					runType = 0;
				}
			}
		}
		
		// Copy the end of the last run
		if(runLength > 0)
		{
			if(runType == 0)
			{
				// Set the RL byte low
				*(dataTemp+dataTempCount) = runLength - 1;
				
				// Copy 0 to runLength bytes to the RLE data here
				memcpy( dataTemp+dataTempCount , dataRun , runLength );
				dataTempCount = dataTempCount + runLength;
				
				dataSum += runLength;
			}
			else if(runType == 1)
			{
				// Set the RL byte high
				*(dataTemp+dataTempCount) = runLength + 125;
				dataTempCount++;
				
				// Only copy the first byte, since all the remaining values are identical
				*(dataTemp+dataTempCount) = dataRun[0];
				dataTempCount++;
				
				dataSum += 2;
			}
			
			runCount++;
		}
		
	}
	
	free(dataRun);
	free(dataTemp);
	
	return 0;
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

// Only compile the openjpeg routines if we have support for it
#ifdef ICNS_OPENPJEG

/**
sample error callback expecting a FILE* client object
*/
void icns_opj_error_callback(const char *msg, void *client_data) {
	//FILE *stream = (FILE*)client_data;
	//fprintf(stream, "[ERROR] %s", msg);
}
/**
sample warning callback expecting a FILE* client object
*/
void icns_opj_warning_callback(const char *msg, void *client_data) {
	//FILE *stream = (FILE*)client_data;
	//fprintf(stream, "[WARNING] %s", msg);
}
/**
sample debug callback expecting no client object
*/
void icns_opj_info_callback(const char *msg, void *client_data) {
	//(void)client_data;
	//fprintf(stdout, "[INFO] %s", msg);
}

// Convert from uncompressed opj data to ICNS_ImageData
int icns_opj_to_image(opj_image_t *image, icns_image_t *outIcon)
{
	int		error = 0;
	unsigned int	iconWidth = 0;
	unsigned int	iconHeight = 0;
	unsigned int	iconPixelDepth = 0;
	unsigned int	iconChannels = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	blockSize = 0;
	int adjustR, adjustG, adjustB, adjustA;
	unsigned char	*dataPtr = NULL;
	int i,j;
	
	if(image == NULL) {
		return -1;
	}
	
	iconWidth = image->comps[0].w;
	iconHeight = image->comps[0].h;
	iconChannels = image->numcomps;
	iconPixelDepth = image->comps[0].prec;
	
	blockSize = iconWidth * iconPixelDepth; // * iconChannels ?
	iconDataSize = iconHeight * blockSize;
	outIcon->imageWidth = iconWidth;
	outIcon->imageHeight = iconHeight;
	outIcon->imageChannels = iconChannels;
	outIcon->pixel_depth = iconPixelDepth;
	outIcon->imageDataSize = iconDataSize;
	outIcon->imageData = (unsigned char *)malloc(iconDataSize);
	if(!outIcon->imageData) {
		printf("Failed alloc iconData\n");
		return 0;
	}
	memset(outIcon->imageData,0,iconDataSize);
	dataPtr = outIcon->imageData;
	
	if (image->comps[3].prec > 8) {
		adjustR = image->comps[3].prec - 8;
		//printf("BMP CONVERSION: Truncating component 3 from %d bits to 8 bits\n",image->comps[3].prec);
	} else 
		adjustA = 0;
	if (image->comps[0].prec > 8) {
		adjustR = image->comps[0].prec - 8;
		//printf("BMP CONVERSION: Truncating component 0 from %d bits to 8 bits\n",image->comps[0].prec);
	} else 
		adjustR = 0;
	if (image->comps[1].prec > 8) {
		adjustG = image->comps[1].prec - 8;
		//printf("BMP CONVERSION: Truncating component 1 from %d bits to 8 bits\n",image->comps[1].prec);
	} else 
		adjustG = 0;
	if (image->comps[2].prec > 8) {
		adjustB = image->comps[2].prec - 8;
		//printf("BMP CONVERSION: Truncating component 2 from %d bits to 8 bits\n",image->comps[2].prec);
	} else 
		adjustB = 0;
	
	for (i = 0; i < iconHeight; i++) {
		for(j = 0; j < iconWidth; j++) {
			icns_pixel32_t *dst_pixel;
			int r, g, b, a;
						
			a = image->comps[3].data[i*iconWidth+j];
			a += (image->comps[3].sgnd ? 1 << (image->comps[3].prec - 1) : 0);
			r = image->comps[0].data[i*iconWidth+j];
			r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
			g = image->comps[1].data[i*iconWidth+j];
			g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
			b = image->comps[2].data[i*iconWidth+j];
			b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

			dst_pixel = (icns_pixel32_t *)&(dataPtr[i*iconWidth*iconChannels+j*iconChannels]);

			dst_pixel->alpha = (unsigned char) ((a >> adjustA)+((a >> (adjustA-1))%2));
			dst_pixel->red = (unsigned char) ((r >> adjustR)+((r >> (adjustR-1))%2));
			dst_pixel->green = (unsigned char) ((g >> adjustG)+((g >> (adjustG-1))%2));
			dst_pixel->blue = (unsigned char) ((b >> adjustB)+((b >> (adjustB-1))%2));
		}
	}
	
	return error;
}

// Decompress jp2
opj_image_t * jp2dec(unsigned char *bufin, int len)
{
	opj_dparameters_t parameters;	/* decompression parameters */
	opj_dinfo_t* dinfo = NULL;
	opj_event_mgr_t event_mgr;		/* event manager */
	opj_cio_t *cio = NULL;
	opj_image_t *image = NULL;

	/* configure the event callbacks (not required) */
	memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
	event_mgr.error_handler = icns_opj_error_callback;
	event_mgr.warning_handler = icns_opj_warning_callback;
	event_mgr.info_handler = icns_opj_info_callback;

	/* get a decoder handle */
	dinfo = opj_create_decompress(CODEC_JP2);
	
	/* set decoding parameters to default values */
	opj_set_default_decoder_parameters(&parameters);

	/* catch events using our callbacks and give a local context */
	opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);

	opj_setup_decoder(dinfo, &parameters);

	/* open a byte stream */
	cio = opj_cio_open((opj_common_ptr)dinfo, bufin, len);

	image = opj_decode(dinfo, cio);
	if(!image) {
		fprintf(stderr, "libicns: jp2dec: failed to decode image!\n");
		opj_destroy_decompress(dinfo);
		opj_cio_close(cio);
		return image;
	}

	/* close the byte stream */
	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);

	return image;
}

#endif /* ifdef ICNS_OPENPJEG */



//***************************** icns_new_element_from_image **************************//
// Creates a new icns element from an image
int icns_new_element_from_image(icns_image_t *imageIn,icns_type_t icnsType,icns_element_t **iconElementOut)
{
	int		error = 0;
	icns_bool_t	swapBytes = ES_IS_LITTLE_ENDIAN;
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
	
	switch(icnsType)
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
			fprintf(stderr,"libicns: icns_new_element_from_image: Unable to parse icon type 0x%8X\n",icnsType);
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
	
	if( (icnsType == ICNS_256x256_32BIT_ARGB_DATA) || (icnsType == ICNS_512x512_32BIT_ARGB_DATA) )
	{
		// Future openjpeg routines to go here
		fprintf(stderr,"libicns: icns_new_element_from_image: libicns does't support importing large icons yet.\n");
		return -1;
	}
	else
	{
		if(icnsType == ICNS_128X128_32BIT_DATA)
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
	newElementType = icnsType;
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
	
	newElement->elementSize = EndianSwap(newElementSize,sizeof(icns_size_t),swapBytes);
	newElement->elementType = EndianSwap(newElementType,sizeof(icns_type_t),swapBytes);
	
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

//***************************** GetIconDataFromIcnsFamily **************************//
// Parses requested data from an icon family - puts it into a icns element

int icns_get_element_from_family(icns_family_t *icnsFamily,icns_type_t icnsType, icns_bool_t *swapBytes,icns_element_t **iconElementOut)
{
	int		error = 0;
	int		foundData = 0;
	icns_bool_t	bigEndian = ES_IS_BIG_ENDIAN;
	icns_type_t	icnsFamilyType = 0x00000000;
	icns_size_t	icnsFamilySize = 0;
	icns_element_t	*icnsElement = NULL;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(icnsFamily == NULL)
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
	
	if(icnsFamily->resourceType == EndianSwap(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE), 1)) {
		*swapBytes = 1;
	} else if(icnsFamily->resourceType == ICNS_FAMILY_TYPE) {
		*swapBytes = 0;
	} else {
		fprintf(stderr,"libicns: icns_get_element_from_family: Invalid icns family!\n");
		error = -1;
	}
	
	if(*swapBytes == bigEndian) {
		printf("libicns: icns_get_element_from_family: warning: endian not as expected.\n");
	}
	
	icnsFamilyType = EndianSwap(icnsFamily->resourceType,sizeof(int),*swapBytes);
	icnsFamilySize = EndianSwap(icnsFamily->resourceSize,sizeof(int),*swapBytes);
	
	#ifdef ICNS_DEBUG
	printf("Bytes are swapped in file: %s\n",(*swapBytes ? "yes" : "no"));
	printf("Resource size: %d\n",icnsFamilySize);
	printf("Resource type: 0x%8X\n",(unsigned int)icnsFamilyType);
	printf("Looking for icon of type: 0x%8X\n",(unsigned int)icnsType);
	#endif
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < icnsFamilySize) )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		#ifdef ICNS_DEBUG
		printf("Found data...\n");
		printf("  type: 0x%8X\n",(unsigned int)elementType);
		printf("  size: %d\n",(unsigned int)elementSize);
		#endif

		if (elementType == icnsType)
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
		memcpy( *iconElementOut, icnsElement, elementSize);
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

int icns_set_element_in_family(icns_family_t **icnsFamilyRef,icns_element_t *newIcnsElement, icns_bool_t *swapBytes)
{
	int		error = 0;
	int		foundData = 0;
	int		copiedData = 0;
	icns_bool_t	bigEndian = ES_IS_BIG_ENDIAN;
	icns_family_t	*icnsFamily = NULL;
	icns_type_t	icnsFamilyType = 0x00000000;
	icns_size_t	icnsFamilySize = 0;
	icns_element_t	*icnsElement = NULL;
	icns_type_t	newElementType = 0x00000000;
	icns_size_t	newElementSize = 0;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	icns_size_t	newIcnsFamilySize = 0;
	icns_family_t	*newIcnsFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	if(icnsFamilyRef == NULL)
	{
		fprintf(stderr,"libicns:icns_set_element_in_family: icns family reference is NULL!\n");
		return -1;
	}
	
	icnsFamily = *icnsFamilyRef;
	
	if(icnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: icns family is NULL!\n");
		return -1;
	}
	
	if(icnsFamily->resourceType == EndianSwap(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE), 1)) {
		*swapBytes = 1;
	} else if(icnsFamily->resourceType == ICNS_FAMILY_TYPE) {
		*swapBytes = 0;
	} else {
		fprintf(stderr,"libicns: icns_set_element_in_family: Invalid icns family!\n");
		error = -1;
	}
	
	if(*swapBytes == bigEndian) {
		printf("libicns: icns_set_element_in_family: warning: endian not as expected.\n");
	}
	
	icnsFamilyType = EndianSwap(icnsFamily->resourceType,sizeof(int),*swapBytes);
	icnsFamilySize = EndianSwap(icnsFamily->resourceSize,sizeof(int),*swapBytes);

	if(newIcnsElement == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: icns element is NULL!\n");
		return -1;
	}
	
	newElementType = EndianSwap(newIcnsElement->elementType,sizeof(int),*swapBytes);
	newElementSize = EndianSwap(newIcnsElement->elementSize,sizeof(int),*swapBytes);
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < icnsFamilySize) )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		if (elementType == newElementType)
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(foundData)
		newIcnsFamilySize = icnsFamilySize - elementSize + newElementSize;
	else
		newIcnsFamilySize = icnsFamilySize + newElementSize;

	newIcnsFamily = malloc(newIcnsFamilySize);
	
	if(newIcnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_set_element_in_family: Unable to allocate memory block of size: %d!\n",newIcnsFamilySize);
		return -1;
	}

	newIcnsFamily->resourceType = EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),*swapBytes);
	newIcnsFamily->resourceSize = EndianSwap(newIcnsFamilySize,sizeof(icns_size_t),*swapBytes);
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	copiedData = 0;
	
	while ( dataOffset < icnsFamilySize )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		if (elementType != newElementType)
		{
			memcpy( ((char *)(newIcnsFamily))+newDataOffset , ((char *)(icnsFamily))+dataOffset, elementSize);
			newDataOffset += elementSize;
		}
		else
		{
			memcpy( ((char *)(newIcnsFamily))+newDataOffset , (char *)newIcnsElement, newElementSize);
			newDataOffset += newElementSize;
			copiedData = 1;
		}

		dataOffset += elementSize;
	}
	
	if(!copiedData)
	{
		memcpy( ((char *)(newIcnsFamily))+newDataOffset , (char *)newIcnsElement, newElementSize);
		newDataOffset += newElementSize;
	}
	
	*icnsFamilyRef = newIcnsFamily;
	
	free(icnsFamily);
	
	return error;
}

//***************************** icns_remove_element_in_family **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

int icns_remove_element_in_family(icns_family_t **icnsFamilyRef,icns_type_t icnsElementType, icns_bool_t *swapBytes)
{
	int		error = 0;
	int		foundData = 0;
	icns_bool_t	bigEndian = ES_IS_BIG_ENDIAN;
	icns_family_t	*icnsFamily = NULL;
	icns_type_t	icnsFamilyType = 0x00000000;
	icns_size_t	icnsFamilySize = 0;
	icns_element_t	*icnsElement = NULL;
	icns_type_t	elementType = 0x00000000;
	icns_size_t	elementSize = 0;
	icns_uint32_t	dataOffset = 0;
	
	if(icnsFamilyRef == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: icns family reference is NULL!\n");
		return -1;
	}
	
	icnsFamily = *icnsFamilyRef;
	
	if(icnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: icns family is NULL!\n");
		return -1;
	}
	
	if(icnsFamily->resourceType == EndianSwap(ICNS_FAMILY_TYPE, sizeof(ICNS_FAMILY_TYPE), 1)) {
		*swapBytes = 1;
	} else if(icnsFamily->resourceType == ICNS_FAMILY_TYPE) {
		*swapBytes = 0;
	} else {
		fprintf(stderr,"libicns: icns_remove_element_in_family: Invalid icns family!\n");
		error = -1;
	}
	
	if(*swapBytes == bigEndian) {
		printf("libicns: icns_remove_element_in_family: warning: endian not as expected.\n");
	}
	
	icnsFamilyType = EndianSwap(icnsFamily->resourceType,sizeof(int),*swapBytes);
	icnsFamilySize = EndianSwap(icnsFamily->resourceSize,sizeof(int),*swapBytes);
	
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( (foundData == 0) && (dataOffset < icnsFamilySize) )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		if (elementType == icnsElementType)
			foundData = 1;
		else
			dataOffset += elementSize;
	}
	
	if(!foundData)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: Unable to find requested icon data for removal!\n");
		return -1;
	}
	
	icns_size_t	newIcnsFamilySize = 0;
	icns_family_t	*newIcnsFamily = NULL;
	icns_uint32_t	newDataOffset = 0;
	
	newIcnsFamilySize = icnsFamilySize - elementSize;
	newIcnsFamily = malloc(newIcnsFamilySize);
	
	if(newIcnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_remove_element_in_family: Unable to allocate memory block of size: %d!\n",newIcnsFamilySize);
		return -1;
	}
	
	newIcnsFamily->resourceType = EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),*swapBytes);
	newIcnsFamily->resourceSize = EndianSwap(newIcnsFamilySize,sizeof(icns_size_t),*swapBytes);
	
	newDataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	
	while ( dataOffset < icnsFamilySize )
	{
		icnsElement = ((icns_element_t*)(((char*)icnsFamily)+dataOffset));
		elementType = EndianSwap(icnsElement->elementType,sizeof(icns_type_t),*swapBytes);
		elementSize = EndianSwap(icnsElement->elementSize,sizeof(icns_size_t),*swapBytes);
		
		if (elementType != icnsElementType)
		{
			memcpy( ((char *)(newIcnsFamily))+newDataOffset , ((char *)(icnsFamily))+dataOffset, elementSize);
			newDataOffset += elementSize;
		}
		
		dataOffset += elementSize;
	}
	
	*icnsFamilyRef = newIcnsFamily;

	free(icnsFamily);
	
	return error;
}

/***************************** icns_create_family **************************/

int icns_create_family(icns_family_t **icnsFamilyOut)
{
	icns_bool_t	swapBytes = 0;
	icns_family_t	*newIcnsFamily = NULL;
	icns_size_t	newIcnsFamilySize = 0;

	if(icnsFamilyOut == NULL)
	{
		fprintf(stderr,"libicns: icns_create_family: icns family reference is NULL!\n");
		return -1;
	}
	
	swapBytes = ES_IS_LITTLE_ENDIAN;
	
	*icnsFamilyOut = NULL;
	
	newIcnsFamilySize = sizeof(icns_type_t) + sizeof(icns_size_t);

	newIcnsFamily = malloc(newIcnsFamilySize);
		
	if(newIcnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_create_family: Unable to allocate memory block of size: %d!\n",newIcnsFamilySize);
		return -1;
	}
	
	newIcnsFamily->resourceType = EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),swapBytes);
	newIcnsFamily->resourceSize = EndianSwap(newIcnsFamilySize,sizeof(icns_size_t),swapBytes);
	
	*icnsFamilyOut = newIcnsFamily;
	
	return 0;
}

/***************************** icns_write_family_to_file **************************/

int icns_write_family_to_file(FILE *dataFile,icns_family_t *icnsFamilyIn)
{
	icns_bool_t	swapBytes = ES_IS_LITTLE_ENDIAN;
	icns_size_t	resourceSize = 0;
	icns_size_t	blockSize = 0;
	icns_uint32_t	dataOutTotal = 0;
	icns_uint32_t	dataOutBlock = 0;
	
	if ( dataFile == NULL )
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: NULL file pointer!\n");
		return -1;
	}	
	
	if ( icnsFamilyIn == NULL )
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: NULL icns family!\n");
		return -1;
	}
	
	resourceSize = EndianSwap(icnsFamilyIn->resourceSize,sizeof(icns_size_t),swapBytes);
	blockSize = 1024;
	
	while( (resourceSize-dataOutTotal) > blockSize)
	{
		dataOutBlock = fwrite ( ((char *)icnsFamilyIn) + dataOutTotal , blockSize , 1 , dataFile );
		if(dataOutBlock != blockSize)
		{
			fprintf(stderr,"libicns: icns_write_family_to_file: Error writing icns to file!\n");
			return -1;
		}
		dataOutTotal += dataOutBlock;
	}
	
	blockSize = (resourceSize-dataOutTotal);
	
	dataOutBlock = fwrite ( ((char *)icnsFamilyIn) + dataOutTotal , blockSize , 1 , dataFile );
	if(dataOutBlock != blockSize)
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: Error writing icns to file!\n");
		return -1;
	}
	dataOutTotal += dataOutBlock;
	
	return 0;
}


/***************************** icns_read_family_from_file **************************/

int icns_read_family_from_file(FILE *dataFile,icns_family_t **icnsFamilyOut)
{
	int	      error = 0;
	unsigned long dataSize = 0;
	void          *dataPtr = NULL;
	
	if ( dataFile == NULL )
	{
		fprintf(stderr,"libicns: icns_read_family_from_file: NULL file pointer!\n");
		return -1;
	}
	
	if ( icnsFamilyOut == NULL )
	{
		fprintf(stderr,"libicns: icns_read_family_from_file: NULL icns family ref!\n");
		return -1;
	}
	
	if(fseek(dataFile,0,SEEK_END) == 0)
	{
		dataSize = ftell(dataFile);
		rewind(dataFile);
		
		dataPtr = (void *)malloc(dataSize);

		if ( (error == 0) && (dataPtr != NULL) )
		{
			if(fread( dataPtr, sizeof(char), dataSize, dataFile) != dataSize)
			{
				free( dataPtr );
				dataPtr = NULL;
				dataSize = 0;
				error = -1;
				fprintf(stderr,"libicns: icns_read_family_from_file: Error occured reading file!\n");
			}
		}
		else
		{
			error = -1;
			fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to allocate memory block of size: %d!\n",(int)dataSize);
		}
	}
	else
	{
		error = -1;
		fprintf(stderr,"libicns: icns_read_family_from_file: Error occured seeking to end of file!\n");
	}
	
	if(error == 0)
		error = icns_family_from_file_data(dataSize,dataPtr,icnsFamilyOut);
	
	if(dataPtr != NULL)
	{
		free(dataPtr);
		dataPtr = NULL;
	}
	
	return error;
}


/***************************** icns_family_from_file_data **************************/

int icns_family_from_file_data(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **icnsFamilyOut)
{
	int		error = 0;
	icns_bool_t	swapBytes = 0;
	unsigned char	*iconDataPtr = NULL;
	unsigned long	dataOffset = 0;
	
	if(dataSize == 0)
	{
		fprintf(stderr,"libicns: icns_family_from_file_data: File size is 0 - no data!\n");
		return -1;
	}
	
	if(icnsFamilyOut == NULL)
	{
		fprintf(stderr,"libicns: icns_family_from_file_data: icnsFamilyOut is NULL!\n");
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
		dataSize = dataSize-dataOffset;
	} else {
		iconDataPtr = (unsigned char *)malloc(dataSize);
		memcpy (iconDataPtr,dataPtr,dataSize);
	}
	
	if(*((icns_type_t*)(iconDataPtr)) != EndianSwap(ICNS_FAMILY_TYPE,sizeof(icns_type_t),swapBytes))
	{
		// Might be embedded in an rsrc file
		if((error = icns_family_from_mac_resource(dataSize,iconDataPtr,icnsFamilyOut)))
		{
			fprintf(stderr,"libicns: icns_family_from_file_data: Error parsing X Icon resource!\n");
			free(iconDataPtr);
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

/***************************** icns_family_from_mac_resourceFork **************************/

int icns_family_from_mac_resource(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **icnsFamilyOut)
{
	icns_bool_t	swapBytes = ES_IS_LITTLE_ENDIAN;
	icns_bool_t	error = 0;
	
	icns_bool_t	found = 0;
	unsigned int	count = 0;
	
	unsigned long	parsedSize = 0;
	unsigned char	*parsedData = NULL;
	
	icns_sint32_t	resHeadDataOffset = 0;
	icns_sint32_t	resHeadMapOffset = 0;
	icns_sint32_t	resHeadDataSize = 0;
	icns_sint32_t	resHeadMapLength = 0;
	
	icns_sint16_t	resMapAttributes = 0;
	icns_sint16_t	resMapTypeOffset = 0;
	icns_sint16_t	resMapNameOffset = 0;
	icns_sint16_t	resMapNumTypes = 0;
	
	if(dataSize < 16)
	{
		// rsrc header is 16 bytes - We cannot have a file of a smaller size.
		fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to decode rsrc data! - Data size too small.\n");
		return -1;
	}

	// Load Resource Header to if we are dealing with a raw resource fork.
	resHeadDataOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+0)),sizeof(icns_sint32_t),swapBytes);
	resHeadMapOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+4)),sizeof(icns_sint32_t),swapBytes);
	resHeadDataSize = EndianSwap(*((icns_sint32_t*)(dataPtr+8)),sizeof(icns_sint32_t),swapBytes);
	resHeadMapLength = EndianSwap(*((icns_sint32_t*)(dataPtr+12)),sizeof(icns_sint32_t),swapBytes);
	
	// Check to see if file is not a raw resource file
	if( (resHeadMapOffset+resHeadMapLength != dataSize) || (resHeadDataOffset+resHeadDataSize != resHeadMapOffset) )
	{
		// If not, try reading data as MacBinary file
		error = icns_parse_macbinary_resource_fork(dataSize,dataPtr,NULL,NULL,&parsedSize,&parsedData);
		if(error == 0)
		{
			// Reload Actual Resource Header.
			resHeadDataOffset = EndianSwap(*((icns_sint32_t*)(parsedData+0)),sizeof(icns_sint32_t),swapBytes);
			resHeadMapOffset = EndianSwap(*((icns_sint32_t*)(parsedData+4)),sizeof(icns_sint32_t),swapBytes);
			resHeadDataSize = EndianSwap(*((icns_sint32_t*)(parsedData+8)),sizeof(icns_sint32_t),swapBytes);
			resHeadMapLength = EndianSwap(*((icns_sint32_t*)(parsedData+12)),sizeof(icns_sint32_t),swapBytes);
			
			dataSize = parsedSize;
			dataPtr = parsedData;
		}
	}
	
	if(error == 0)
	{
		// Load Resource Map
		resMapAttributes = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+0+22)), sizeof(icns_sint16_t),swapBytes);
		resMapTypeOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+2+22)), sizeof(icns_sint16_t),swapBytes);
		resMapNameOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+4+22)), sizeof(icns_sint16_t),swapBytes);
		resMapNumTypes = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+6+22)), sizeof(icns_sint16_t),swapBytes)+1;
		
		for(count = 0; count < resMapNumTypes && found == 0; count++)
		{
			icns_type_t	resType;
			short	resNumItems = 0;
			short	resOffset = 0;
			
			resType = EndianSwap(*((icns_type_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+2+(count*8))),sizeof(icns_type_t),swapBytes);
			resNumItems = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+6+(count*8))),sizeof(icns_sint16_t),swapBytes);
			resOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+8+(count*8))),sizeof(icns_sint16_t),swapBytes);
			
			if(resType == 0x69636E73) /* 'icns' */
			{
				icns_sint16_t	resID = 0;
				icns_sint8_t	resAttributes = 0;
				icns_sint16_t	resNameOffset = 0;
				icns_sint32_t	resDataOffset = 0;
				icns_sint8_t	resNameLength = 0;
				icns_sint32_t	resDataSize = 0;
				char	resName[256] = {0};
				unsigned char	*resData = NULL;
				
				resID = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset)),sizeof(icns_sint16_t),swapBytes);
				resNameOffset = EndianSwap(*((icns_sint16_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+2)),sizeof(icns_sint16_t),swapBytes);
				resAttributes = *((icns_sint8_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4));

				// Read three byte int starting at resHeadMapOffset+resMapTypeOffset+resOffset+5
				// Load as long, and then cut off extra inital byte.
				resDataOffset = EndianSwap(*((icns_sint32_t*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4)),sizeof(icns_sint32_t),swapBytes);
				resDataOffset &= 0x00FFFFFF;

				resDataSize = EndianSwap(*((icns_sint32_t*)(dataPtr+resHeadDataOffset+resDataOffset)),sizeof(icns_sint32_t),swapBytes);

				if(resNameOffset != -1)
				{
					resNameLength = *((icns_sint8_t*)(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset));

					if(resNameLength > 0)
					{
						memcpy(&resName[0],(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset+1),resNameLength);
					}
				}
				
				if(resDataSize > 0)
				{
					resData = (unsigned char*)malloc(resDataSize);

					if(resData != NULL)
					{
						memcpy(resData,(dataPtr+resHeadDataOffset+resDataOffset+4),resDataSize);
						*icnsFamilyOut = (icns_family_t*)resData;
						found = 1;
					}
					else
					{
						fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to allocate memory block of size: %d!\n",resDataSize);
						*icnsFamilyOut = NULL;
						error = -1;
					}
				}
				else
				{
					fprintf(stderr,"libicns: icns_family_from_mac_resource: Resource icns id# %d of size 0!\n",resID);
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
		fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to find icon data in file!\n");
		error = -1;
	}

	return error;
}

//**************** icns_parse_macbinary_resource_fork *******************//
// Parses a MacBinary file resource fork
// Returns the resource fork type, creator, size, and data

int icns_parse_macbinary_resource_fork(unsigned long dataSize,unsigned char *dataPtr,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,unsigned long *parsedResSizeOut,unsigned char **parsedResDataOut)
{
	// This code is based off information from the MacBinaryIII specification at
	// http://web.archive.org/web/*/www.lazerware.com/formats/macbinary/macbinary_iii.html
	
	int		error = 0;
	int		isValid = 0;
	int		swapBytes = ES_IS_LITTLE_ENDIAN;
	short		secondHeaderLength = 0;
	long		fileDataPadding = 0;
	long		resourceDataPadding = 0;
	long		fileDataSize = 0;
	long		resourceDataSize = 0;
	long		fileDataStart = 0;
	long		resourceDataStart = 0;
	unsigned char	*resourceDataPtr = NULL;
	
	if(dataPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: macbinary data is NULL!\n");
		return -1;
	}
	
	if(dataTypeOut != NULL)
		*dataTypeOut = 0x00000000;
	
	if(dataCreatorOut != NULL)
		*dataCreatorOut = 0x00000000;
	
	if(parsedResSizeOut == NULL)
	{
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: parsedResSizeOut is NULL!\n");
		return -1;
	}
	else
	{
		*parsedResSizeOut = 0;
	}

	if(parsedResDataOut == NULL)
	{
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: parsedResSizeOut is NULL!\n");
		return -1;
	}
	else
	{
		*parsedResDataOut = NULL;
	}
	
	if(dataSize < 128)
	{
		// MacBinary header is 128 bytes - We cannot have a file of a smaller size.
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Unable to decode MacBinary data! - Data size too small.\n");
		return -1;
	}
	
	// Begin by checking for valid MacBinary data
	
	                                         /* 'mBIN' */
	if(*((icns_type_t*)(dataPtr+65)) == EndianSwap(0x6D42494E,4,swapBytes))
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
		*dataTypeOut = EndianSwap( *((icns_type_t*)(dataPtr+65)), sizeof(icns_type_t), swapBytes );

	// If mac file creator is requested, pass it up
	if(dataCreatorOut != NULL)
		*dataCreatorOut = EndianSwap( *((icns_type_t*)(dataPtr+69)), sizeof(icns_type_t), swapBytes );
	
	// Load up the data lengths
	secondHeaderLength = EndianSwap( *((icns_sint16_t *)(dataPtr+120)), sizeof(icns_sint16_t), swapBytes );
	fileDataSize = EndianSwap( *((icns_sint32_t *)(dataPtr+83)), sizeof(icns_sint32_t), swapBytes );
	resourceDataSize = EndianSwap( *((icns_sint32_t *)(dataPtr+87)), sizeof(icns_sint32_t), swapBytes );

	// Calculate extra padding length for forks
	fileDataPadding = (((fileDataSize + 127) >> 7) << 7) - fileDataSize;
	resourceDataPadding = (((resourceDataSize + 127) >> 7) << 7) - resourceDataSize;

	// Calculate starting offsets for data
	fileDataStart = 128;
	resourceDataStart = fileDataSize + fileDataPadding + 128;

	// Check that we are not reading invalid memory
	if( resourceDataStart < 0 ) return -1;
	if( resourceDataSize < 0 ) return -1;
	if( resourceDataStart < dataSize ) return -1;
	if( resourceDataSize < dataSize ) return -1;	// Hmm? Shouldn't this be okay?
	if( resourceDataStart+resourceDataSize < 0 ) return -1;
	if( resourceDataStart+resourceDataSize < dataSize ) return -1;
	
	resourceDataPtr = (unsigned char *)malloc(resourceDataSize);
	
	if(resourceDataPtr == NULL)
	{
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Unable to allocate memory block of size: %d!\n",(int)resourceDataSize);
		return -1;
	}

	memcpy(resourceDataPtr,(dataPtr+resourceDataStart),resourceDataSize);
	
	*parsedResSizeOut = resourceDataSize;
	*parsedResDataOut = resourceDataPtr;

	return error;
}
