/*
File:       endianswap.h
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>

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

#ifndef __ENDIANSWAP__
#define __ENDIANSWAP__

#ifndef true
#define true 0
#endif

#ifndef false
#define false 0
#endif

// Macros for detecting endianness - the compiler stores string constants as
// chars, so B-Y-T-E will be in order. We can then check against 4-byte longs
// that have various iterations of B-Y-T-E according to architecture.
// Note that these are not calculated at compile time and have a CPU cost
#define		ES_IS_LITTLE_ENDIAN	( ((long)0x45545942) == *((long*)("BYTE")) )	// ETYB 3210
#define		ES_IS_BIG_ENDIAN	( ((long)0x42595445) == *((long*)("BYTE")) )	// BYTE 0123
#define		ES_IS_PDP_ENDIAN	( ((long)0x54454259) == *((long*)("BYTE")) )	// TEBY 2301

#define		EndianSwap(x,s,b)	( (b == false) ? x : (( s == 1 ? EndianSwap08(x) : ( s == 2 ? EndianSwap16(x) : ( s == 4 ? EndianSwap32(x) : (0x00000000) ) ) )))

#define		EndianSwap08(x)	( x )
#define		EndianSwap16(x)	( (( x & 0xFF00 ) >> 8) | (( x & 0x00FF ) << 8) )
#define		EndianSwap32(x)	( (( x & 0xFF000000 ) >> 24) | (( x & 0x00FF0000 ) >> 8) | (( x & 0x0000FF00 ) << 8) | (( x & 0x000000FF ) << 24) )

#endif /* __ENDIANSWAP__ */
