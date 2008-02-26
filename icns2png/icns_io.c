/*
File:       icns_io.c
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

/***************************** icns_write_family_to_file **************************/

int icns_write_family_to_file(FILE *dataFile,icns_family_t *iconFamilyIn)
{
	icns_size_t	resourceSize = 0;
	icns_size_t	blockSize = 0;
	icns_uint32_t	dataOutTotal = 0;
	icns_uint32_t	dataOutBlock = 0;
	
	if ( dataFile == NULL )
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: NULL file pointer!\n");
		return -1;
	}	
	
	if ( iconFamilyIn == NULL )
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: NULL icns family!\n");
		return -1;
	}
	
	ICNS_READ_UNALIGNED(resourceSize, &(iconFamilyIn->resourceSize), icns_size_t);
	resourceSize = EndianSwapBtoN(resourceSize,sizeof(icns_size_t));
	blockSize = 1024;
	
	while( (resourceSize-dataOutTotal) > blockSize)
	{
		dataOutBlock = fwrite ( ((char *)iconFamilyIn) + dataOutTotal , blockSize , 1 , dataFile );
		if(dataOutBlock != blockSize)
		{
			fprintf(stderr,"libicns: icns_write_family_to_file: Error writing icns to file!\n");
			return -1;
		}
		dataOutTotal += dataOutBlock;
	}
	
	blockSize = (resourceSize-dataOutTotal);
	
	dataOutBlock = fwrite ( ((char *)iconFamilyIn) + dataOutTotal , blockSize , 1 , dataFile );
	if(dataOutBlock != blockSize)
	{
		fprintf(stderr,"libicns: icns_write_family_to_file: Error writing icns to file!\n");
		return -1;
	}
	dataOutTotal += dataOutBlock;
	
	return 0;
}


/***************************** icns_read_family_from_file **************************/

int icns_read_family_from_file(FILE *dataFile,icns_family_t **iconFamilyOut)
{
	int	      error = 0;
	unsigned long dataSize = 0;
	void          *dataPtr = NULL;
	
	if ( dataFile == NULL )
	{
		fprintf(stderr,"libicns: icns_read_family_from_file: NULL file pointer!\n");
		return -1;
	}
	
	if ( iconFamilyOut == NULL )
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
		error = icns_family_from_file_data(dataSize,dataPtr,iconFamilyOut);
	
	if(dataPtr != NULL)
	{
		free(dataPtr);
		dataPtr = NULL;
	}
	
	return error;
}


/***************************** icns_family_from_file_data **************************/

int icns_family_from_file_data(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **iconFamilyOut)
{
	int		error = 0;
	icns_size_t	resourceSize = 0;
	unsigned char	*iconDataPtr = NULL;
	unsigned long	dataOffset = 0;
	
	if(dataSize == 0)
	{
		fprintf(stderr,"libicns: icns_family_from_file_data: File size is 0 - no data!\n");
		return -1;
	}
	
	if(iconFamilyOut == NULL)
	{
		fprintf(stderr,"libicns: icns_family_from_file_data: iconFamilyOut is NULL!\n");
		return -1;
	}
	
	// search for icns entry, NG icns haf various offsets!
	// Note by Mathew 02/18/2008
	// Baghira hack disabled for security reasons
	// Are there not specs for the NG format??
	//iconDataPtr = dataPtr;
	//while ( (dataOffset < dataSize-sizeof(icns_type_t)) && (*((icns_type_t*)(iconDataPtr)) != EndianSwapBtoN(ICNS_FAMILY_TYPE,sizeof(icns_type_t))) ) {
	//	++dataOffset;
	//	++iconDataPtr;
	//}
	
	// Copy the data to a new block of memory
	if((dataSize-dataOffset) > 0) {
		iconDataPtr = (unsigned char *)malloc(dataSize-dataOffset);
		memcpy (iconDataPtr,dataPtr+dataOffset,dataSize-dataOffset);
		dataSize = dataSize-dataOffset;
	} else {
		iconDataPtr = (unsigned char *)malloc(dataSize);
		memcpy (iconDataPtr,dataPtr,dataSize);
	}
	
	if(*((icns_type_t*)(iconDataPtr)) != EndianSwapBtoN(ICNS_FAMILY_TYPE,sizeof(icns_type_t)))
	{
		// Might be embedded in an rsrc file
		if((error = icns_family_from_mac_resource(dataSize,iconDataPtr,iconFamilyOut)))
		{
			fprintf(stderr,"libicns: icns_family_from_file_data: Error parsing X Icon resource!\n");
			*iconFamilyOut = NULL;	
		}
		
		// Because icns_family_from_mac_resource allocates it's own memory block
		free(iconDataPtr);
	}
	else
	{
		// Data is an X Icon file - no parsing needed at this point
		*iconFamilyOut = (icns_family_t*)iconDataPtr;
		ICNS_READ_UNALIGNED(resourceSize, &((*iconFamilyOut)->resourceSize), icns_size_t);
		if( dataSize != EndianSwapBtoN( resourceSize, sizeof(icns_size_t)) )
		{
			fprintf(stderr,"libicns: icns_family_from_file_data: Invalid icns resource size!\n");
			return -1;
		}

	}

	return error;
}

/***************************** icns_family_from_mac_resourceFork **************************/

int icns_family_from_mac_resource(unsigned long dataSize,unsigned char *dataPtr,icns_family_t **iconFamilyOut)
{
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
	ICNS_READ_UNALIGNED(resHeadDataOffset, (dataPtr+0), icns_sint32_t);
	ICNS_READ_UNALIGNED(resHeadMapOffset, (dataPtr+4), icns_sint32_t);
	ICNS_READ_UNALIGNED(resHeadDataSize, (dataPtr+8), icns_sint32_t);
	ICNS_READ_UNALIGNED(resHeadMapLength, (dataPtr+12), icns_sint32_t);
	resHeadDataOffset = EndianSwapBtoN(resHeadDataOffset,sizeof(icns_sint32_t));
	resHeadMapOffset = EndianSwapBtoN(resHeadMapOffset,sizeof(icns_sint32_t));
	resHeadDataSize = EndianSwapBtoN(resHeadDataSize,sizeof(icns_sint32_t));
	resHeadMapLength = EndianSwapBtoN(resHeadMapLength,sizeof(icns_sint32_t));
	
	// Check to see if file is not a raw resource file
	if( (resHeadMapOffset+resHeadMapLength != dataSize) || (resHeadDataOffset+resHeadDataSize != resHeadMapOffset) )
	{
		// If not, try reading data as MacBinary file
		error = icns_parse_macbinary_resource_fork(dataSize,dataPtr,NULL,NULL,&parsedSize,&parsedData);

		if(error != 0)
		{
			fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to to decode macbinary data.\n");
			return -1;
					
		}
		
		// Reload Actual Resource Header.
		ICNS_READ_UNALIGNED(resHeadDataOffset, (parsedData+0), icns_sint32_t);
		ICNS_READ_UNALIGNED(resHeadMapOffset, (parsedData+4), icns_sint32_t);
		ICNS_READ_UNALIGNED(resHeadDataSize, (parsedData+8), icns_sint32_t);
		ICNS_READ_UNALIGNED(resHeadMapLength, (parsedData+12), icns_sint32_t);
		resHeadDataOffset = EndianSwapBtoN(resHeadDataOffset,sizeof(icns_sint32_t));
		resHeadMapOffset = EndianSwapBtoN(resHeadMapOffset,sizeof(icns_sint32_t));
		resHeadDataSize = EndianSwapBtoN(resHeadDataSize,sizeof(icns_sint32_t));
		resHeadMapLength = EndianSwapBtoN(resHeadMapLength,sizeof(icns_sint32_t));
				
		dataSize = parsedSize;
		dataPtr = parsedData;

		if( (resHeadMapOffset+resHeadMapLength != dataSize) || (resHeadDataOffset+resHeadDataSize != resHeadMapOffset) )
		{
			fprintf(stderr,"libicns: icns_family_from_mac_resource: Invalid macbinary resource data.\n");
			goto cleanup;
		}
	}

	if(resHeadMapOffset+28 > dataSize)
	{
		fprintf(stderr,"libicns: icns_family_from_mac_resource: Invalid resource header.\n");
		error = -1;
		goto cleanup;
	}
	
	if(error == 0)
	{
		// Load Resource Map
		ICNS_READ_UNALIGNED(resMapAttributes, (dataPtr+resHeadMapOffset+0+22), icns_sint16_t);
		ICNS_READ_UNALIGNED(resMapTypeOffset, (dataPtr+resHeadMapOffset+2+22), icns_sint16_t);
		ICNS_READ_UNALIGNED(resMapNameOffset, (dataPtr+resHeadMapOffset+4+22), icns_sint16_t);
		ICNS_READ_UNALIGNED(resMapNumTypes, (dataPtr+resHeadMapOffset+6+22), icns_sint16_t);
		resMapAttributes = EndianSwapBtoN(resMapAttributes, sizeof(icns_sint16_t));
		resMapTypeOffset = EndianSwapBtoN(resMapTypeOffset, sizeof(icns_sint16_t));
		resMapNameOffset = EndianSwapBtoN(resMapNameOffset, sizeof(icns_sint16_t));
		resMapNumTypes = EndianSwapBtoN(resMapNumTypes, sizeof(icns_sint16_t));
		// 0 == 1 here, so fix that
		resMapNumTypes = resMapNumTypes + 1;

		if( (resHeadMapOffset+resMapTypeOffset+2+(count*8)) > dataSize)
		{
			fprintf(stderr,"libicns: icns_family_from_mac_resource: Invalid resource map.\n");
			error = -1;
			goto cleanup;
		}
		
		for(count = 0; count < resMapNumTypes && found == 0; count++)
		{
			icns_type_t	resType;
			short		resNumItems = 0;
			short		resOffset = 0;
			
			ICNS_READ_UNALIGNED(resType, (dataPtr+resHeadMapOffset+resMapTypeOffset+2+(count*8)), icns_type_t);
			ICNS_READ_UNALIGNED(resNumItems, (dataPtr+resHeadMapOffset+resMapTypeOffset+6+(count*8)), icns_sint16_t);
			ICNS_READ_UNALIGNED(resOffset, (dataPtr+resHeadMapOffset+resMapTypeOffset+8+(count*8)), icns_sint16_t);
			resType = EndianSwapBtoN(resType,sizeof(icns_type_t));
			resNumItems = EndianSwapBtoN(resNumItems,sizeof(icns_sint16_t));
			resOffset = EndianSwapBtoN(resOffset,sizeof(icns_sint16_t));
			
			if(resType == ICNS_FAMILY_TYPE)
			{
				icns_sint16_t	resID = 0;
				icns_sint8_t	resAttributes = 0;
				icns_sint16_t	resNameOffset = 0;
				icns_sint32_t	resDataOffset = 0;
				icns_sint8_t	resNameLength = 0;
				icns_sint32_t	resDataSize = 0;
				char	resName[256] = {0};
				unsigned char	*resData = NULL;
				
				ICNS_READ_UNALIGNED(resID, (dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset), icns_sint16_t);
				ICNS_READ_UNALIGNED(resNameOffset, (dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+2), icns_sint16_t);
				ICNS_READ_UNALIGNED(resAttributes, (dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4), icns_sint8_t);
				
				resID = EndianSwapBtoN(resID,sizeof(icns_sint16_t));
				resNameOffset = EndianSwapBtoN(resNameOffset,sizeof(icns_sint16_t));
				resAttributes = EndianSwapBtoN(resAttributes,sizeof(icns_sint8_t));
								
				// Read in the resource name, if it exists (-1 indicates it doesn't)
				if(resNameOffset != -1)
				{
					ICNS_READ_UNALIGNED(resNameLength, (dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset), icns_sint8_t);

					if(resNameLength > 0)
					{
						memcpy(&resName[0],(dataPtr+resHeadMapOffset+resMapNameOffset+resNameOffset+1),resNameLength);
					}
				}

				// Read three byte int starting at resHeadMapOffset+resMapTypeOffset+resOffset+5
				// Load as long, and then cut off extra inital byte.
				ICNS_READ_UNALIGNED(resDataOffset, (dataPtr+resHeadMapOffset+resMapTypeOffset+resOffset+4), icns_sint32_t);
				resDataOffset = EndianSwapBtoN(resDataOffset,sizeof(icns_sint32_t));
				resDataOffset &= 0x00FFFFFF;
				
				ICNS_READ_UNALIGNED(resDataSize, (dataPtr+resHeadDataOffset+resDataOffset), icns_sint32_t);
				resDataSize = EndianSwapBtoN(resDataSize,sizeof(icns_sint32_t));

				if( (resHeadDataOffset+resDataOffset) > dataSize )
				{
					fprintf(stderr,"libicns: icns_family_from_mac_resource: Resource icns id# %d has invalid data offset!\n",resID);
					error = -1;
					goto cleanup;
				}
				
				
					
				if( (resDataSize <= 0) || (resDataSize > (dataSize-(resHeadDataOffset+resDataOffset+4))) )
				{
					fprintf(stderr,"libicns: icns_family_from_mac_resource: Resource icns id# %d has invalid size %ld!\n",resID,dataSize);
					error = -1;
					goto cleanup;
				}
				
				resData = (unsigned char*)malloc(resDataSize);
				
				if(resData != NULL)
				{
					icns_type_t	resourceType = 0x00000000;
					icns_size_t	resourceSize = 0;

					memcpy( resData ,(dataPtr+resHeadDataOffset+resDataOffset+4),resDataSize);
					*iconFamilyOut = (icns_family_t*)resData;
					// Check the data... this needs to be accurate, but we might be able to repair it for now
					resourceType = (*iconFamilyOut)->resourceType;
					resourceSize = (*iconFamilyOut)->resourceSize;
					if(ICNS_FAMILY_TYPE != EndianSwapBtoN( resourceType ,sizeof(icns_type_t)))
					{
						fprintf(stderr,"libicns: icns_family_from_mac_resource: warning: family type is incorrect - attempting repair!\n");
						resourceType = EndianSwapNtoB( ICNS_FAMILY_TYPE ,sizeof(icns_type_t));
						ICNS_WRITE_UNALIGNED(&((*iconFamilyOut)->resourceType), resourceType, sizeof(icns_type_t));
					}
					if(resDataSize != EndianSwapBtoN( resourceSize ,sizeof(icns_size_t)))
					{
						fprintf(stderr,"libicns: icns_family_from_mac_resource: warning: family size is incorrect - attempting repair!\n");
						resourceSize = EndianSwapNtoB( resDataSize ,sizeof(icns_size_t));
						ICNS_WRITE_UNALIGNED(&((*iconFamilyOut)->resourceSize), resourceSize, sizeof(icns_size_t));
					}
					found = 1;
				}
				else
				{
					fprintf(stderr,"libicns: icns_family_from_mac_resource: Unable to allocate memory block of size: %d!\n",resDataSize);
					*iconFamilyOut = NULL;
					error = -1;
				}
			}
		}
	}
	
cleanup:
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
	icns_type_t	dataType = 0x00000000;
	icns_type_t	dataCreator = 0x00000000;
	int		isValid = 0;
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
	
	// Read headers
	ICNS_READ_UNALIGNED(dataType, (dataPtr+65), icns_type_t);
	ICNS_READ_UNALIGNED(dataCreator, (dataPtr+69), icns_type_t);

	// Checking for valid MacBinary data...
                                  /* 'mBIN' */
	if(dataType == EndianSwapBtoN(0x6D42494E,4))
	{
		// Valid MacBinary III file
		isValid = 1;
	}
	else
	{
		char	byte00 = 0;
		char	byte74 = 0;
		
		ICNS_READ_UNALIGNED(byte00, (dataPtr+0), char);
		ICNS_READ_UNALIGNED(byte74, (dataPtr+74), char);
		
		// Bytes 0 and 74 should both be zero in a valid MacBinary file
		if( ( byte00 == 0 ) && ( byte74 == 0 ) )
		{
			isValid = 1;
		}
		
		// There are other checks beyond this, but in this code,
		// by the time this function is called, chances of getting
		// a match on a non MacBinary file are pretty low...
	}
	
	if( !isValid )
	{
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Invalid MacBinary file!\n");
		return -1;
	}
	
	// Start MacBinary Parsing routines
	
	// If mac file type is requested, pass it up
	if(dataTypeOut != NULL)
	{
		*dataTypeOut = EndianSwapBtoN( dataType, sizeof(icns_type_t) );
	}

	// If mac file creator is requested, pass it up
	if(dataCreatorOut != NULL)
	{
		*dataCreatorOut = EndianSwapBtoN( dataCreator, sizeof(icns_type_t) );
	}

	// Load up the data lengths
	ICNS_READ_UNALIGNED(secondHeaderLength, (dataPtr+120), icns_sint16_t);
	ICNS_READ_UNALIGNED(fileDataSize, (dataPtr+83), icns_sint32_t);
	ICNS_READ_UNALIGNED(resourceDataSize, (dataPtr+87), icns_sint32_t);
	secondHeaderLength = EndianSwapBtoN( secondHeaderLength, sizeof(icns_sint16_t) );
	fileDataSize = EndianSwapBtoN( fileDataSize, sizeof(icns_sint32_t) );
	resourceDataSize = EndianSwapBtoN( resourceDataSize, sizeof(icns_sint32_t) );

	// Calculate extra padding length for forks
	fileDataPadding = (((fileDataSize + 127) >> 7) << 7) - fileDataSize;
	resourceDataPadding = (((resourceDataSize + 127) >> 7) << 7) - resourceDataSize;

	// Calculate starting offsets for data
	fileDataStart = 128;
	resourceDataStart = fileDataSize + fileDataPadding + 128;

	// Check that we are not reading invalid memory
	if( (resourceDataStart < 128) || (resourceDataStart > dataSize) ) {
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Invalid resource data start!\n");
	       	return -1;
	}
	if( (resourceDataSize < 16) || (resourceDataSize > (dataSize-128))  ) {
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Invalid resource data size!\n");
		return -1;
	}
	if( (resourceDataStart+resourceDataSize > dataSize) ) {
		fprintf(stderr,"libicns: icns_parse_macbinary_resource_fork: Invalid resource data location!\n");
		return -1;
	}

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
