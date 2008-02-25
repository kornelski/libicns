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

// Add any other +CROSS-PLATFORM+ headers here that might define
// ES_ARCH_BYTE_ORDER, ES_BIG_ENDIAN, ES_LITTLE_ENDIAN, ES_PDP_ENDIAN
#ifdef __APPLE__
 #include <machine/endian.h>
#endif

#ifdef __WIN32__
 #include <sys/param.h>
#endif

#include <stdlib.h>

#ifndef __ENDIANSWAP__
#define __ENDIANSWAP__

// This should be defined in the makefile
#define ES_DEBUG

// Notes on endianess. Endianess has to the with the order of significant
// bytes in a numerical data type. Significant meaning the part that will
// impact the number the most. To clarify, with the number 0x12345678, 1 
// is the most significant part of the number. In a processor, multi-byte
// numbers are stored in a certian way. On a Power PC processor, for example
// The number 0x12345678 would actually be stored [12] [34] [56] [78].
// This is called big endian. On an Intel processor, however, the same number
// would be stored as [78] [56] [34] [12]. The number is still 0x12345678
// but this is how it is stored. Other processors, may use other
// byte ordering methods; PDP-11 would be stored as [56] [78] [12] [34].

                                                    //  0   1   2   3
#define		ES_BIG_ENDIAN           0x42595445  // [B] [Y] [T] [E]
#define		ES_LITTLE_ENDIAN        0x45545942  // [E] [T] [Y] [B]
#define		ES_PDP_ENDIAN           0x59424554  // [Y] [B] [E] [T]

#define		ES_ORDERED_BYTE		(*((unsigned long*)("BYTE")))

#define		ES_NATIVE_ENDIAN        0x0123ABCD

#define		ES_UNKNOWN_ENDIAN       0x00000000

// Macros for detecting endianness. First we try to use the compiler
// defined byte order - otherwise, the compiler stores string constants as
// chars, so B-Y-T-E will be in order. We can then check against 4-byte longs
// that have various iterations of B-Y-T-E according to architecture.
// Note that these are not calculated at compile time and have a CPU cost

#define		ES_RUNTIME_IS_BIG_ENDIAN    ( ES_BIG_ENDIAN == ES_ORDERED_BYTE )
#define		ES_RUNTIME_IS_LITTLE_ENDIAN ( ES_LITTLE_ENDIAN == ES_ORDERED_BYTE )
#define		ES_RUNTIME_IS_PDP_ENDIAN    ( ES_PDP_ENDIAN == ES_ORDERED_BYTE )

// The basic macros for swapping bytes between types
// Macros are specified in the following way:
// EndianSwap[SRC]to[DEST][SIZE]
// i.e. EndianSwapBtoL32 will swap from a 32-byte big endian to little endian
// i.e. EndianSwapLtoP16 will swap from a 16-byte little endian to php endian

// One byte value: we don't need to swap in any case
#define		EndianSwapBtoL08(x)       ( x )
#define		EndianSwapLtoB08(x)       ( x )
#define		EndianSwapBtoP08(x)       ( x )
#define		EndianSwapPtoB08(x)       ( x )
#define		EndianSwapLtoP08(x)       ( x )
#define		EndianSwapPtoL08(x)       ( x )

// Two byte value: a simple swap should work for all endians
#define		EndianSwapBtoL16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )
#define		EndianSwapLtoB16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )
#define		EndianSwapBtoP16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )
#define		EndianSwapPtoB16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )
#define		EndianSwapLtoP16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )
#define		EndianSwapPtoL16(x)       ( (( (x) & 0xFF00 ) >> 8) | (( (x) & 0x00FF ) << 8) )

// Four-byte-value: BIG ENDIAN <-> LITTLE ENDIAN (Works the same both ways)
#define		EndianSwapBtoL32(x)       ( (( (x) & 0xFF000000 ) >> 24) | (( (x) & 0x00FF0000 ) >>  8) | (( (x) & 0x0000FF00 ) <<  8) | (( (x) & 0x000000FF ) << 24) )
#define		EndianSwapLtoB32(x)       ( (( (x) & 0xFF000000 ) >> 24) | (( (x) & 0x00FF0000 ) >>  8) | (( (x) & 0x0000FF00 ) <<  8) | (( (x) & 0x000000FF ) << 24) )

// Four-byte-value: BIG ENDIAN <-> PDP ENDIAN (Works the same both ways)
#define		EndianSwapBtoP32(x)       ( (( (x) & 0xFF000000 ) >>  8) | (( (x) & 0x00FF0000 ) <<  8) | (( (x) & 0x0000FF00 ) >>  8) | (( (x) & 0x000000FF ) <<  8) )
#define		EndianSwapPtoB32(x)       ( (( (x) & 0xFF000000 ) >>  8) | (( (x) & 0x00FF0000 ) <<  8) | (( (x) & 0x0000FF00 ) >>  8) | (( (x) & 0x000000FF ) <<  8) )

// Four-byte-value: LITTLE ENDIAN <-> PDP ENDIAN (Works the same both ways)
#define		EndianSwapLtoP32(x)       ( (( (x) & 0xFF000000 ) >> 16) | (( (x) & 0x00FF0000 ) >> 16) | (( (x) & 0x0000FF00 ) << 16) | (( (x) & 0x000000FF ) << 16) )
#define		EndianSwapPtoL32(x)       ( (( (x) & 0xFF000000 ) >> 16) | (( (x) & 0x00FF0000 ) >> 16) | (( (x) & 0x0000FF00 ) << 16) | (( (x) & 0x000000FF ) << 16) )

// Try hard to determine endianess at compile time. __BYTE_ORDER is defined
// on most distributions, along with __BIG_ENDIAN and __LITTLE_ENDIAN, but
// on other platforms some others are defined We work together to establish
// something consistent

#ifdef __BYTE_ORDER
	#define ES_ARCH_BYTE_ORDER                   __BYTE_ORDER

	#ifdef __LITTLE_ENDIAN
		#define ES_ARCH_LITTLE_ENDIAN        __LITTLE_ENDIAN
	#endif
	#ifdef __BIG_ENDIAN
		#define ES_ARCH_BIG_ENDIAN           __BIG_ENDIAN
	#endif
	#ifdef __PDP_ENDIAN
		#define ES_ARCH_PDP_ENDIAN           __PDP_ENDIAN
	#endif
#endif

#ifdef __BYTE_ORDER__
	#ifndef ES_ARCH_BYTE_ORDER
	        #define ES_ARCH_BYTE_ORDER                   __BYTE_ORDER__
	#endif

	#ifdef __LITTLE_ENDIAN__
		#ifndef ES_ARCH_LITTLE_ENDIAN
			#define ES_ARCH_LITTLE_ENDIAN        __LITTLE_ENDIAN__
		#endif
	#endif
	#ifdef __BIG_ENDIAN__
		#ifndef ES_ARCH_BIG_ENDIAN
			#define ES_ARCH_BIG_ENDIAN           __BIG_ENDIAN__
		#endif
	#endif
	#ifdef __PDP_ENDIAN__
		#ifndef ES_ARCH_PDP_ENDIAN
			#define ES_ARCH_PDP_ENDIAN           __PDP_ENDIAN__
		#endif
	#endif
#endif

#ifdef BYTE_ORDER
	#ifndef ES_ARCH_BYTE_ORDER
	        #define ES_ARCH_BYTE_ORDER                   BYTE_ORDER
	#endif

	#ifdef LITTLE_ENDIAN
		#ifndef ES_ARCH_LITTLE_ENDIAN
			#define ES_ARCH_LITTLE_ENDIAN        LITTLE_ENDIAN
		#endif
	#endif
	#ifdef BIG_ENDIAN
		#ifndef ES_ARCH_BIG_ENDIAN
			#define ES_ARCH_BIG_ENDIAN           BIG_ENDIAN
		#endif
	#endif
	#ifdef PDP_ENDIAN
		#ifndef ES_ARCH_PDP_ENDIAN
			#define ES_ARCH_PDP_ENDIAN           PDP_ENDIAN
		#endif
	#endif
#endif

// If we were able to resolve the byte order in some way, then
// Go ahead and use the compile-time macros for endian swaps
#ifdef ES_ARCH_BYTE_ORDER
	#if ES_ARCH_BYTE_ORDER==ES_ARCH_BIG_ENDIAN
		#define ES_IS_BIG_ENDIAN      (1)
		#define ES_IS_LITTLE_ENDIAN   (0)
		#define ES_IS_PDP_ENDIAN      (0)
		#define	EndianSwapNtoB08(x)   (x)
		#define	EndianSwapNtoB16(x)   (x)
		#define	EndianSwapNtoB32(x)   (x)
		#define	EndianSwapBtoN08(x)   (x)
		#define	EndianSwapBtoN16(x)   (x)
		#define	EndianSwapBtoN32(x)   (x)
		#define	EndianSwapNtoL08(x)   EndianSwapLtoB08(x)
		#define	EndianSwapNtoL16(x)   EndianSwapLtoB16(x)
		#define	EndianSwapNtoL32(x)   EndianSwapLtoB32(x)
		#define	EndianSwapLtoN08(x)   EndianSwapBtoL08(x)
		#define	EndianSwapLtoN16(x)   EndianSwapBtoL16(x)
		#define	EndianSwapLtoN32(x)   EndianSwapBtoL32(x)
		#define	EndianSwapNtoP08(x)   EndianSwapPtoB08(x)
		#define	EndianSwapNtoP16(x)   EndianSwapPtoB16(x)
		#define	EndianSwapNtoP32(x)   EndianSwapPtoB32(x)
		#define	EndianSwapPtoN08(x)   EndianSwapBtoP08(x)
		#define	EndianSwapPtoN16(x)   EndianSwapBtoP16(x)
		#define	EndianSwapPtoN32(x)   EndianSwapBtoP32(x)
	#elif ES_ARCH_BYTE_ORDER==ES_ARCH_LITTLE_ENDIAN
		#define ES_IS_BIG_ENDIAN      (0)
		#define ES_IS_LITTLE_ENDIAN   (1)
		#define ES_IS_PDP_ENDIAN      (0)
		#define	EndianSwapNtoB08(x)   EndianSwapBtoL08(x)
		#define	EndianSwapNtoB16(x)   EndianSwapBtoL16(x)
		#define	EndianSwapNtoB32(x)   EndianSwapBtoL32(x)
		#define	EndianSwapBtoN08(x)   EndianSwapLtoB08(x)
		#define	EndianSwapBtoN16(x)   EndianSwapLtoB16(x)
		#define	EndianSwapBtoN32(x)   EndianSwapLtoB32(x)
		#define	EndianSwapNtoL08(x)   (x)
		#define	EndianSwapNtoL16(x)   (x)
		#define	EndianSwapNtoL32(x)   (x)
		#define	EndianSwapLtoN08(x)   (x)
		#define	EndianSwapLtoN16(x)   (x)
		#define	EndianSwapLtoN32(x)   (x)
		#define	EndianSwapNtoP08(x)   EndianSwapPtoL08(x)
		#define	EndianSwapNtoP16(x)   EndianSwapPtoL16(x)
		#define	EndianSwapNtoP32(x)   EndianSwapPtoL32(x)
		#define	EndianSwapPtoN08(x)   EndianSwapLtoP08(x)
		#define	EndianSwapPtoN16(x)   EndianSwapLtoP16(x)
		#define	EndianSwapPtoN32(x)   EndianSwapLtoP32(x)
	#elif ES_ARCH_BYTE_ORDER==ES_ARCH_PDP_ENDIAN
		#define "order is pdp"
		#define ES_IS_BIG_ENDIAN      (0)
		#define ES_IS_LITTLE_ENDIAN   (0)
		#define ES_IS_PDP_ENDIAN      (1)
		#define	EndianSwapNtoB08(x)   EndianSwapBtoP08(x)
		#define	EndianSwapNtoB16(x)   EndianSwapBtoP16(x)
		#define	EndianSwapNtoB32(x)   EndianSwapBtoP32(x)
		#define	EndianSwapBtoN08(x)   EndianSwapPtoB08(x)
		#define	EndianSwapBtoN16(x)   EndianSwapPtoB16(x)
		#define	EndianSwapBtoN32(x)   EndianSwapPtoB32(x)
		#define	EndianSwapNtoL08(x)   EndianSwapLtoP08(x)
		#define	EndianSwapNtoL16(x)   EndianSwapLtoP16(x)
		#define	EndianSwapNtoL32(x)   EndianSwapLtoP32(x)
		#define	EndianSwapLtoN08(x)   EndianSwapPtoL08(x)
		#define	EndianSwapLtoN16(x)   EndianSwapPtoL16(x)
		#define	EndianSwapLtoN32(x)   EndianSwapPtoL32(x)
		#define	EndianSwapNtoP08(x)   (x)
		#define	EndianSwapNtoP16(x)   (x)
		#define	EndianSwapNtoP32(x)   (x)
		#define	EndianSwapPtoN08(x)   (x)
		#define	EndianSwapPtoN16(x)   (x)
		#define	EndianSwapPtoN32(x)   (x)
	#else
		#define ES_UNDETERMINED_ENDIAN
	#endif
#else
	#define ES_UNDETERMINED_ENDIAN
#endif /* ifdef ES_ARCH_BYTE_ORDER */

#ifdef ES_UNDETERMINED_ENDIAN
	#warning "Unable to determine endianess at compile time. Forced to use runtime checks!"
	#define ES_IS_BIG_ENDIAN      ES_RTIME_IS_BIG_ENDIAN
	#define ES_IS_LITTLE_ENDIAN   ES_RTIME_IS_LITTLE_ENDIAN
	#define ES_IS_PDP_ENDIAN      ES_RTIME_IS_PDP_ENDIAN
	
	#warning "Unable to determine endianess at compile time. Forced to use runtime endian swaps!"
	#define	EndianSwapNtoB08(x)   EndianSwapRuntime08(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapNtoB16(x)   EndianSwapRuntime16(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapNtoB32(x)   EndianSwapRuntime32(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapBtoN08(x)   EndianSwapRuntime08(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapBtoN16(x)   EndianSwapRuntime16(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapBtoN32(x)   EndianSwapRuntime32(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapNtoL08(x)   EndianSwapRuntime08(ES_NATIVE_ENDIAN,ES_LITTLE_ENDIAN,x)
	#define	EndianSwapNtoL16(x)   EndianSwapRuntime16(ES_NATIVE_ENDIAN,ES_LITTLE_ENDIAN,x)
	#define	EndianSwapNtoL32(x)   EndianSwapRuntime32(ES_NATIVE_ENDIAN,ES_LITTLE_ENDIAN,x)
	#define	EndianSwapLtoN08(x)   EndianSwapRuntime08(ES_LITTLE_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapLtoN16(x)   EndianSwapRuntime16(ES_LITTLE_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapLtoN32(x)   EndianSwapRuntime32(ES_LITTLE_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapNtoP08(x)   EndianSwapRuntime08(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapNtoP16(x)   EndianSwapRuntime16(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapNtoP32(x)   EndianSwapRuntime32(ES_NATIVE_ENDIAN,ES_BIG_ENDIAN,x)
	#define	EndianSwapPtoN08(x)   EndianSwapRuntime08(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapPtoN16(x)   EndianSwapRuntime16(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
	#define	EndianSwapPtoN32(x)   EndianSwapRuntime32(ES_BIG_ENDIAN,ES_NATIVE_ENDIAN,x)
#endif /* ifdef ES_UNKNOWN_ENDIAN */

// Convenience macros; these will have a slight additional runtime cost
#define EndianSwapNtoB(x,s) ( s == 1 ? EndianSwapNtoB08(x) : ( s == 2 ? EndianSwapNtoB16(x) : ( s == 4 ? EndianSwapNtoB32(x) : (0) ) ) )
#define EndianSwapBtoN(x,s) ( s == 1 ? EndianSwapBtoN08(x) : ( s == 2 ? EndianSwapBtoN16(x) : ( s == 4 ? EndianSwapBtoN32(x) : (0) ) ) )
#define EndianSwapNtoL(x,s) ( s == 1 ? EndianSwapNtoL08(x) : ( s == 2 ? EndianSwapNtoL16(x) : ( s == 4 ? EndianSwapNtoL32(x) : (0) ) ) )
#define EndianSwapLtoN(x,s) ( s == 1 ? EndianSwapLtoN08(x) : ( s == 2 ? EndianSwapLtoN16(x) : ( s == 4 ? EndianSwapLtoN32(x) : (0) ) ) )
#define EndianSwapNtoP(x,s) ( s == 1 ? EndianSwapNtoP08(x) : ( s == 2 ? EndianSwapNtoP16(x) : ( s == 4 ? EndianSwapNtoP32(x) : (0) ) ) )
#define EndianSwapPtoN(x,s) ( s == 1 ? EndianSwapPtoN08(x) : ( s == 2 ? EndianSwapPtoN16(x) : ( s == 4 ? EndianSwapPtoN32(x) : (0) ) ) )


// Runtime functions for when we have no choice
char	EndianSwapRuntime08(long endianSource,long endianDest,char value);
short	EndianSwapRuntime16(long endianSource,long endianDest,short value);
int	EndianSwapRuntime32(long endianSource,long endianDest,int value);

#endif /* __ENDIANSWAP__ */
