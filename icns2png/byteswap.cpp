/*
* File:       byteutils.cpp
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

#include "byteswap.h"

void		ByteSwapAddr(void *data, int size, bool swap)
{
	if(swap == true)
	{
		switch(size)
		{
			case 1:
				break;
			case 2:
				*((short *)data) = ByteSwap16( *((short *)data ) );
				break;
			case 4:
				*((long *)data)  = ByteSwap32( *((long *)data  ) );
				break;
			case 8:
				break;
			default:
				break;
		}
	}
}
