/*
 * icnsutil
 *
 * Copyright (C) 2008 Julien BLACHE <jb@jblache.org>
 * Copyright (C) 2012 Mathew Eis <mathew@eisbox.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <errno.h>

#include <png.h>
#include <icns.h>

#define	FALSE	0
#define	TRUE	1

#if PNG_LIBPNG_VER >= 10209
 #define PNG2ICNS_EXPAND_GRAY 1
#endif

static const char *icon_names[] = {
	"icon_16x16.png",
	"icon_16x16@2x.png",
	"icon_32x32.png",
	"icon_32x32@2x.png",
	"icon_128x128.png",
	"icon_128x128@2x.png",
	"icon_256x256.png",
	"icon_256x256@2x.png",
	"icon_512x512.png",
	"icon_512x512@2x.png",
	NULL
};

static int read_png(FILE *fp, png_bytepp buffer, int32_t *bpp, int32_t *width, int32_t *height)
{
	png_structp png_ptr;
	png_infop info;
	png_uint_32 w;
	png_uint_32 h;
	png_bytep *rows;

	int bit_depth;
	int32_t color_type;

	int row;
	int rowsize;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
		return FALSE;

	info = png_create_info_struct(png_ptr);
	if (info == NULL)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return FALSE;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info, NULL);
		return FALSE;
	}

	png_init_io(png_ptr, fp);

	png_read_info(png_ptr, info);
	png_get_IHDR(png_ptr, info, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);

	switch (color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			#ifdef PNG2ICNS_EXPAND_GRAY
			png_set_expand_gray_1_2_4_to_8(png_ptr);
			#else
			png_set_gray_1_2_4_to_8(png_ptr);
			#endif

			if (bit_depth == 16) {
				png_set_strip_16(png_ptr);
				bit_depth = 8;
			}

			png_set_gray_to_rgb(png_ptr);
			png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			#ifdef PNG2ICNS_EXPAND_GRAY
			png_set_expand_gray_1_2_4_to_8(png_ptr);
			#else
			png_set_gray_1_2_4_to_8(png_ptr);
			#endif

			if (bit_depth == 16) {
				png_set_strip_16(png_ptr);
				bit_depth = 8;
			}

			png_set_gray_to_rgb(png_ptr);
			break;

		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png_ptr);

			if (png_get_valid(png_ptr, info, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(png_ptr);
			else
				png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
			break;

		case PNG_COLOR_TYPE_RGB:
			if (bit_depth == 16) {
				png_set_strip_16(png_ptr);
				bit_depth = 8;
			}

			png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			if (bit_depth == 16) {
				png_set_strip_16(png_ptr);
				bit_depth = 8;
			}

			break;
	}

	*width = w;
	*height = h;
	*bpp = bit_depth * 4;

	png_read_update_info(png_ptr, info);

	rowsize = png_get_rowbytes(png_ptr, info);
	rows = malloc (sizeof(png_bytep) * h);
	*buffer = malloc(rowsize * h + 8);

	rows[0] = *buffer;
	for (row = 1; row < h; row++)
	{
		rows[row] = rows[row-1] + rowsize;
	}

	png_read_image(png_ptr, rows);
	png_destroy_read_struct(&png_ptr, &info, NULL);

	free(rows);

	return TRUE;
}

static int add_png_to_family(icns_family_t **iconFamily, char *pngname)
{
	FILE *pngfile;

	int icnsErr = ICNS_STATUS_OK;
	icns_image_t icnsImage;
	icns_image_t icnsMask;
	icns_type_t iconType;
	icns_type_t maskType;
	icns_icon_info_t iconInfo;

	icns_element_t *iconElement = NULL;
	icns_element_t *maskElement = NULL;
	char iconStr[5] = {0,0,0,0,0};
	char maskStr[5] = {0,0,0,0,0};
	int iconDataOffset = 0;
	int maskDataOffset = 0;
    
    char isHiDPI = 0;
    
    int pngnamelen = strlen(pngname);
    int namea2xpng = pngnamelen - 7;

	png_bytep buffer;
	int width, height, bpp;
    
	if(namea2xpng > 0) {
			if(memcmp(&pngname[namea2xpng],"@2x.png",7) == 0) {
					isHiDPI = 1;
			}
			if(memcmp(&pngname[namea2xpng],"@2X.PNG",7) == 0) {
					isHiDPI = 1;
			}
	}
    
	pngfile = fopen(pngname, "rb");
	if (pngfile == NULL)
	{
		fprintf(stderr, "Could not open '%s' for reading: %s\n", pngname, strerror(errno));
		return FALSE;
	}

	if (!read_png(pngfile, &buffer, &bpp, &width, &height))
	{
		fprintf(stderr, "Failed to read PNG file\n");
		fclose(pngfile);

		return FALSE;
	}

	fclose(pngfile);

	icnsImage.imageWidth = width;
	icnsImage.imageHeight = height;
	icnsImage.imageChannels = 4;
	icnsImage.imagePixelDepth = 8;
	icnsImage.imageDataSize = width * height * 4;
	icnsImage.imageData = buffer;

	iconInfo.isImage = 1;
	iconInfo.iconWidth = icnsImage.imageWidth;
	iconInfo.iconHeight = icnsImage.imageHeight;
	iconInfo.iconBitDepth = bpp;
	iconInfo.iconChannels = (bpp == 32 ? 4 : 1);
	iconInfo.iconPixelDepth = bpp / iconInfo.iconChannels;

	iconType = icns_get_type_from_image_info_advanced(iconInfo,isHiDPI);
	maskType = icns_get_mask_type_for_icon_type(iconType);

	icns_type_str(iconType,iconStr);
	icns_type_str(maskType,maskStr);

	/* Only convert the icons that match sizes icns supports */
	if (iconType == ICNS_NULL_TYPE)
	{
		fprintf(stderr, "Unable to determine icon type: PNG file '%s' is %dx%d\n", pngname, width, height);
		free(buffer);

		return FALSE;
	}

	if (bpp != 32)
	{
		fprintf(stderr, "Bit depth %d unsupported in '%s'\n", bpp, pngname);
		free(buffer);

		return FALSE;
	}

	icns_set_print_errors(0);
	if (icns_get_element_from_family(*iconFamily, iconType, &iconElement) == ICNS_STATUS_OK)
	{
		icns_set_print_errors(1);

		fprintf(stderr, "Duplicate icon element of type '%s' detected (%s)\n", iconStr, pngname);
		free(buffer);

		return FALSE;
	}
	
	icns_set_print_errors(1);
	
	/*
	if(maskType != ICNS_NULL_TYPE)
	{
		printf("Using icns type '%s', mask '%s' for '%s'\n", iconStr, maskStr, pngname);
	}
	else
	{
		printf("Using icns type '%s' (ARGB) for '%s'\n", iconStr, pngname);
	}
	*/
	
	icnsErr = icns_new_element_from_image(&icnsImage, iconType, &iconElement);
	
	if (iconElement != NULL)
	{
		if (icnsErr == ICNS_STATUS_OK)
		{
			icns_set_element_in_family(iconFamily, iconElement);
		}
		free(iconElement);
	}

	if(maskType != ICNS_NULL_TYPE)
	{
		icns_init_image_for_type(maskType, &icnsMask);

		iconDataOffset = 0;
		maskDataOffset = 0;
	
		while ((iconDataOffset < icnsImage.imageDataSize) && (maskDataOffset < icnsMask.imageDataSize))
		{
			icnsMask.imageData[maskDataOffset] = icnsImage.imageData[iconDataOffset+3];
			iconDataOffset += 4; /* move to the next alpha byte */
			maskDataOffset += 1; /* move to the next byte */
		}

		icnsErr = icns_new_element_from_mask(&icnsMask, maskType, &maskElement);

		if (maskElement != NULL)
		{
			if (icnsErr == ICNS_STATUS_OK)
			{
				icns_set_element_in_family(iconFamily, maskElement);
			}
			free(maskElement);
		}
		
		icns_free_image(&icnsMask);
	}

	free(buffer);

	return TRUE;
}

int usage(void)
{
		printf("Usage: icnsutil -c {icns | iconset} [-o file] file\n");
		exit(1);
		return 1;
}

int iconset_to_icns(char *srcfile, char *dstfile)
{
	FILE *icnsfile;
	icns_family_t	*iconFamily;
	char *pngfile = NULL;
	char *outfile = NULL;
	int	srclen = strlen(srcfile);
	int i = 0;
	
	pngfile = malloc(srclen + 32);
	strncpy(&pngfile[0],&srcfile[0],srclen);
	pngfile[srclen] = '/';
	
	if(dstfile == NULL) {
		int srcstart = srclen - 1;
		
		while(srcfile[srcstart] != '/' && srcfile[srcstart] != '.' && srcstart > 0)
			srcstart--;
			
		if(srcfile[srcstart] != '.' || srcstart == 0)
			srcstart = srclen;
			
		outfile = malloc(srclen + 6);
		strncpy(&outfile[0],&srcfile[0],srcstart);
		strncpy(&outfile[srcstart],".icns",6);
		dstfile = outfile;
	}
	
	icnsfile = fopen (dstfile, "wb+");
	if (icnsfile == NULL)
	{
		fprintf (stderr, "Could not open '%s' for writing: %s\n", dstfile, strerror(errno));
		goto cleanup;
	}
	
	icns_create_family(&iconFamily);
	
	while(icon_names[i] != NULL) {
		strcpy(&pngfile[srclen + 1],icon_names[i]);
		printf("adding %s\n",pngfile);
		add_png_to_family(&iconFamily,pngfile);
		i++;
	}
    
	if (icns_write_family_to_file(icnsfile, iconFamily) != ICNS_STATUS_OK)
	{
		fprintf(stderr, "Failed to write icns file\n");
		fclose(icnsfile);
		goto cleanup;
	}

	fclose(icnsfile);

	printf("Saved icns file to %s\n",dstfile);
	
cleanup:

	if(iconFamily != NULL)
		free(iconFamily);
		
	if(outfile != NULL)
		free(outfile);
		
	if(pngfile != NULL)
		free(pngfile);
		
	return 0;
}

int icns_to_iconset(char *srcfile, char *dstfile)
{
	char *outfile = NULL;
	int	srclen = strlen(srcfile);
	
	if(dstfile == NULL) {
		int srcstart = srclen - 1;
		
		while(srcfile[srcstart] != '/' && srcfile[srcstart] != '.' && srcstart > 0)
			srcstart--;
			
		if(srcfile[srcstart] != '.' || srcstart == 0)
			srcstart = srclen;
			
		outfile = malloc(srclen + 9);
		strncpy(&outfile[0],&srcfile[0],srcstart);
		strncpy(&outfile[srcstart],".iconset",9);
		dstfile = outfile;
	}
	
	printf("src: %s\n",srcfile);
	printf("dst: %s\n",dstfile);

	return 0;
}

int main(int argc, char **argv)
{
	int (*conv_fn)(char *,char *);
	
	if (argc < 4)
		usage();
		
	if ( strcmp(argv[1],"-c") != 0)
		usage();

	if ( strcmp(argv[2],"icns") == 0)
		conv_fn = &iconset_to_icns;
	else if ( strcmp(argv[2],"iconset") == 0)
		conv_fn = &icns_to_iconset;
	else
		usage();
	
	if (strcmp(argv[3],"-o") == 0) {
		return (*conv_fn)(argv[5],argv[4]);
	} else {
		return (*conv_fn)(argv[3],NULL);
	}
	
	return 0;
}
