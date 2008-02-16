/*
Copyright (C) 2008 Chenxiao Zhao <chenxiao.zhao@gmail.com>
 
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
#include <png.h>

#include "jp2dec.h"
#include "icns.h"

typedef struct pixel32_struct
{
	unsigned char alpha;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} pixel32;

/**
sample error callback expecting a FILE* client object
*/
void error_callback(const char *msg, void *client_data) {
	//FILE *stream = (FILE*)client_data;
	//fprintf(stream, "[ERROR] %s", msg);
}
/**
sample warning callback expecting a FILE* client object
*/
void warning_callback(const char *msg, void *client_data) {
	//FILE *stream = (FILE*)client_data;
	//fprintf(stream, "[WARNING] %s", msg);
}
/**
sample debug callback expecting no client object
*/
void info_callback(const char *msg, void *client_data) {
	//(void)client_data;
	//fprintf(stdout, "[INFO] %s", msg);
}

// Convert from uncompressed opj data to ICNS_ImageData
int opj2icns_image_t(opj_image_t *image, icns_image_t *outIcon)
{
	int		error = 0;
	unsigned int	iconWidth = 0;
	unsigned int	iconHeight = 0;
	unsigned int	iconDepth = 0;
	unsigned int	iconChannels = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	blockSize = 0;
	int adjustR, adjustG, adjustB, adjustA;
	unsigned char	*dataPtr = NULL;
	int i,j;
	
	if(image == NULL) {
		return 1;
	}
	
	iconWidth = image->comps[0].w;
	iconHeight = image->comps[0].h;
	iconChannels = image->numcomps;
	iconDepth = image->comps[0].prec * image->numcomps;
	
	blockSize = iconWidth *(iconDepth / sizeof(icns_uint8_t));
	iconDataSize = iconHeight * blockSize;
	outIcon->imageWidth = iconWidth;
	outIcon->imageHeight = iconHeight;
	outIcon->imageChannels = iconChannels;
	outIcon->imageDepth = iconDepth;
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
		printf("BMP CONVERSION: Truncating component 3 from %d bits to 8 bits\n",image->comps[3].prec);
	} else 
		adjustA = 0;
	if (image->comps[0].prec > 8) {
		adjustR = image->comps[0].prec - 8;
		printf("BMP CONVERSION: Truncating component 0 from %d bits to 8 bits\n",image->comps[0].prec);
	} else 
		adjustR = 0;
	if (image->comps[1].prec > 8) {
		adjustG = image->comps[1].prec - 8;
		printf("BMP CONVERSION: Truncating component 1 from %d bits to 8 bits\n",image->comps[1].prec);
	} else 
		adjustG = 0;
	if (image->comps[2].prec > 8) {
		adjustB = image->comps[2].prec - 8;
		printf("BMP CONVERSION: Truncating component 2 from %d bits to 8 bits\n",image->comps[2].prec);
	} else 
		adjustB = 0;
	
	for (i = 0; i < iconHeight; i++) {
		for(j = 0; j < iconWidth; j++) {
			pixel32 *dst_pixel;
			int r, g, b, a;
						
			a = image->comps[3].data[i*iconWidth+j];
			a += (image->comps[3].sgnd ? 1 << (image->comps[3].prec - 1) : 0);
			r = image->comps[0].data[i*iconWidth+j];
			r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
			g = image->comps[1].data[i*iconWidth+j];
			g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
			b = image->comps[2].data[i*iconWidth+j];
			b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

			dst_pixel = (pixel32 *)&(dataPtr[i*iconWidth*iconChannels+j*iconChannels]);

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
	event_mgr.error_handler = error_callback;
	event_mgr.warning_handler = warning_callback;
	event_mgr.info_handler = info_callback;

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
		fprintf(stderr, "ERROR -> j2k_to_image: failed to decode image!\n");
		opj_destroy_decompress(dinfo);
		opj_cio_close(cio);
		return image;
	}

	/* close the byte stream */
	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);

	return image;
}
