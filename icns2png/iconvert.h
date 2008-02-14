/*
File:       iconvert.cpp
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

#include <png.h>
#include "apple_mactypes.h"
#include "apple_icons.h"
#include "apple_iconstorage.h"
#include "image.h"

#ifndef _ICONVERT_H_
#define	_ICONVERT_H_	1

#define	kByteSize	8

typedef struct IconData {
	OSType type;
	size_t size;
	unsigned char *data;
} IconData;

int ReadXIconFile(char *fileName,IconFamilyPtr *iconFamily);
int GetIconDataFromIconFamily(IconFamilyPtr inPtr,ResType iconType,IconData *outIcon, int *byteswap);
int ParseIconData(ResType iconType,Ptr rawDataPtr,long rawDataLength,ImageDataPtr outIcon, int byteSwap);
int convertIcon128ToPNG(IconData icon, IconData maskIcon, int byteSwap, char *filename);
int convertIcon512ToPNG(IconData icon, char *filename);

#endif
