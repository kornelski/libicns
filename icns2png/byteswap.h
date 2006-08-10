/*
* File:       byteutils.h
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

#ifndef __BYTEUTILS__
#define __BYTEUTILS__

#define		BytesSwapped	(	((long)'ETYB') == *((long*)("BYTE"))   )

#define		ByteSwap(x,s,b)	(  (b == false) ? x : (( s == 1 ? ByteSwap08(x) : ( s == 2 ? ByteSwap16(x) : ( s == 4 ? ByteSwap32(x) : (0x00000000) ) ) )))

#define		ByteSwap08(x)	(  x  )
#define		ByteSwap16(x)	(  (( x & 0xFF00 ) >> 8) | (( x & 0x00FF ) << 8) )
#define		ByteSwap32(x)	(  (( x & 0xFF000000 ) >> 24) | (( x & 0x00FF0000 ) >> 8) | (( x & 0x0000FF00 ) << 8) | (( x & 0x000000FF ) << 24) )

void		ByteSwapAddr(void *data, int size, bool swap);

#endif /* __BYTEUTILS__ */
