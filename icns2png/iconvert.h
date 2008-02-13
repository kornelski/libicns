/*
File:       iconvert.cpp
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

#ifndef _ICONVERT_H_
#define	_ICONVERT_H_	1

#include "apple_mactypes.h"
#include "apple_icons.h"

<<<<<<< .mine
#ifndef __cplusplus

#ifndef bool
typedef char bool;
#define	bool	bool
#endif

#ifndef true
#define true    ((bool) 1)
#endif

#ifndef false
#define false   ((bool) 0)
#endif

#endif

=======
#ifndef __cplusplus

#ifndef bool
typedef char bool;
#endif

#ifndef true
#define true    ((bool) 1)
#endif

#ifndef false
#define false   ((bool) 0)
#endif

#endif

>>>>>>> .r27
#define	kByteSize	8

typedef struct IconImage
{
	int			width;
	int			height;
	short			depth;
	long			dataSize; // This should techincally be width*height*pixelDepth/8
	unsigned char	*iconData;
} IconImage, *IconImagePtr;

bool ReadXIconFile(char *fileName,IconFamilyPtr *iconFamily);
bool GetIconDataFromIconFamily(IconFamilyPtr inPtr,ResType iconType,IconImagePtr outIcon, bool *byteswap);
bool ParseIconData(ResType iconType,Ptr rawDataPtr,long rawDataLength,IconImagePtr outIcon, bool byteSwap);

#endif
