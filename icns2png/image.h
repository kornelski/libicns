/*
File:       image.h
Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>

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

#ifndef _IMAGE_H_
#define	_IMAGE_H_	1

typedef struct ImageData
{
	int		width;
	int		height;
	short		depth;
	long		dataSize; // This should always = width*height*pixelDepth/8
	unsigned char	*iconData;
} ImageData, *ImageDataPtr;

#endif
