/*
File:       icnsinfo.c
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
#include <string.h>
#include <png.h>

#include "icns.h"
#include "icns_internals.h"

int DesribeIconFamilyFile(char *filename);

#define	ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define	MAX_INPUTFILES  4096

char 	*inputfiles[MAX_INPUTFILES];
int	fileindex = 0;

int	showDetail = 0;

int parse_options(int argc, char** argv)
{
	int opt;

	while(1) {
		opt = getopt(argc, argv, "dh:");
		if(opt < 0)
			break;
		switch(opt)
		{
			case 'd':
				showDetail = 1;
				break;
			case 'h':
				return -1;
				break;
			default:
				fprintf(stderr, "Invalid options specified. %c (%d)\n",(char)opt,opt);
				return -1;
				break;
		}
	}

	argc -= optind;
	argv += optind;
	
	while (argc) {
		if(fileindex >= MAX_INPUTFILES) {
			fprintf(stderr, "No more file can be added\n");
			break;
		}
		inputfiles[fileindex] = malloc(strlen(argv[0])+1);
		if(!inputfiles[fileindex]) {
			printf("Out of Memory\n");
			exit(1);
		}
		strcpy(inputfiles[fileindex], argv[0]);
		fileindex++;
		argc--;
		argv++;
	}
	
	return 0;
}


int main(int argc, char *argv[])
{
	int count;
	
	if(argc < 2)
	{
		printf("Usage: icns2png [options] [file]\n");
		return -1;
	}
	
	if(parse_options(argc, argv) != 0)
	{
		printf("Usage: icnsinfo           [file]                                              \n");
		printf("icnsinfo lists information about an icns file.                                \n");
		printf("                                                                              \n");
		printf("Example:                                                                      \n");
		printf("icnsinfo anicon.icns            # List information about anicon.icns          \n");
		printf("icnsinfo -d myicon.icns         # List detailed information about mycon.icns  \n");
		printf("                                                                              \n");
		printf("Options:                                                                      \n");
		printf(" -d            Shows more in-depth information about the icns file            \n");
		printf(" -h            Shows this help message.                                       \n");
		return 0;
	}
	
	icns_set_print_errors(1);
	
	for(count = 0; count < fileindex; count++)
	{
		if(DesribeIconFamilyFile(inputfiles[count]))
			fprintf(stderr, "Describing %s failed!\n",inputfiles[count]);
	}
	
	for(count = 0; count < fileindex; count++)
		if(inputfiles[count] != NULL)
			free(inputfiles[count]);

	return 0;
}

int DesribeIconFamilyFile(char *filename)
{
	int		error = ICNS_STATUS_OK;
	FILE            *inFile = NULL;
	icns_family_t	*iconFamily = NULL;
	icns_byte_t	*dataPtr = NULL;
	char		typeStr[5] = {0,0,0,0,0};
	unsigned long	dataOffset = 0;
	int		elementCount = 0;

	inFile = fopen( filename, "r" );
	
	if ( inFile == NULL ) {
		fprintf (stderr, "Unable to open file %s!\n",filename);
		goto cleanup;
	}
	
	printf("----------------------------------------------------\n");
	printf("Reading icns family from %s...\n",filename);
	
	error = icns_read_family_from_file(inFile,&iconFamily);
		
	fclose(inFile);
		
	if(error) {
		fprintf (stderr, "Unable to read icns family from file %s!\n",filename);
		goto cleanup;
	}
	
	// Start listing info:
	memcpy(&typeStr,&(iconFamily->resourceType),4);
	
	printf(" Icon family type is '%s'\n",typeStr);
	printf(" Icon family size is %d bytes\n",iconFamily->resourceSize);
	
	// Skip past the icns header
	dataOffset = sizeof(icns_type_t) + sizeof(icns_size_t);
	dataPtr = (icns_byte_t *)iconFamily;
	
	printf(" Listing icon elements...\n");
	
	// Iterate through the icns resource, converting the 'size' values to native endian
	while(((dataOffset+8) < iconFamily->resourceSize) && (error == 0))
	{
		icns_element_t	       iconElement;
		icns_icon_image_info_t iconInfo;
		
		memcpy(&iconElement,(dataPtr+dataOffset),8);
		memcpy(&typeStr,&(iconElement.elementType),4);
		
		iconInfo = icns_get_image_info_for_type(iconElement.elementType);
		
		printf("  found element type '%s' data size: %d ",typeStr,iconElement.elementSize-8);
		if((iconElement.elementSize-8) == iconInfo.iconRawDataSize)
			printf("(uncompressed)");
		if((iconElement.elementSize-8) < iconInfo.iconRawDataSize)
			printf("(compressed)");
		if((iconElement.elementSize-8) == (iconInfo.iconRawDataSize*2))
			printf("(with mask)");
		printf("\n");
		
		if(showDetail == 1) {
			printf("   %d-bit icon, size (%dx%d), raw data bytes: %d\n",iconInfo.iconBitDepth,iconInfo.iconWidth,iconInfo.iconHeight,(int)iconInfo.iconRawDataSize);
		}
		// Move on to the next element
		dataOffset += iconElement.elementSize;
		elementCount++;
	}
	
	if(elementCount > 0) {
		printf(" %d elements total in %s.\n",elementCount,filename);
	} else {
		printf(" No elements found in %s.\n",filename);
	}
	
cleanup:
	
	if(iconFamily != NULL) {
		free(iconFamily);
		iconFamily = NULL;
	}
	
	return error;
}
