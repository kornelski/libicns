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

int ConvertIconFamilyFile(char *filename);
int WritePNGImage(FILE *outputfile,icns_image_t *image,icns_image_t *mask);

#define	ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define	MAX_INPUTFILES  4096

#define	ICNS_INVALID_MASK	0x00000000
#define	ICNS_INVALID_DATA	0x00000000

char 	*inputfiles[MAX_INPUTFILES];
int	fileindex = 0;

/* default iconType to be extracted */
int		iconExtractSizeID = 5;	// 128x128
int		iconExtractDepth = 32;
icns_type_t	iconType = ICNS_128X128_32BIT_DATA;
icns_type_t	maskType = ICNS_128X128_8BIT_MASK;

icns_type_t getIconDataType(int sizeID,int depthVal)
{
	const icns_type_t iconTypes32[] = {  
				ICNS_512x512_32BIT_ARGB_DATA,
				ICNS_512x512_32BIT_ARGB_DATA,
				ICNS_256x256_32BIT_ARGB_DATA,
				ICNS_256x256_32BIT_ARGB_DATA,
				ICNS_128X128_32BIT_DATA,
				ICNS_128X128_32BIT_DATA,
				ICNS_48x48_32BIT_DATA,
				ICNS_48x48_32BIT_DATA,
				ICNS_32x32_32BIT_DATA,
				ICNS_32x32_32BIT_DATA,
				ICNS_16x16_32BIT_DATA,
				ICNS_16x16_32BIT_DATA,
				ICNS_INVALID_DATA
			     };

	const icns_type_t iconTypes8[] = {  
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_48x48_8BIT_DATA,
				ICNS_48x48_8BIT_DATA,
				ICNS_32x32_8BIT_DATA,
				ICNS_32x32_8BIT_DATA,
				ICNS_16x16_8BIT_DATA,
				ICNS_16x16_8BIT_DATA,
				ICNS_16x12_8BIT_DATA
			     };

	const icns_type_t iconTypes4[] = {  
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_48x48_4BIT_DATA,
				ICNS_48x48_4BIT_DATA,
				ICNS_32x32_4BIT_DATA,
				ICNS_32x32_4BIT_DATA,
				ICNS_16x16_4BIT_DATA,
				ICNS_16x16_4BIT_DATA,
				ICNS_16x12_4BIT_DATA
			     };
			     
	const icns_type_t iconTypes1[] = {  
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_INVALID_DATA,
				ICNS_48x48_1BIT_DATA,
				ICNS_48x48_1BIT_DATA,
				ICNS_32x32_1BIT_DATA,
				ICNS_32x32_1BIT_DATA,
				ICNS_16x16_1BIT_DATA,
				ICNS_16x16_1BIT_DATA,
				ICNS_16x12_1BIT_DATA
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
			return ICNS_INVALID_DATA;
	}
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
				 "48",
				 "48x48",
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
			maskType = icns_get_mask_type_for_icon_type(iconType);
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
			maskType = icns_get_mask_type_for_icon_type(iconType);
			found = 1;
		}
	}
	if(!found)
		return -1;
	return 0;
}

int parse_options(int argc, char** argv)
{
	int err = 0;
	int opt;

	while(1) {
		opt = getopt(argc, argv, "d:h:s:");
		if(opt < 0)
			break;
		switch(opt)
		{
			case 'd':
				if((err = parse_depth(optarg))) {
					fprintf(stderr, "Invalid icon color depth specified.\n");
					return err;
				}
				break;
			case 'h':
				return -1;
				break;
			case 's':
				if((err = parse_size(optarg))) {
					fprintf(stderr, "Invalid icon size specified.\n");
					return err;
				}
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
		printf("Usage: icns2png [options] [file]                                              \n");
		printf("icns2png extracts images from mac .icns files, and exports them to png format.\n");
		printf("                                                                              \n");
		printf("Examples:                                                                     \n");
		printf("icns2png anicon.icns            # Extract 128x128 32-bit icon to anicon.png   \n");
		printf("icns2png -s 48 anicon.icns      # Extract 48x48 32-bit icon to anicon.png     \n");
		printf("icns2png -s 32 -d 1 anicon.icns # Extract 32x32 1-bit icon to anicon.png      \n");
		printf("                                                                              \n");
		printf("Options:                                                                      \n");
		printf(" -d            Sets the pixel depth of the icon to extract. (1,4,8,32)        \n");
		printf(" -s            Sets the size of the icon to extract. (16,32,64,128,256,512)   \n");
		printf("               Sizes 16x12, 16x16, 32x32, 48x48, 128x128, etc. are also valid.\n");
		printf(" -h            Shows this help message.                                       \n");
		return 0;
	}
	
	for(count = 0; count < fileindex; count++)
	{
		if(ConvertIconFamilyFile(inputfiles[count]))
			fprintf(stderr, "Conversion of %s failed!\n",inputfiles[count]);
	}
	
	return 0;
}

int ConvertIconFamilyFile(char *filename)
{
	int		error = 0;
	char		*infilename = NULL;
	char		*outfilename = NULL;
	unsigned int	filenamelength = 0;
	unsigned int	infilenamelength = 0;
	unsigned int	outfilenamelength = 0;
	icns_family_t	*iconFamily = NULL;
	icns_element_t	*icon = NULL;
	icns_element_t	*mask = NULL;
	icns_image_t	iconImage;
	icns_image_t	maskImage;
	FILE            *inFile = NULL;
	FILE 		*outfile = NULL;
	
	memset ( &iconImage, 0, sizeof(icns_image_t) );
	memset ( &maskImage, 0, sizeof(icns_image_t) );

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
	
	inFile = fopen( infilename, "r" );
	
	if ( inFile == NULL ) {
		fprintf(stderr,"Unable to open file %s!\n",infilename);
		goto cleanup;
	}

	error = icns_read_family_from_file(inFile,&iconFamily);
		
	fclose(inFile);
		
	if(error) {
		fprintf(stderr,"Unable to read icns family from file %s!\n",infilename);
		goto cleanup;
	}
	
	error = icns_get_element_from_family(iconFamily,iconType,&icon);
	
	if(error) {
		fprintf(stderr,"Unable to load icon data from icon family!\n");
		goto cleanup;
	}

	error = icns_get_image32_from_element(icon, &iconImage);

	if(error) {
		fprintf(stderr,"Unable to load image from icon data!\n");
		goto cleanup;
	}
	
	if(maskType != ICNS_INVALID_MASK) {
		error = icns_get_element_from_family(iconFamily,maskType,&mask);
	
		if(error) {
			fprintf(stderr,"Unable to load mask data from icon family!\n");
			goto cleanup;
		}
		
		error = icns_get_image32_from_element(mask, &maskImage);

		if(error) {
			fprintf(stderr,"Unable to load image from mask data!\n");
			goto cleanup;
		}
	}
	
	outfile = fopen(outfilename,"w");
	if(!outfile) {
		fprintf(stderr,"Unable to open %s for writing!\n",outfilename);
		goto cleanup;
	}
	
	// Finally! We save the image
	if(maskType != ICNS_INVALID_MASK) {
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
	icns_free_image(&iconImage);
	icns_free_image(&maskImage);
	free(infilename);
	free(outfilename);
	
	return error;
}

//***************************** WritePNGImage **************************//
// Relatively generic PNG file writing routine

int	WritePNGImage(FILE *outputfile,icns_image_t *image,icns_image_t *mask)
{
	int 			width = 0;
	int 			height = 0;
	int 			image_channels = 0;
	int			image_bit_depth = 0;
	int 			mask_channels = 0;
	int			mask_bit_depth = 0;
	png_structp 		png_ptr;
	png_infop 		info_ptr;
	png_bytep 		*row_pointers;
	int			i, j;
	
	if (image == NULL)
	{
		fprintf (stderr, "icns image NULL!\n");
		return -1;
	}
	
	width = image->imageWidth;
	height = image->imageHeight;
	image_channels = image->imageChannels;
	image_bit_depth = image->pixel_depth;
	
	//printf("width: %d\n",width);
	//printf("height: %d\n",height);
	//printf("image_channels: %d\n",image_channels);
	//printf("image_bit_depth: %d\n",image_bit_depth);
	
	if(mask != NULL) {
		mask_channels = mask->imageChannels;
		mask_bit_depth = mask->pixel_depth;
	}
	
	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (png_ptr == NULL)
	{
		fprintf (stderr, "PNG error: cannot allocate libpng main struct\n");
		return -1;
	}

	info_ptr = png_create_info_struct (png_ptr);

	if (info_ptr == NULL)
	{
		fprintf (stderr, "PNG error: cannot allocate libpng info struct\n");
		png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
		return -1;
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
				return -1;
			}
			
			for(j = 0; j < width; j++)
			{
				icns_pixel32_t	*src_rgb_pixel;
				icns_pixel32_t	*src_pha_pixel;
				icns_pixel32_t	*dst_pixel;
				
				dst_pixel = (icns_pixel32_t *)&(row_pointers[i][j*image_channels]);
				
				src_rgb_pixel = (icns_pixel32_t *)&(image->imageData[i*width*image_channels+j*image_channels]);

				dst_pixel->red = src_rgb_pixel->red;
				dst_pixel->green = src_rgb_pixel->green;
				dst_pixel->blue = src_rgb_pixel->blue;
				
				if(mask != NULL) {
					src_pha_pixel = (icns_pixel32_t *)&(mask->imageData[i*width*mask_channels+j*mask_channels]);
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
