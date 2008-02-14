/*
File:       icns2png.c
Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
Copyright (C) 2002 Chenxiao Zhao <chenxiao.zhao@gmail.com>

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

#include "icns.h"
#include "pngwriter.h"
#include "jp2dec.h"

void parse_format(char *format);
void parse_options(int argc, char** argv);
int ConvertIcnsFile(char *filename);
int convertIcon128ToPNG(ICNS_IconData icon, ICNS_IconData maskIcon, int byteSwap, char *filename);
int convertIcon512ToPNG(ICNS_IconData icon, char *filename);
int ReadFile(char *fileName,long *dataSize,void **dataPtr);

#define	ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define	MAX_INPUTFILES  4096

#define	kNoMaskData	0x00000000

char 	*inputfiles[MAX_INPUTFILES];
int	fileindex = 0;

/* default iconType to be extracted */
int	iconType = kThumbnail32BitData;
int	maskType = kThumbnail8BitMask;


void parse_format(char *format)
{
	const char *formats[] = {
				 "ICON512", 
				 "ICON256", 
				 "ICON128"
				};
	const int iconTypes[] = {  
				kIconServices512PixelDataARGB, 
				kIconServices256PixelDataARGB, 
				kThumbnail32BitData
			     };
	const int maskTypes[] = {  
				kNoMaskData, 
				kNoMaskData, 
				kThumbnail8BitMask
			     };
	int i;
	for(i = 0; i < ARRAY_SIZE(formats); i++) {
		if(strcmp(formats[i], format) == 0) {
			iconType = iconTypes[i];
			maskType = maskTypes[i];
			break;
		}
	}
}

void parse_options(int argc, char** argv)
{
	int opt;

	while(1) {
		opt = getopt(argc, argv, "-t:");
		if(opt < 0)
			break;
		switch(opt) {
		case 't':
			parse_format(optarg);
			break;
		case 1:
			if(fileindex >= MAX_INPUTFILES) {
				fprintf(stderr, "No more file can be added\n");
				break;
			}
			inputfiles[fileindex] = malloc(strlen(optarg)+1);
			if(!inputfiles[fileindex]) {
				printf("Out of Memory\n");
				exit(1);
			}
			strcpy(inputfiles[fileindex], optarg);
			fileindex++;
			break;
		default:
			exit(1);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int count;

	if(argc < 2)
	{
		printf("Usage: icns2png input.icns\n");
		return -1;
	}
	
	parse_options(argc, argv);

	for(count = 0; count < fileindex; count++)
	{
		if(ConvertIcnsFile(inputfiles[count]))
			fprintf(stderr, "Conversion of %s failed!\n",argv[count]);
	}
	
	return 0;
}

int ConvertIcnsFile(char *filename)
{
	int		error = 0;
	char		*infilename;
	char		*outfilename;
	int		filenamelength = 0;
	int		infilenamelength = 0;
	int		outfilenamelength = 0;
	char		*fileDataPtr = NULL;
	long		fileDataSize = 0;
	int		byteSwap = 0;
	IconFamilyPtr	iconFamily = NULL;
	ICNS_IconData	icon; icon.data = NULL;
	ICNS_IconData	mask; mask.data = NULL;
	ICNS_ImageData	iconImage;
	ICNS_ImageData	maskImage;
	FILE 		*outfile = NULL;

	filenamelength = strlen(filename);

	infilenamelength = filenamelength;
	outfilenamelength = filenamelength;

	// Create a buffer for the input filename
	infilename = (char *)malloc(infilenamelength+1);

	// Copy the input filename into the new buffer
	strncpy(infilename,filename,infilenamelength+1);
	infilename[filenamelength] = 0;

	// See if we can find a '.'
	while(infilename[outfilenamelength] != '.' && outfilenamelength > 0)
		outfilenamelength--;

	// Caculate new filename length
	if(outfilenamelength == 0)
		outfilenamelength = strlen(filename);
	outfilenamelength += 4;

	// Create a buffer for the output filename
	outfilename = (char *)malloc(outfilenamelength+1);
	if(infilenamelength < outfilenamelength+1) {
		strncpy(outfilename,filename,infilenamelength);
	} else {
		strncpy(outfilename,filename,outfilenamelength+1);
	}

	// Add the .png extension to the filename
	outfilename[outfilenamelength-4] = '.';
	outfilename[outfilenamelength-3] = 'p';
	outfilename[outfilenamelength-2] = 'n';
	outfilename[outfilenamelength-1] = 'g';
	outfilename[outfilenamelength-0] = 0;

	printf("Converting %s...\n",infilename);
	error = ReadFile(infilename,&fileDataSize,(void **)&fileDataPtr);
	
	// Catch errors...
	if(error)
	{
		fprintf(stderr,"Unable to read file %s!\n",infilename);
		free(infilename);
		free(outfilename);
		if(fileDataPtr != NULL)
			free(fileDataPtr);
		return 1;
	}
	
	// ReadXIconFile converts the icns file into an IconFamily
	error = GetIconFamilyFromFileData(fileDataSize,fileDataPtr,&iconFamily);
	
	if(error) {
		fprintf(stderr,"Unable to load icns file into icon family!\n");
		goto cleanup;
	}
	
	error = GetIconDataFromIconFamily(iconFamily,iconType,&icon,&byteSwap);
	
	if(error) {
		fprintf(stderr,"Unable to load icon data from icon family!\n");
		goto cleanup;
	}

	error = ParseIconData(iconType, (Ptr)icon.data, icon.size, &iconImage, byteSwap);

	if(error) {
		fprintf(stderr,"Unable to load raw data from icon data!\n");
		goto cleanup;
	}
	
	if(maskType != kNoMaskData) {
		error = GetIconDataFromIconFamily(iconFamily,maskType,&mask,&byteSwap);
	
		if(error) {
			fprintf(stderr,"Unable to load icon data from icon family!\n");
			goto cleanup;
		}
		
		error = ParseIconData(maskType, (Ptr)mask.data, mask.size, &maskImage, byteSwap);
	}
	
	outfile = fopen(outfilename,"w");
	if(!outfile) {
		fprintf(stderr,"Unable to open %s for writing!\n",outfilename);
		goto cleanup;
	}
	
	// Finally! We save the image
	if(maskType != kNoMaskData) {
		error = WritePNGImage(outfile,&iconImage,&maskImage);	
	} else {
		error = WritePNGImage(outfile,&iconImage,NULL);	
	}
	
	if(error) {
		fprintf(stderr,"Error writing PNG image!\n");
	} else {
		printf("Saved to %s.\n",outfilename);
	}
	
	// Cleanup
cleanup:
	if(outfile != NULL) {
		fclose(outfile);
		outfile = NULL;
	}
	if(iconFamily != NULL) {
		free(iconFamily);
		iconFamily = NULL;
	}
	if(icon.data != NULL) {
		free(icon.data);
		icon.data = NULL;
	}
	if(mask.data != NULL) {
		free(mask.data);
		mask.data = NULL;
	}
	free(infilename);
	free(outfilename);
	
	return error;
}

int convertIcon128ToPNG(ICNS_IconData icon, ICNS_IconData maskIcon, int byteSwap, char *filename)
{
	ICNS_ImageData iconImage;
	ICNS_ImageData maskImage;
	int err = 0;
	FILE *outfile = NULL;

	if(!icon.data || !icon.data || !filename)
		return 0;

	iconImage.iconData = NULL;
	maskImage.iconData = NULL;

	err = ParseIconData(kThumbnail32BitData, (Ptr)icon.data, icon.size, &iconImage, byteSwap);
	if(err) goto out;

	err = ParseIconData(kThumbnail8BitMask, (Ptr)maskIcon.data, maskIcon.size, &maskImage, byteSwap);
	if(err) goto out;

	if(!iconImage.iconData) {
		err = 1;
		goto out;
	}

	outfile = fopen(filename,"w");
	if(!outfile) goto out;

	err = WritePNGImage(outfile,&iconImage,&maskImage);
out:
	if(outfile)
		fclose(outfile);
	if(iconImage.iconData)
		free(iconImage.iconData);
	if(maskImage.iconData)
		free(maskImage.iconData);
	return err;
}

int convertIcon512ToPNG(ICNS_IconData icon, char *filename)
{
	ICNS_ImageData iconImage;
	FILE *outfile = NULL;
	int err;

	if(!icon.data || !filename)
		goto error;

	err = ParseIconData(kIconServices512PixelDataARGB, (Ptr)icon.data, icon.size, &iconImage, false);
	if(err) goto out;

	outfile = fopen(filename,"w");
	if(!outfile) goto out;
	
	if(!err)
		err = WritePNGImage(outfile,&iconImage,NULL);
	
out:
	if(outfile)
		fclose(outfile);
	if(iconImage.iconData != NULL)
		free(iconImage.iconData);
	
	return err;
error:
	return true;
}

//***************************** ReadFile **************************//
// Generic file reading routine

int ReadFile(char *fileName,long *dataSize,void **dataPtr)
{
	int	error = 0;
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

			if ( (error == 0) && (*dataPtr != NULL) )
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
