/*
File:       icns.h
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
Copyright (C) 2002 Chenxiao Zhao <chenxiao.zhao@gmail.com>

With the exception of the limited portions mentiond, this library
is free software; you can redistribute it and/or modify it under
the terms of the GNU Library General Public License as published
by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
Boston, MA 02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "icns.h"

#ifndef _ICNS_INTERNALS_H_
#define	_ICNS_INTERNALS_H_

/*  Compile-time variables   */
/*  These should really be set from the Makefile */

// Enable debugging messages?
// #define	ICNS_DEBUG	1

// Enable supprt for 256x256 and 512x512 icons
// Use either Jasper or OpenJPEG, but not both
// #define	ICNS_JASPER
// #define	ICNS_OPENJPEG

/* Make sure we're not using both libraries */
#if defined(ICNS_JASPER) && defined(ICNS_OPENJPEG)
	#error "Must use either Jasper or OpenJPEG, but not both!"
#endif

/*  Include Jasper headers   */
#ifdef ICNS_JASPER
#include <jasper/jasper.h>
#endif

/*  Include OpenJPEG headers   */
#ifdef ICNS_OPENJPEG
#include <openjpeg.h>
#endif

// We do not want to expose any of the internal stuff
#pragma GCC visibility push(hidden)

/* icns structures */

typedef struct icns_rgba_t
{
	icns_byte_t	 r;
	icns_byte_t	 g;
	icns_byte_t	 b;
	icns_byte_t	 a;
} icns_rgba_t;

typedef struct icns_argb_t
{
	icns_byte_t	 a;
	icns_byte_t	 r;
	icns_byte_t	 g;
	icns_byte_t	 b;
} icns_argb_t;

typedef struct icns_rgb_t
{
	icns_byte_t	 r;
	icns_byte_t	 g;
	icns_byte_t	 b;
} icns_rgb_t;

/* icns constants */

#define			ICNS_BYTE_BITS	8

/* icns macros */

/*
These functions swap the position of the alpha channel
*/


static inline icns_rgba_t ICNS_ARGB_TO_RGBA(icns_argb_t pxin) {
	icns_rgba_t pxout;
	pxout.r = pxin.r;
	pxout.g = pxin.g;
	pxout.b = pxin.b;
	pxout.a = pxin.a;
	return pxout;
}

static inline icns_argb_t ICNS_RGBA_TO_ARGB(icns_rgba_t pxin) {
	icns_argb_t pxout;
	pxout.r = pxin.r;
	pxout.g = pxin.g;
	pxout.b = pxin.b;
	pxout.a = pxin.a;
	return pxout;
}

/*
These macros will will on systems that support unaligned
accesses, as well as those that don't support it
*/

#define ICNS_READ_UNALIGNED(val, addr, size)        memcpy(&(val), (addr), size)
#define ICNS_WRITE_UNALIGNED(addr, val, size)       memcpy((addr), &(val), size)


/* global variables */
icns_bool_t gShouldPrintErrors;

/* icns function prototypes */

// icns_debug.c
void bin_print_byte(int x);
void bin_print_int(int x);

// icns_element.c
int icns_new_element_from_image_or_mask(icns_image_t *imageIn,icns_type_t iconType,icns_bool_t isMask,icns_element_t **iconElementOut);
int icns_update_element_with_image_or_mask(icns_image_t *imageIn,icns_bool_t isMask,icns_element_t **iconElement);

// icns_io.c
int icns_parse_family_data(icns_size_t dataSize,icns_byte_t *data,icns_family_t **iconFamilyOut);
int icns_find_family_in_mac_resource(icns_size_t resDataSize, icns_byte_t *resData, icns_family_t **dataOut);
int icns_read_macbinary_resource_fork(icns_size_t dataSize,icns_byte_t *dataPtr,icns_type_t *dataTypeOut, icns_type_t *dataCreatorOut,icns_size_t *parsedResSizeOut,icns_byte_t **parsedResDataOut);
icns_bool_t icns_icns_header_check(icns_size_t dataSize,icns_byte_t *dataPtr);
icns_bool_t icns_rsrc_header_check(icns_size_t dataSize,icns_byte_t *dataPtr);
icns_bool_t icns_macbinary_header_check(icns_size_t dataSize,icns_byte_t *dataPtr);

// icns_jp2.c
#ifdef ICNS_JASPER
int icns_jas_jp2_to_image(icns_size_t dataSize, icns_byte_t *dataPtr, icns_image_t *imageOut);
int icns_jas_image_to_jp2(icns_image_t *image, icns_size_t *dataSizeOut, icns_byte_t **dataPtrOut);
#endif
#ifdef ICNS_OPENJPEG
int icns_opj_jp2_to_image(icns_size_t dataSize, icns_byte_t *dataPtr, icns_image_t *imageOut);
int icns_opj_image_to_jp2(icns_image_t *image, icns_size_t *dataSizeOut, icns_byte_t **dataPtrOut);
int icns_opj_to_image(opj_image_t *image, icns_image_t *outIcon);
int icns_opj_jp2_dec(icns_size_t dataSize, icns_byte_t *dataPtr, opj_image_t **imageOut);
void icns_opj_error_callback(const char *msg, void *client_data);
void icns_opj_warning_callback(const char *msg, void *client_data);
void icns_opj_info_callback(const char *msg, void *client_data);
#endif

// icns_utils.c
void icns_print_err(const char *template, ...);

#pragma GCC visibility pop

#endif /* _ICNS_INTERNALS_H_ */
