/*
* File:       iconvert.cpp
* Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
*               2007 Thomas Lübking <thomas.luebking@web.de>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mactypes.h"
#include "macicondefs.h"
#include "iconvert.h"
#include "byteswap.h"

bool ParseXIconResource(long dataSize,char *dataPtr,IconFamilyPtr *iconFamily);
bool ParseMacBinaryResourceFork(long dataSize,char *dataPtr,OSType *dataType, OSType *dataCreator,long *parsedResSize,char **parsedResData);
bool ReadFile(char *fileName,long *dataSize,void **dataPtr);

uint numOffsets = 3;
uint offsets[3] = {342, 670, 2606};

#define IS_ICNS (*((OSTypePtr)(iconDataPtr)) == kIconFamilyType || *((OSTypePtr)(iconDataPtr)) == ByteSwap(kIconFamilyType,sizeof(OSType),true))
#define IS_NOT_ICNS (*((OSTypePtr)(iconDataPtr)) != kIconFamilyType && *((OSTypePtr)(iconDataPtr)) != ByteSwap(kIconFamilyType,sizeof(OSType),true))

/***************************** ReadXIconFile **************************/

bool ReadXIconFile(char *fileName,IconFamilyPtr *iconFamily)
{
	bool	error = false;
	char	*dataPtr = NULL;
	char	*iconDataPtr = NULL;
	long	dataSize = 0;
	long	dataOffset = 0;

	// Read the file into memory
	error = ReadFile(fileName,&dataSize,(void **)&dataPtr);
	iconDataPtr = dataPtr;
	
	// Catch errors...
	if(error == true)
	{
		fprintf(stderr,"Unable to read file %s!\n",fileName);
	}
	else if(dataSize == 0)
	{
		fprintf(stderr,"File size is 0 - no data!\n");
	}
	else
	{
		// search for icns entry, NG icns haf various offsets!
		// IMHO, this is hackish and should be fixed
		// Are there specs for the NG format??
		while (dataOffset < dataSize && IS_NOT_ICNS)
		{
			++dataOffset; ++iconDataPtr;
		}
		
		// Copy the data to a new block of memory since it moved
		iconDataPtr = (char *)malloc(dataSize-dataOffset);
		memcpy (iconDataPtr,dataPtr+dataOffset,dataSize-dataOffset);
		free(dataPtr);
	}
	
	if( IS_NOT_ICNS )
	{
		if((error = ParseXIconResource(dataSize,iconDataPtr,iconFamily)) == true)
		{
			fprintf(stderr,"Error parsing X Icon resource!\n");
			*iconFamily = NULL;
			free(iconDataPtr);
		}
	}
	else
	{
		// Data is an X Icon file - no parsing needed at this point
		*iconFamily = (IconFamilyPtr)iconDataPtr;
	}
	
	return error;
}

/***************************** ParseXIconResource **************************/

bool ParseXIconResource(long dataSize,char *dataPtr,IconFamilyPtr *iconFamily)
{
	bool	error = false;
	bool	found = false;
	int		count = 0;
	
	bool	byteSwapped = BytesSwapped;

	long	parsedSize = 0;
	char	*parsedData = NULL;
	
	long	resHeadDataOffset = 0;
	long	resHeadMapOffset = 0;
	long	resHeadDataLength = 0;
	long	resHeadMapLength = 0;
	
	short	resMapAttributes = 0;
	short	resMapTypeOffset = 0;
	short	resMapNameOffset = 0;
	short	resMapNumTypes = 0;

	// Load Resource Header to if we are dealing with a raw resource fork.
	resHeadDataOffset = ByteSwap(*((long*)(dataPtr+0)),sizeof(long),byteSwapped);
	resHeadMapOffset = ByteSwap(*((long*)(dataPtr+4)),sizeof(long),byteSwapped);
	resHeadDataLength = ByteSwap(*((long*)(dataPtr+8)),sizeof(long),byteSwapped);
	resHeadMapLength = ByteSwap(*((long*)(dataPtr+12)),sizeof(long),byteSwapped);
	
	// Check to see if file is not a raw resource file
	if( (resHeadMapOffset+resHeadMapLength != dataSize) || (resHeadDataOffset+resHeadDataLength != resHeadMapOffset) )
	{
		// If not, try reading data as MacBinary file
		if((error = ParseMacBinaryResourceFork(dataSize,dataPtr,NULL,NULL,&parsedSize,&parsedData)) != true)
		{
			// Reload Actual Resource Header.
			resHeadDataOffset = ByteSwap(*((long*)(parsedData+0)),sizeof(long),byteSwapped);
			resHeadMapOffset = ByteSwap(*((long*)(parsedData+4)),sizeof(long),byteSwapped);
			resHeadDataLength = ByteSwap(*((long*)(parsedData+8)),sizeof(long),byteSwapped);
			resHeadMapLength = ByteSwap(*((long*)(parsedData+12)),sizeof(long),byteSwapped);
			
			dataSize = parsedSize;
			dataPtr = parsedData;
		}
	}
	
	if(error != true)
	{
		// Load Resource Map
		resMapAttributes = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+0+22)), sizeof(short),byteSwapped);
		resMapTypeOffset = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+2+22)), sizeof(short),byteSwapped);
		resMapNameOffset = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+4+22)), sizeof(short),byteSwapped);
		resMapNumTypes = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+6+22)), sizeof(short),byteSwapped)+1;
		
		for(count = 0; count < resMapNumTypes && found == false; count++)
		{
			OSType	resType;
			short	resNumItems = 0;
			short	resOffset = 0;
			
			resType = ByteSwap(*((OSTypePtr)(dataPtr+resHeadMapOffset+resMapTypeOffset+2+(count*8))),sizeof(OSType),byteSwapped);
			resNumItems = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+resMapTypeOffset+6+(count*8))),sizeof(short),byteSwapped);
			resOffset = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+resMapTypeOffset+8+(count*8))),sizeof(short),byteSwapped);
			
			if(resType == 'icns')
			{
				short	resID = 0;
				char	resAttributes = 0;
				short	resNameOffset = 0;
				long	resDataOffset = 0;
				char	resNameLength = 0;
				long	resDataLength = 0;
				char	resName[256] = {0};
				char	*resData = NULL;
				
				resID = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset)),sizeof(short),byteSwapped);
				resNameOffset = ByteSwap(*((short*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+2)),sizeof(short),byteSwapped);
				resAttributes = *((char*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4));

				// Read three byte int starting at resHeadMapOffset+resMapTypeOffset+resOffset+5
				// Load as long, and then cut off extra inital byte.
				resDataOffset = ByteSwap(*((long*)(dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4)),sizeof(long),byteSwapped);
				resDataOffset &= 0x00FFFFFF;

				resDataLength = ByteSwap(*((long*)(dataPtr+resHeadDataOffset+resDataOffset)),sizeof(long),byteSwapped);

				if(resNameOffset != -1)
				{
					resNameLength = *((char*)(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset));

					if(resNameLength > 0)
					{
						memcpy(&resName[0],(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset+1),resNameLength);
					}
				}
				
				if(resDataLength > 0)
				{
					resData = (char*)malloc(resDataLength);

					if(resData != NULL)
					{
						memcpy(resData,(dataPtr+resHeadDataOffset+resDataOffset+4),resDataLength);
						*iconFamily = (IconFamilyPtr)resData;
						found = true;
					}
					else
					{
						fprintf(stderr,"Error allocating %d bytes of memory!\n",resDataLength);
						*iconFamily = NULL;
						error = true;
					}
				}
				else
				{
					fprintf(stderr,"Resource icns id# %d of size 0!\n",resID);
					error = true;
				}
			}
		}
	}
	
	if(parsedData != NULL)
	{
		free(parsedData);
		parsedData = NULL;
	}
	
	if(found == false)
	{
		fprintf(stderr,"Unable to find icon data in file!\n");
		error = true;
	}

	return error;
}

//***************************** ParseMacBinaryResourceFork **************************//
// Parses a MacBinary file - returns type, creator, and resource fork size/data

bool ParseMacBinaryResourceFork(long dataSize,char *dataPtr,OSType *dataType, OSType *dataCreator,long *parsedResSize,char **parsedResData)
{
	// This code is based off information from the MacBinaryIII specification at
	// http://web.archive.org/web/*/www.lazerware.com/formats/macbinary/macbinary_iii.html
	
	bool		error = false;
	bool		isValid = false;
	bool		byteSwapped = BytesSwapped;
	short		secondHeaderLength = 0;
	long		fileDataPadding = 0;
	long		resourceDataPadding = 0;
	long		fileDataLength = 0;
	long		resourceDataLength = 0;
	long		fileDataStart = 0;
	long		resourceDataStart = 0;
	char		*resourceDataPtr = NULL;
	
	*parsedResSize = 0;
	*parsedResData = NULL;
	
	// Begin by checking for valid MacBinary data
	
	if(dataSize < 128)
	{
		// MacBinary header is 128 bytes - We cannot have a file of a smaller size.
		fprintf(stderr,"Unable to decode MacBinary data! - Data size too small.\n");
		return true;
	}
	
	if(*((OSTypePtr)(dataPtr+65)) == ByteSwap('mBIN',4,byteSwapped))
	{
		// Valid MacBinary III file
		isValid = true;
	}
	else
	{
		// Bytes 0 and 74 should both be zero in a valid MacBinary file
		if( ( *(dataPtr) == 0 ) && ( *(dataPtr+74) == 0 ) )
		{
			isValid = true;
		}
		
		// There are other checks beyond this, but in this code,
		// by the time this function is called, chances of getting
		// a match on a non MacBinary file are pretty low...
	}
	
	if( !isValid ) return true;
	
	// Start MacBinary Parsing routines
	
	// If mac file type is requested, pass it up
	if(dataType != NULL)
		*dataType = ByteSwap( *((OSTypePtr)(dataPtr+65)), sizeof(OSType), byteSwapped );

	// If mac file creator is requested, pass it up
	if(dataCreator != NULL)
		*dataCreator = ByteSwap( *((OSTypePtr)(dataPtr+69)), sizeof(OSType), byteSwapped );
	
	// Load up the data lengths
	secondHeaderLength = ByteSwap( *((short *)(dataPtr+120)), sizeof(short), byteSwapped );
	fileDataLength = ByteSwap( *((long *)(dataPtr+83)), sizeof(long), byteSwapped );
	resourceDataLength = ByteSwap( *((long *)(dataPtr+87)), sizeof(long), byteSwapped );

	// Calculate extra padding length for forks
	fileDataPadding = (((fileDataLength + 127) >> 7) << 7) - fileDataLength;
	resourceDataPadding = (((resourceDataLength + 127) >> 7) << 7) - resourceDataLength;

	// Calculate starting offsets for data
	fileDataStart = 128;
	resourceDataStart = fileDataLength + fileDataPadding + 128;

	// Check that we are not reading invalid memory
	if( 0 > resourceDataStart ) return true;
	if( 0 > resourceDataLength ) return true;
	if( dataSize < resourceDataStart ) return true;
	if( dataSize < resourceDataLength ) return true;
	if( 0 > resourceDataStart+resourceDataLength ) return true;
	if( dataSize < resourceDataStart+resourceDataLength ) return true;
	
	resourceDataPtr = (char *)malloc(resourceDataLength);
	
	if(resourceDataPtr != NULL)
	{
		memcpy(resourceDataPtr,(dataPtr+resourceDataStart),resourceDataLength);
		
		*parsedResSize = resourceDataLength;
		*parsedResData = resourceDataPtr;
	}
	else
	{
		fprintf(stderr,"Error allocating memory for parsed resource!\n");
		error = true;
	}

	return error;
}

//***************************** ReadFile **************************//
// Generic file reading routine

bool ReadFile(char *fileName,long *dataSize,void **dataPtr)
{
	bool	error = false;
	FILE	*dataFile = 0;

	*dataSize = 0;
	dataFile = fopen( fileName, "r" );
	
	if ( dataFile != NULL )
	{
		if(fseek(dataFile,0,SEEK_END) == 0)
		{
			*dataSize = ftell(dataFile);
			rewind(dataFile);
			
			*dataPtr = (void *)malloc(*dataSize);

			if ( (error == false) && (*dataPtr != NULL) )
			{
				if(fread( *dataPtr, sizeof(char), *dataSize, dataFile) != *dataSize)
				{
					free( *dataPtr );
					*dataPtr = NULL;
					*dataSize = 0;
					error = true;
					fprintf(stderr,"Error occured reading file!\n");
				}
			}
			else
			{
				error = true;
				fprintf(stderr,"Error occured allocating memory!\n");
			}
		}
		else
		{
			error = true;
			fprintf(stderr,"Error occured seeking to end of file!\n");
		}
		fclose( dataFile );
	}
	else
	{
		error = true;
		fprintf(stderr,"Error occured opening file!\n");
	}

	return error;
}

//***************************** GetIconDataFromIconFamily **************************//
// Parses requested data from an icon family - puts it into a "raw" image format

bool GetIconDataFromIconFamily(IconFamilyPtr inPtr,ResType iconType,IconImagePtr outIcon, bool *byteswap)
{
	UInt32	hOffset = 0;
	bool	error = true;
	bool	foundData = false;
	OSType	testType;
	Size	iconSize = 0;
	int		*dataPtr = NULL;
	
	if(outIcon == NULL)
	{
		fprintf(stderr,"Output pointer icon is NULL!\n");
		error = true;
	}
	else
	{
		if(*((OSTypePtr)(inPtr)) == ByteSwap(kIconFamilyType,sizeof(kIconFamilyType),true))
		{
			*byteswap = true;
			*((OSTypePtr)(inPtr)) = kIconFamilyType;
			inPtr->resourceSize = ByteSwap(inPtr->resourceSize,sizeof(int),true);
		}
		
		/*
		printf("Resource size: %d, %d\n",inPtr->resourceSize,sizeof(IconFamilyResource));
		printf("Resource Types: 0x%8X, 0x%8X\n",*((OSTypePtr)(inPtr)),kIconFamilyType);
		printf("Looking for icon of type: 0x%8X\n",iconType);
		*/
		
		if (( *((OSTypePtr)(inPtr)) != kIconFamilyType) || (inPtr->resourceSize < sizeof(IconFamilyResource)))
		{
			fprintf(stderr,"Error loading icns resource!\n");
		}
		else
		{
			hOffset += sizeof(OSType) + sizeof(Size);
			
			if(*byteswap == true)
			{
				while (hOffset < ByteSwap(inPtr->resourceSize,sizeof(int),true) )
				{
					testType = ByteSwap((OSType)((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementType,sizeof(OSType),true);
					
					if (testType == iconType)
					{
						iconSize = ByteSwap(((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementSize,sizeof(int),true) - sizeof(OSType) - sizeof(Size);
						dataPtr = (int *)((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementData;
						error = ParseIconData(iconType,(char *)dataPtr,iconSize,outIcon,*byteswap);
						foundData = true;
						break;
					}
					hOffset += ByteSwap(((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementSize,sizeof(int),true);
				}
			}
			else
			{
				while (hOffset < inPtr->resourceSize)
				{
					testType = (OSType)((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementType;
					
					if (testType == iconType)
					{
						iconSize = ((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementSize - sizeof(OSType) - sizeof(Size);
						error = ParseIconData(iconType,(char *)((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementData,iconSize,outIcon,*byteswap);
						foundData = true;
						break;
					}
					hOffset += ((IconFamilyElementPtr)(((char*)inPtr)+hOffset))->elementSize;
				}
			}
		}
		
		if(foundData == false)
		{
			fprintf(stderr,"Unable to find requested icon data!\n");
			error = true;
		}
	}
	
	return error;
}

//***************************** ParseIconData **************************//
// Actual conversion of the icon data into uncompressed raw pixels

bool ParseIconData(ResType iconType,Ptr rawDataPtr,long rawDataLength,IconImagePtr outIcon, bool byteSwap)
{
	bool			error = false;
	unsigned int	iconWidth = 0;
	unsigned int	iconHeight = 0;
	unsigned int	iconDepth = 0;
	unsigned long	iconDataSize = 0;

	unsigned long	dataCount = 0;
	unsigned long	blockSize = 0;
	
	if(outIcon == NULL)
	{
		fprintf(stderr,"Icon structure is NULL!\n");
		return false;
	}
	
	switch(iconType)
	{
		// Icon Image Data Types
		case kThumbnail32BitData:
			iconWidth = 128;
			iconHeight = 128;
			iconDepth = 32;
			break;
		case kHuge32BitData:
		case kHuge8BitData:
		case kHuge1BitData:
			iconWidth = 48;
			iconHeight = 48;
			if(iconType == kHuge32BitData)
				iconDepth = 32;
			if(iconType == kHuge8BitData)
				iconDepth = 8;
			if(iconType == kHuge1BitData)
				iconDepth = 1;
			break;
		case kLarge32BitData:
		case kLarge8BitData:
		case kLarge1BitData:
			iconWidth = 32;
			iconHeight = 32;
			if(iconType == kLarge32BitData)
				iconDepth = 32;
			if(iconType == kLarge8BitData)
				iconDepth = 8;
			if(iconType == kLarge1BitData)
				iconDepth = 1;
			break;
		case kSmall32BitData:
		case kSmall8BitData:
		case kSmall1BitData:
			iconWidth = 16;
			iconHeight = 16;
			if(iconType == kSmall32BitData)
				iconDepth = 32;
			if(iconType == kSmall8BitData)
				iconDepth = 8;
			if(iconType == kSmall1BitData)
				iconDepth = 1;
			break;
		case kMini8BitData:
		case kMini1BitData:
			iconWidth = 16;
			iconHeight = 12;
			if(iconType == kMini8BitData)
				iconDepth = 8;
			if(iconType == kMini1BitData)
				iconDepth = 1;
			break;
		// Icon Mask Data Types
		case kThumbnail8BitMask:
			iconWidth = 128;
			iconHeight = 128;
			iconDepth = 8;
			break;
		case kHuge8BitMask:
			iconWidth = 48;
			iconHeight = 48;
			if(iconType == kHuge8BitMask)
				iconDepth = 8;
			break;
		case kLarge8BitMask:
			iconWidth = 32;
			iconHeight = 32;
			if(iconType == kLarge8BitMask)
				iconDepth = 8;
			break;
		case kSmall8BitMask:
			iconWidth = 16;
			iconHeight = 16;
			if(iconType == kSmall8BitMask)
				iconDepth = 8;
			break;
		default:
			return false;
			break;
	}

	/*
	printf("Data count is %d\n",dataCount);
	printf("Data length in bytes is %d\n",rawDataLength);
	printf("Color bit depth: %d\n",iconDepth);
	*/
	
	iconDataSize = (iconWidth*iconHeight*(iconDepth/kByteSize));
	outIcon->width = iconWidth;
	outIcon->height = iconHeight;
	outIcon->depth = iconDepth;
	outIcon->dataSize = iconDataSize;
	outIcon->iconData = (unsigned char *)malloc(iconDataSize);
	memset(outIcon->iconData,0,iconDataSize);

	blockSize = iconWidth *(iconDepth / kByteSize);

	// Data is RLE Encoded
	if((iconDepth == 32) && (rawDataLength < (iconHeight * blockSize)))
	{
		unsigned int	myshift = 0;
		unsigned int	mymask = 0;
		unsigned int	length = 0;
		unsigned int	value = 0;
		long			r = 0;
		long			y = 0;
		long			i = 0;
		long			destIconLength = 0;
		unsigned int	*destIconDataPtr = NULL;		// Decompressed Raw Icon Data
		unsigned int	*destIconDataBasePtr = NULL;	// Decompressed Raw Icon Data Base Address
		
		destIconDataPtr = (unsigned int *)malloc(iconHeight * blockSize);
		destIconDataBasePtr = destIconDataPtr;

		if(destIconDataPtr != NULL)
		{
			destIconLength = (iconWidth * iconHeight);
			
			myshift = 24;
			mymask = 0xFFFFFFFF;
			r = 0;

			// What's this??? In the 128x128 icons,
			// The red channel will be 2 pixels off if we don't start
			// 4 bytes ahead. Perhaps it is a part of some flag?
			if(iconWidth == 128)
				r+=4;

			for(i = 0; i < destIconLength; i++)
				destIconDataPtr[i] = 0x00000000;

			// Alpha?, Red,   Green, Blue
			// 24,     16,    8,     0   
			while(myshift > 0)
			{
				// Next Color Byte
				myshift -= 8;
				
				// Right shift mask 8 bits to prevent overwriting our other colors
				mymask >>= 8;
				
				for(y = 0; y < destIconLength; y = y)
				{
					if( (rawDataPtr[r] & 0x80) == 0)
					{
						// Top bit is clear - run of various values to follow
						length = (int)(0xFF & rawDataPtr[r++]) + 1; // 1 <= len <= 128
						
						for(i = 0; i < (int)length; i++)
							destIconDataPtr[y++] |= ( ((int)rawDataPtr[r++]) << myshift) & mymask;
					}
					else
					{
						// Top bit is set - run of one value to follow
						length = (int)(0xFF & rawDataPtr[r++]) - 125; // 3 <= len <= 130
						
						// Set the value to the color shifted to the correct bit offset
						value = ( ((int)rawDataPtr[r++]) << myshift) & mymask;

						for(i = 0; i < (int)length; i++)
							destIconDataPtr[y++] |= value;
					}
				}
			}

			if(byteSwap == true)
				for(dataCount = 0; dataCount < iconDataSize; dataCount+=(iconDepth / kByteSize))
					ByteSwapAddr(((char *)destIconDataPtr)+dataCount,(iconDepth / kByteSize),true);
					
			for(dataCount = 0; dataCount < iconHeight; dataCount++)
				memcpy(&(((char*)(outIcon->iconData))[dataCount*blockSize]),&(((char*)(destIconDataPtr))[dataCount*blockSize]),blockSize);
			
			free(destIconDataBasePtr);
			destIconDataBasePtr = NULL;
			destIconDataPtr = NULL;
		}
		else
		{
			error = true;
		}
	}
	else // No RLE Compression
	{
		if(byteSwap == true)
			for(dataCount = 0; dataCount < rawDataLength; dataCount+=(iconDepth / kByteSize))
				ByteSwapAddr(rawDataPtr+dataCount,(iconDepth / kByteSize),true);
		
		for(dataCount = 0; dataCount < iconHeight; dataCount++)
			memcpy(&(((char*)(outIcon->iconData))[dataCount*blockSize]),&(((char*)(rawDataPtr))[dataCount*blockSize]),blockSize);
	}
	
	return error;
}
