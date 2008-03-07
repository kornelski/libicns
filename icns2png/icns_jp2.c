/*
File:       icns_jp2.c
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
#include "icns_internals.h"

#ifdef ICNS_JASPER
#include <jasper/jasper.h>
#endif

#ifdef ICNS_OPENJPEG
#include <openjpeg.h>
#endif


#if defined(ICNS_JASPER) && defined(ICNS_OPENJPEG)
	#error "Should use either Jasper or OpenJPEG, but not both!"
#endif


int icns_jp2_to_image(icns_size_t dataSize, icns_byte_t *dataPtr, icns_image_t *imageOut)
{
	int error = ICNS_STATUS_OK;
	
	if(dataPtr == NULL)
	{
		icns_print_err("icns_jp2_to_image: JP2 data is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(imageOut == NULL)
	{
		icns_print_err("icns_jp2_to_image: Image out is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(dataSize == 0)
	{
		icns_print_err("icns_jp2_to_image: Invalid data size! (%d)\n",dataSize);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	#ifdef ICNS_DEBUG
	printf("Decoding JP2 image...\n");
	#endif
	
	#ifdef ICNS_JASPER
		error = icns_jas_jp2_to_image(dataSize, dataPtr, imageOut);	
	#else
	#ifdef ICNS_OPENJPEG
		error = icns_opj_jp2_to_image(dataSize, dataPtr, imageOut);	
	#else
		icns_print_err("icns_jp2_to_image: libicns requires jasper or openjpeg to convert jp2 data!\n");
		icns_free_image(imageOut);
		error = ICNS_STATUS_UNSUPPORTED;
	#endif
	#endif
	
	#ifdef ICNS_DEBUG
	if(error == ICNS_STATUS_OK) {
		printf("  decode result:\n");
		printf("  width is: %d\n",imageOut->imageWidth);
		printf("  height is: %d\n",imageOut->imageHeight);
		printf("  channels are: %d\n",imageOut->imageChannels);
		printf("  pixel depth is: %d\n",imageOut->imagePixelDepth);
		printf("  data size is: %d\n",(int)imageOut->imageDataSize);
	} else {
		printf("  decode failed.\n");
	}
	#endif
	
	return error;
}


#ifdef ICNS_JASPER

int icns_jas_jp2_to_image(icns_size_t dataSize, icns_byte_t *dataPtr, icns_image_t *imageOut)
{
	int           error = ICNS_STATUS_OK;
	jas_stream_t  *imagestream = NULL;
	int           datafmt = 0;
	jas_image_t   *image = NULL;
	jas_matrix_t  *bufs[4] = {NULL,NULL,NULL,NULL};
	icns_sint32_t imageChannels = 0;
	icns_sint32_t imageWidth = 0;
	icns_sint32_t imageHeight = 0;
	icns_sint32_t imagePixelDepth = 0;
	icns_sint32_t imageDataSize = 0;
	icns_byte_t   *imageData = NULL;
	icns_sint8_t    adjust[4] = {0,0,0,0};
	int x, y, c;
	
	if(dataPtr == NULL)
	{
		icns_print_err("icns_jas_jp2_to_image: JP2 data is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(imageOut == NULL)
	{
		icns_print_err("icns_jas_jp2_to_image: Image out is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(dataSize == 0)
	{
		icns_print_err("icns_jas_jp2_to_image: Invalid data size! (%d)\n",dataSize);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	jas_init();
	
	// Connect a jasper stream to the memory
	imagestream = jas_stream_memopen((char*)dataPtr, dataSize);
	
	if (imagestream == NULL)
	{
		icns_print_err("icns_jas_jp2_to_image: Unable to connect to buffer for decoding!\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	
	// Determine the image format
	datafmt = jas_image_getfmt(imagestream);
	
	if (datafmt < 0)
	{
		icns_print_err("icns_jas_jp2_to_image: Unable to determine jp2 data format! (%d)\n",datafmt);
		jas_stream_close(imagestream);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	//.Decode the image data
	// WARNING: libjasper as of 1.9001 may 'crash' here if built with
	// debugging enabled. There is an assertion in the decoding library
	// that fails if there are not 3 channels or components (RGB). The
	// data in icns files is usually RGBA - 4 channels. Thus, the
	// assert will cause the program to crash.
	if (!(image = jas_image_decode(imagestream, datafmt, 0)))
	{
		icns_print_err("icns_jas_jp2_to_image: Error while decoding jp2 data stream!\n");
		return ICNS_STATUS_INVALID_DATA;
	}
	jas_stream_close(imagestream);
	
	// JP2 components, i.e. channels in icns case
	imageChannels = jas_image_numcmpts(image);

	// There should be 4 of these
	if( imageChannels != 4)
	{
		icns_print_err("icns_jas_jp2_to_image: Number of jp2 components (%d) is invalid!\n",imageChannels);
		return ICNS_STATUS_INVALID_DATA;
	}
	
	// Assume that we can retrieve all the relevant image
	// information from componenent number zero.
	imageWidth = jas_image_cmptwidth(image, 0);
	imageHeight = jas_image_cmptheight(image, 0);
	imagePixelDepth = jas_image_cmptprec(image, 0);
	
	imageDataSize = imageHeight * imageWidth * imageChannels * (imagePixelDepth / ICNS_BYTE_BITS);
	imageOut->imageWidth = imageWidth;
	imageOut->imageHeight = imageHeight;
	imageOut->imageChannels = imageChannels;
	imageOut->imagePixelDepth = imagePixelDepth;
	imageOut->imageDataSize = imageDataSize;
	imageData = (icns_byte_t *)malloc(imageDataSize);
	if(!imageData) {
		icns_print_err("icns_jas_jp2_to_image: Unable to allocate memory block of size: %d!\n",imageDataSize);
		error = ICNS_STATUS_NO_MEMORY;
		goto exception;
	} else {
		memset(imageData,0,imageDataSize);
		imageOut->imageData = imageData;
	}
	
	for(c = 0; c < 4; c++)
	{
		int depth = jas_image_cmptprec(image, c);
		if (depth > 8) {
			adjust[c] = depth - 8;
			#ifdef ICNS_DEBUG
			printf("BMP CONVERSION: Will be trucating component %d (%d bits) by %d bits to 8 bits.\n",c,depth,adjust[c]);
			#endif
		} else {
			adjust[c] = 0;
		}
	}
	
	for (c = 0; c < 4; c++)
	{
		if ((bufs[c] = jas_matrix_create(1, imageWidth)) == NULL)
		{
			icns_print_err("icns_jas_jp2_to_image: Unable to create image matix! (No memory)\n");
			error = ICNS_STATUS_NO_MEMORY;
			goto exception;
		}
	}
	
	for (y=0; y<imageHeight; y++)
	{
		icns_rgba_t *dst_pixel;
		int r, g, b, a;
		
		for(c = 0; c < 4; c++)
		{
			if(jas_image_readcmpt(image, c, 0, y, imageWidth, 1, bufs[c]))
			{
				icns_print_err("icns_jas_jp2_to_image: Unable to read data for component #%d!\n",c);
				error = ICNS_STATUS_INVALID_DATA;
				goto exception;
			}
		}
		
		for (x=0; x<imageWidth; x++)
		{
			r = (jas_matrix_getv(bufs[0], x));
			g = (jas_matrix_getv(bufs[1], x));
			b = (jas_matrix_getv(bufs[2], x));
			a = (jas_matrix_getv(bufs[3], x));
			
			dst_pixel = (icns_rgba_t *)&(imageData[y*imageWidth*imageChannels+x*imageChannels]);
			
			dst_pixel->r = (icns_byte_t) ((r >> adjust[0])+((r >> (adjust[0]-1))%2));
			dst_pixel->g = (icns_byte_t) ((g >> adjust[1])+((g >> (adjust[1]-1))%2));
			dst_pixel->b = (icns_byte_t) ((b >> adjust[2])+((b >> (adjust[2]-1))%2));
			dst_pixel->a = (icns_byte_t) ((a >> adjust[3])+((a >> (adjust[3]-1))%2));
			
		}
	}
	
exception:
	
	for(c = 0; c < 4; c++) {
		if(bufs[c] != NULL)
			jas_matrix_destroy(bufs[c]);
	}
	
	jas_image_destroy(image);
	jas_image_clearfmts();
	jas_cleanup();
	
	return error;
}


#endif /* ifdef ICNS_JASPER */



// Only compile the openjpeg routines if we have support for it
#ifdef ICNS_OPENJPEG

int icns_opj_jp2_to_image(icns_size_t dataSize, icns_byte_t *dataPtr, icns_image_t *imageOut)
{
	int         error = ICNS_STATUS_OK;
	opj_image_t *image = NULL;

	if(dataPtr == NULL)
	{
		icns_print_err("icns_opj_jp2_to_image: JP2 data is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(imageOut == NULL)
	{
		icns_print_err("icns_opj_jp2_to_image: Image out is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(dataSize == 0)
	{
		icns_print_err("icns_opj_jp2_to_image: Invalid data size! (%d)\n",dataSize);
		return ICNS_STATUS_INVALID_DATA;
	}

	error = icns_opj_jp2_dec(dataSize, dataPtr, &image);
	
	if(!image)
		return ICNS_STATUS_INVALID_DATA;

	error = icns_opj_to_image(image,imageOut);

	opj_image_destroy(image);

	return error;	
}

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
	int		error = ICNS_STATUS_OK;
	icns_uint32_t	iconWidth = 0;
	icns_uint32_t	iconHeight = 0;
	icns_uint32_t	iconPixelDepth = 0;
	icns_uint32_t	iconChannels = 0;
	unsigned long	iconDataSize = 0;
	unsigned long	blockSize = 0;
	icns_sint8_t    adjust[4] = {0,0,0,0};
	icns_byte_t	*dataPtr = NULL;
	int c, i,j;
	
	if(image == NULL)
	{
		icns_print_err("icns_opj_to_image: OpenJPEG image is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	if(outIcon == NULL)
	{
		icns_print_err("icns_opj_to_image: Icon out is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
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
	outIcon->imagePixelDepth = iconPixelDepth;
	outIcon->imageDataSize = iconDataSize;
	outIcon->imageData = (icns_byte_t *)malloc(iconDataSize);
	if(!outIcon->imageData) {
		icns_print_err("icns_create_family: Unable to allocate memory block of size: %d!\n",iconDataSize);
		return ICNS_STATUS_NO_MEMORY;
	}
	memset(outIcon->imageData,0,iconDataSize);
	
	dataPtr = outIcon->imageData;
	
	for(c = 0; c < 4; c++)
	{
		int depth = image->comps[c].prec;
		if (depth > 8) {
			adjust[c] = depth - 8;
			#ifdef ICNS_DEBUG
			printf("BMP CONVERSION: Will be trucating component %d (%d bits) by %d bits to 8 bits.\n",c,depth,adjust[c]);
			#endif
		} else {
			adjust[c] = 0;
		}
	}
	
	for (i = 0; i < iconHeight; i++) {
		for(j = 0; j < iconWidth; j++) {
			icns_rgba_t *dst_pixel;
			int r, g, b, a;
						
			r = image->comps[0].data[i*iconWidth+j];
			r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
			g = image->comps[1].data[i*iconWidth+j];
			g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
			b = image->comps[2].data[i*iconWidth+j];
			b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);
			a = image->comps[3].data[i*iconWidth+j];
			a += (image->comps[3].sgnd ? 1 << (image->comps[3].prec - 1) : 0);
			
			dst_pixel = (icns_rgba_t *)&(dataPtr[i*iconWidth*iconChannels+j*iconChannels]);
			
			dst_pixel->r = (icns_byte_t) ((r >> adjust[0])+((r >> (adjust[0]-1))%2));
			dst_pixel->g = (icns_byte_t) ((g >> adjust[1])+((g >> (adjust[1]-1))%2));
			dst_pixel->b = (icns_byte_t) ((b >> adjust[2])+((b >> (adjust[2]-1))%2));
			dst_pixel->a = (icns_byte_t) ((a >> adjust[3])+((a >> (adjust[3]-1))%2));
		}
	}
	
	return error;
}

// Decompress jp2
int icns_opj_jp2_dec(icns_size_t dataSize, icns_byte_t *dataPtr, opj_image_t **imageOut)
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
	cio = opj_cio_open((opj_common_ptr)dinfo, dataPtr, dataSize);

	image = opj_decode(dinfo, cio);
	if(!image) {
		icns_print_err("icns_opj_jp2_dec: failed to decode image!\n");
		opj_destroy_decompress(dinfo);
		opj_cio_close(cio);
		return ICNS_STATUS_INVALID_DATA;
	} else {
		*imageOut = image;	
	}

	/* close the byte stream */
	opj_cio_close(cio);
	opj_destroy_decompress(dinfo);

	return ICNS_STATUS_OK;
}



#endif /* ifdef ICNS_OPENJPEG */


