/*
File:       endianswap.c
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

#include "endianswap.h"

char	EndianSwapRuntime08(long endianSource,long endianDest,char value)
{
	long	usedSource = 0;
	long	usedDest = 0;
	
	usedSource = endianSource;
	usedDest = endianDest;
	
	return value;
}

short	EndianSwapRuntime16(long endianSource,long endianDest,short value)
{
	short swapValue = 0;
	
	if(endianSource == ES_NATIVE_ENDIAN)
	{
		if(ES_RUNTIME_IS_LITTLE_ENDIAN)
			endianSource = ES_LITTLE_ENDIAN;
		else if(ES_RUNTIME_IS_BIG_ENDIAN)
			endianSource = ES_BIG_ENDIAN;
		else if(ES_RUNTIME_IS_PDP_ENDIAN)
			endianSource = ES_PDP_ENDIAN;
		else
			endianSource = ES_UNKNOWN_ENDIAN;
	}
	
	if(endianDest == ES_NATIVE_ENDIAN)
	{
		if(ES_RUNTIME_IS_LITTLE_ENDIAN)
			endianDest = ES_LITTLE_ENDIAN;
		else if(ES_RUNTIME_IS_BIG_ENDIAN)
			endianDest = ES_BIG_ENDIAN;
		else if(ES_RUNTIME_IS_PDP_ENDIAN)
			endianDest = ES_PDP_ENDIAN;
		else
			endianDest = ES_UNKNOWN_ENDIAN;
	}
	
	switch(endianSource)
	{
	case ES_LITTLE_ENDIAN:
		if(endianDest == ES_BIG_ENDIAN) {
			swapValue = EndianSwapLtoB16(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapLtoP16(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	case ES_BIG_ENDIAN:
		if(endianDest == ES_LITTLE_ENDIAN) {
			swapValue = EndianSwapBtoL16(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapBtoP16(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	case ES_PDP_ENDIAN:
		if(endianDest == ES_LITTLE_ENDIAN) {
			swapValue = EndianSwapPtoL16(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapLtoB16(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	default:
		swapValue = swapValue;
		break;
	}
	
	return swapValue;
}

int	EndianSwapRuntime32(long endianSource,long endianDest,int value)
{
	short swapValue = 0;
	
	if(endianSource == ES_NATIVE_ENDIAN)
	{
		if(ES_RUNTIME_IS_LITTLE_ENDIAN)
			endianSource = ES_LITTLE_ENDIAN;
		else if(ES_RUNTIME_IS_BIG_ENDIAN)
			endianSource = ES_BIG_ENDIAN;
		else if(ES_RUNTIME_IS_PDP_ENDIAN)
			endianSource = ES_PDP_ENDIAN;
		else
			endianSource = ES_UNKNOWN_ENDIAN;
	}
	
	if(endianDest == ES_NATIVE_ENDIAN)
	{
		if(ES_RUNTIME_IS_LITTLE_ENDIAN)
			endianDest = ES_LITTLE_ENDIAN;
		else if(ES_RUNTIME_IS_BIG_ENDIAN)
			endianDest = ES_BIG_ENDIAN;
		else if(ES_RUNTIME_IS_PDP_ENDIAN)
			endianDest = ES_PDP_ENDIAN;
		else
			endianDest = ES_UNKNOWN_ENDIAN;
	}
	
	switch(endianSource)
	{
	case ES_LITTLE_ENDIAN:
		if(endianDest == ES_BIG_ENDIAN) {
			swapValue = EndianSwapLtoB32(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapLtoP32(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	case ES_BIG_ENDIAN:
		if(endianDest == ES_LITTLE_ENDIAN) {
			swapValue = EndianSwapBtoL32(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapBtoP32(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	case ES_PDP_ENDIAN:
		if(endianDest == ES_LITTLE_ENDIAN) {
			swapValue = EndianSwapPtoL32(swapValue);
		} else if(endianDest == ES_PDP_ENDIAN) {
			swapValue = EndianSwapLtoB32(swapValue);
		} else {
			swapValue = swapValue;
		}
		break;
	default:
		swapValue = swapValue;
		break;
	}
	
	return swapValue;
}
