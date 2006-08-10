/*
* File:       pngwriter.cpp
* Copyright (C) 2002 Mathew Eis <mathew@bearca.com>
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
#include <png.h>

#include "pngwriter.h"
#include "icondefs.h"
#include "iconvert.h"

typedef struct pixel32_struct
{
	unsigned char			alpha;
	unsigned char			red;
	unsigned char			green;
	unsigned char			blue;
} pixel32;

bool	WritePNGImage(FILE *outputfile,IconImage *iconImage,IconImage *maskImage)
{
	bool			error = false;
    int 			width = iconImage->width;
    int 			height = iconImage->height;
	int 			image_channels = 4;
	int				mask_channels = 1;
    int				image_bit_depth = iconImage->depth/image_channels;
    int				alpha_bit_depth = maskImage->depth/mask_channels;
    png_structp 	png_ptr;
    png_infop 		info_ptr;
	png_bytep 		*row_pointers;
    int				i, j;

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (png_ptr == NULL)
    {
        fprintf (stderr, "PNG error: cannot allocate libpng main struct\n");
        return false;
    }

    info_ptr = png_create_info_struct (png_ptr);
    
    if (info_ptr == NULL)
    {
        fprintf (stderr, "PNG error: cannot allocate libpng info struct\n");
        png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
        return false;
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
				return false;
			}
			
			for(j = 0; j < width; j++)
			{
				pixel32	*src_rgb_pixel;
				pixel32	*src_pha_pixel;
				pixel32	*dst_pixel;
				
				src_rgb_pixel = (pixel32 *)&(iconImage->iconData[i*width*image_channels+j*image_channels]);
				src_pha_pixel = (pixel32 *)&(maskImage->iconData[i*width+j]);
				
				dst_pixel = (pixel32 *)&(row_pointers[i][j*image_channels]);
								
				dst_pixel->alpha = src_pha_pixel->alpha;
				dst_pixel->red = src_rgb_pixel->red;
				dst_pixel->green = src_rgb_pixel->green;
				dst_pixel->blue = src_rgb_pixel->blue;
			}
		}
	}
	
	png_write_image (png_ptr,row_pointers);

	png_write_end (png_ptr, info_ptr);
    
    png_destroy_write_struct (&png_ptr, &info_ptr);

	for (j = 0; j < height; j++)
		free(row_pointers[j]);

	return error;
}