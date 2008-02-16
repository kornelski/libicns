/*
File:       icns2png.c
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
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
#include <png.h>

#include "icns.h"

int ConvertIcnsFile(char *filename);
int ReadFile(char *fileName,unsigned long *dataSize,void **dataPtr);
int WritePNGImage(FILE *outputfile,icns_image_t *image,icns_image_t *mask);

#define	ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define	MAX_INPUTFILES  4096

#define	kNoMaskData	0x00000000
#define	kNoIconData	0x00000000

typedef struct pixel32_struct
{
	unsigned char	 alpha;
	unsigned char	 red;
	unsigned char	 green;
	unsigned char	 blue;
} pixel32;

char 	*inputfiles[MAX_INPUTFILES];
int	fileindex = 0;

/* default iconType to be extracted */
int		iconExtractSizeID = 5;	// 128x128
int		iconExtractDepth = 32;
icns_type_t	iconType = kThumbnail32BitData;
icns_type_t	maskType = kThumbnail8BitMask;

icns_type_t getIconDataType(int sizeID,int depthVal)
{
	const icns_type_t iconTypes32[] = {  
				kIconServices512PixelDataARGB,
				kIconServices512PixelDataARGB,
				kIconServices256PixelDataARGB,
				kIconServices256PixelDataARGB,
				kThumbnail32BitData,
				kThumbnail32BitData,
				kHuge32BitData,
				kHuge32BitData,
				kLarge32BitData,
				kLarge32BitData,
				kSmall32BitData,
				kSmall32BitData,
				kNoIconData
			     };

	const icns_type_t iconTypes8[] = {  
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kHuge8BitData,
				kHuge8BitData,
				kLarge8BitData,
				kLarge8BitData,
				kSmall8BitData,
				kSmall8BitData,
				kMini8BitData
			     };

	const icns_type_t iconTypes4[] = {  
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kHuge4BitData,
				kHuge4BitData,
				kLarge4BitData,
				kLarge4BitData,
				kSmall4BitData,
				kSmall4BitData,
				kMini4BitData
			     };
			     
	const icns_type_t iconTypes1[] = {  
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kNoIconData,
				kHuge1BitData,
				kHuge1BitData,
				kLarge1BitData,
				kLarge1BitData,
				kSmall1BitData,
				kSmall1BitData,
				kMini1BitData
	};
	
	switch(depthVal) {
		case 32:
			return iconTypes32[sizeID];
		case 8:
			return iconTypes8[sizeID];
		case 4:
			return iconTypes4[sizeID];
		case 1:
			return iconTypes1[sizeID];
		default:
			return kNoIconData;
	}
}

icns_type_t getIconMaskType(int sizeID,int depthVal)
{
	const icns_type_t maskTypes32[] = {  
				kNoMaskData, 
				kNoMaskData, 
				kNoMaskData,
				kNoMaskData,
				kThumbnail8BitMask,
				kThumbnail8BitMask,
				kHuge8BitMask,
				kHuge8BitMask,
				kLarge8BitMask,
				kLarge8BitMask,
				kSmall8BitMask,
				kSmall8BitMask,
				kNoMaskData
			     };
	const icns_type_t maskTypes8x[] = {  
				kNoMaskData, 
				kNoMaskData, 
				kNoMaskData,
				kNoMaskData,
				kNoMaskData,
				kNoMaskData,
				kHuge1BitMask,
				kHuge1BitMask,
				kLarge1BitMask,
				kLarge1BitMask,
				kSmall1BitMask,
				kSmall1BitMask,
				kMini1BitMask
			     };
     if(depthVal <= 8)
	     return maskTypes8x[sizeID];
     else
	     return maskTypes32[sizeID];
}

int parse_size(char *size)
{
	const char *sizes[] = {
				 "512",
				 "512x512",
				 "256",
				 "256x256",
				 "128",
				 "128x128",
				 "64",
				 "64x64",
				 "32",
				 "32x32",
				 "16",
				 "16x16",
				 "16x12"
				};
				
			     

	int i;
	int found = 0;
	if(size == NULL)
		return -1;
	for(i = 0; i < ARRAY_SIZE(sizes); i++) {
		if(strcmp(sizes[i], size) == 0) {
			iconExtractSizeID = i;
			iconType = getIconDataType(iconExtractSizeID,iconExtractDepth);
			maskType = getIconMaskType(iconExtractSizeID,iconExtractDepth);
			found = 1;
			break;
		}
	}
	if(!found)
		return -1;
	return 0;
}

int parse_depth(char *cdepth)
{
	const char *depths[] =   {"32", "8", "4", "1"};
	const int depthVals[] =  { 32 ,  8 ,  4 ,  1 };
	int i;
	int found = 0;
	if(cdepth == NULL)
		return -1;
	for(i = 0; i < ARRAY_SIZE(depths); i++) {
		if(strcmp(depths[i], cdepth) == 0) {
			iconExtractDepth = depthVals[i];
			iconType = getIconDataType(iconExtractSizeID,iconExtractDepth);
			maskType = getIconMaskType(iconExtractSizeID,iconExtractDepth);
			found = 1;
		}
	}
	if(!found)
		return -1;
	return 0;
}

int parse_options(int argc, char** argv)
{
	int err,opt;
	err = 0;
	while(1) {
		opt = getopt(argc, argv, "-d:h:s:");
		if(opt < 0)
			break;
		switch(opt) {
			case 'd':
				if((err = parse_depth(optarg))) {
					fprintf(stderr, "Invalid icon color depth specified.\n");
					return err;
				}
				break;
			case 'h':
				return 1;
				break;
			case 's':
				if((err = parse_size(optarg))) {
					fprintf(stderr, "Invalid icon size specified.\n");
					return err;
				}
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
		printf("Usage: icns2png [options] [file]                                              \n");
		printf("icns2png extracts images from mac .icns files, and exports them to png format.\n");
		printf("                                                                              \n");
		printf("Examples:                                                                     \n");
		printf("icns2png anicon.icns            # Extract 128x128 32-bit icon to anicon.png   \n");
		printf("icns2png -s 64 anicon.icns      # Extract 64x64 32-bit icon to anicon.png     \n");
		printf("icns2png -s 32 -d 1 anicon.icns # Extract 32x32 1-bit icon to anicon.png      \n");
		printf("                                                                              \n");
		printf("Options:                                                                      \n");
		printf(" -d            Sets the pixel depth of the icon to extract. (1,4,8,32)        \n");
		printf(" -s            Sets the size of the icon to extract. (16,32,64,128,256,512)   \n");
		printf("               Sizes 16x12, 16x16, 32x32, 64x64, 128x128, etc. are also valid.\n");
		printf(" -h            Shows this help message.                                       \n");
		return 0;
	}
	
	for(count = 0; count < fileindex; count++)
	{
		if(ConvertIcnsFile(inputfiles[count]))
			fprintf(stderr, "Conversion of %s failed!\n",inputfiles[count]);
	}
	
	return 0;
}

int ConvertIcnsFile(char *filename)
{
	int		error = 0;
	char		*infilename;
	char		*outfilename;
	unsigned int	filenamelength = 0;
	unsigned int	infilenamelength = 0;
	unsigned int	outfilenamelength = 0;
	unsigned char	*fileDataPtr = NULL;
	unsigned long	fileDataSize = 0;
	icns_bool_t	byteSwap = 0;
	icns_family_t	*iconFamily = NULL;
	icns_element_t	*icon = NULL;
	icns_element_t	*mask = NULL;
	icns_image_t	iconImage;
	icns_image_t	maskImage;
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
	error = GetICNSFamilyFromFileData(fileDataSize,fileDataPtr,&iconFamily);
	
	if(error) {
		fprintf(stderr,"Unable to load icns file into icon family!\n");
		goto cleanup;
	}
	
	error = GetICNSElementFromICNSFamily(iconFamily,iconType,&byteSwap,&icon);
	
	if(error) {
		fprintf(stderr,"Unable to load icon data from icon family!\n");
		goto cleanup;
	}

	error = GetICNSImageFromICNSElement(icon, byteSwap, &iconImage);

	if(error) {
		fprintf(stderr,"Unable to load raw data from icon data!\n");
		goto cleanup;
	}
	
	if(maskType != kNoMaskData) {
		error = GetICNSElementFromICNSFamily(iconFamily,maskType,&byteSwap,&mask);
	
		if(error) {
			fprintf(stderr,"Unable to load icon data from icon family!\n");
			goto cleanup;
		}
		
		error = GetICNSImageFromICNSElement(mask, byteSwap, &maskImage);
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
	if(icon != NULL) {
		free(icon);
		icon = NULL;
	}
	if(mask != NULL) {
		free(mask);
		mask = NULL;
	}
	free(infilename);
	free(outfilename);
	
	return error;
}

//***************************** ReadFile **************************//
// Generic file reading routine

int ReadFile(char *fileName,unsigned long *dataSize,void **dataPtr)
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

//***************************** WritePNGImage **************************//
// Relatively generic PNG file writing routine

int	WritePNGImage(FILE *outputfile,icns_image_t *image,icns_image_t *mask)
{
	int 			width = image->imageWidth;
	int 			height = image->imageHeight;
	int 			image_channels = image->imageChannels;
	int			image_bit_depth = image->imageDepth/image_channels;
	png_structp 		png_ptr;
	png_infop 		info_ptr;
	png_bytep 		*row_pointers;
	int			i, j;
	
	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (png_ptr == NULL)
	{
		fprintf (stderr, "PNG error: cannot allocate libpng main struct\n");
		return 1;
	}

	info_ptr = png_create_info_struct (png_ptr);

	if (info_ptr == NULL)
	{
		fprintf (stderr, "PNG error: cannot allocate libpng info struct\n");
		png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
		return 1;
	}

	png_init_io(png_ptr, outputfile);

	png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
	
	png_set_IHDR (png_ptr, info_ptr, width, height, image_bit_depth, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	png_write_info (png_ptr, info_ptr);

	png_set_swap_alpha( png_ptr );
	png_set_swap( png_ptr );

	if(image_bit_depth < 8)
		png_set_packing (png_ptr);

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep)*height);
	
	if (row_pointers == NULL)
	{
		fprintf (stderr, "PNG error: unable to allocate row_pointers\n");
	}
	else
	{
		for (i = 0; i < height; i++)
		{
			if ((row_pointers[i] = (png_bytep)malloc(width*image_channels)) == NULL)
			{
				fprintf (stderr, "PNG error: unable to allocate rows\n");
				for (j = 0; j < i; j++)
					free(row_pointers[j]);
				free(row_pointers);
				return 1;
			}
			
			for(j = 0; j < width; j++)
			{
				pixel32		*src_rgb_pixel;
				pixel32		*src_pha_pixel;
				pixel32		*dst_pixel;
				
				dst_pixel = (pixel32 *)&(row_pointers[i][j*image_channels]);
				
				src_rgb_pixel = (pixel32 *)&(image->imageData[i*width*image_channels+j*image_channels]);

				dst_pixel->red = src_rgb_pixel->red;
				dst_pixel->green = src_rgb_pixel->green;
				dst_pixel->blue = src_rgb_pixel->blue;
				
				if(mask != NULL) {
					src_pha_pixel = (pixel32 *)&(mask->imageData[i*width+j]);
					dst_pixel->alpha = src_pha_pixel->alpha;
				} else {
					dst_pixel->alpha = src_rgb_pixel->alpha;
				}
			}
		}
	}
	
	png_write_image (png_ptr,row_pointers);

	png_write_end (png_ptr, info_ptr);
	
	png_destroy_write_struct (&png_ptr, &info_ptr);

	for (j = 0; j < height; j++)
		free(row_pointers[j]);
	free(row_pointers);

	return 0;
}
