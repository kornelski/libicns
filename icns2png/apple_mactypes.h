/*
File:       apple_mactypes.h

This file is based on information found in the file MacTypes.h
originating in the 2002, Quicktime 6.0.2 developer's kit
as developed by Apple Computer, Inc.

Modifications have been made to make it more compatible with the
GNU gcc and g++ compilers, and to limit the information to that
necessary for the icns format. To be clear, bug reports on this
file should NOT be filed to Apple Computer, Inc. This is a work
in derivative, not the original work.

All data types and naming conventions in this file were created and
Copyright: (c) 1985-2001 by Apple Computer, Inc., all rights reserved.

*/

#ifndef __APPLE_MACTYPES__
#define	__APPLE_MACTYPES__	1

#define FOUR_CHAR_CODE(x)           	(unsigned long)(x)

typedef unsigned char                   UInt8;
typedef signed char                     SInt8;
typedef unsigned short                  UInt16;
typedef signed short                    SInt16;
typedef unsigned long                   UInt32;
typedef signed long                     SInt32;

typedef char *                          Ptr;
typedef Ptr *                           Handle;
typedef long                            Size;

typedef unsigned long                   FourCharCode;
typedef FourCharCode                    OSType;
typedef FourCharCode                    ResType;

typedef	OSType *			OSTypePtr;

#endif

