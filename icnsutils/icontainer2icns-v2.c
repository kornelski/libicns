/*
File:       icontainer2icns.cpp
Copyright (C) 2005 Thomas Lübking <baghira-style@gmx.net>
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
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
Boston, MA 02110-1301, USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Extract icns files from an icontainer archive
iContainer format is some kind binary object tree
It is likely encoded by the Mac OS X NSArchiver
which, in turn, uses Objective-C type encodings; see
http://gcc.gnu.org/onlinedocs/gcc-3.0.4/gcc_7.html

We are after the blocks of NSData containing the icns icons

Note: Doing this in C is a pretty poor implementation; it
would much better be done in some kind of Objective-C language.

The following is a VERY rough layout of the iContainer tree, version 3

NSMutableDictionary:NSDictionary:NSObject = (
{
  NSString "iconOrder" = NSMutableArray:NSArray(internal-unique-candyBar-ID-1,internal-unique-candyBar-ID-2,...)
  ContainerVersion = 3
  // May contain any of the following:
  CustomIcon = NSData {binary-icns-icon}
  AuthorName = Artiste De Icone
  AuthorEmail = atriste@artistes.org
  AuthorURL = http://icns.artistes.org/home/
  Copyright = 2010 Future Design Studio
  AuthoringLock = ????
  BackgroundColor = NSNumber:NSValue {array-of-floats} // NOTE: Content of NSValue unknown
  PreviewSize = NSNumber:NSValue {some-preview-size} // NOTE: Content of NSValue unknown
  // Still working on the following
  // Some kind of array/dictionary?
  {
    Name = iMovie
    Type = IcnsFile
    IcnsData = {Raw-icns-data} ??
    // May additionally contain some of the following:
    Author = Artiste De Icone
    URL = http://icns.artistes.org/home/
    Copyright = 2010 Future Design Studio
  }
  {
    ...
  }
}

Then, there also appears to be another tree, version 50 (0x32)
Reversal of this format is still a work in progress...

NSMutableDictionary:NSDictionary:NSObject = (
{
  NSString "Copyright" = 2010 Future Design Studio

  Icons = {
    internal-unique-candyBar-ID-6 = {
      Type = IcnsData
      Name = my-icon-name
      NSData {raw-icon-data}
    }
  }
}


*/

#define	ARCHIVE_TYPE_UNKNOWN	0
#define	ARCHIVE_TYPE_BE		1
#define	ARCHIVE_TYPE_LE		2

void decodeArchive(FILE *stream,char archiveTypeID)
{
	unsigned char	classDataBuf[256] = {0};
	char		spaces[512] = {0};
	int		s = 0;
	int		c = 0;
	int		d = 0;
	int		depth = 0;
	int		mark = 0;
	int		markSet = 0;

	int		nextArrayKey = 0;
	int		nextDataBuffer = 0;

	while ((c = getc(stream)) != EOF)
	{
		for(s = 0; s < depth; s++)
			spaces[s] = ' ';
		spaces[depth] = 0;
	
		if( c == 0x84 ) {
			if(markSet == 0) {
				mark = 0;
			}
			mark++;
			markSet++;
		} else {
			markSet = 0;
			if(d == 0x92) {
				printf("%sMissing Marker\n",spaces);
				depth-=2;
				spaces[depth] = 0;
				printf("%s}\n",spaces);
			}
		}

		if(c == 1 && nextDataBuffer == 0)
		{
			printf("%sGrab next byte marker...\n",spaces);
			c = getc(stream);
		}
		else if(c > 0x00 && c < 0x80)
		{
			printf("%sKey byte: %d (0x%02X)\n",spaces,c,c);
			if(nextArrayKey == 1)
			{
				printf("%sArray Key\n",spaces);
				c = getc(stream);
			}
			else if(mark > 1 || nextDataBuffer == 1)
			{
				printf("%sReading %d bytes of data...\n",spaces,c);
				fread ( &classDataBuf[0], sizeof(unsigned char), c, stream );
				classDataBuf[c] = 0;
				printf("%sData is '%s'\n",spaces,classDataBuf);

				// Go back and get a new character...
				continue;
			}
			else
			{
				printf("%sUnknown Key (mark <= 1)\n",spaces);
			}
		}
		
		nextArrayKey = 0;
		nextDataBuffer = 0;
		
		switch(c) {
			case 0x00:
				printf("%sPrevious data was Object/Class label\n",spaces);
				break;
			case '#': // 0x23 - class object
				printf("%sClass Object [UNHANDLED]\n",spaces);
				break;
			case '*': // 0x2A - character string
				printf("%sCharacter String [UNHANDLED]\n",spaces);
				break;
			case '+': // 0x2B - ?? (Related to strings)
				printf("%sCString to follow...\n",spaces);
				nextDataBuffer = 1;
				break;
			case '3': // 0x33 - ??
				printf("%sUnknown - 3 [UNHANDLED]\n",spaces);
				break;
			case ':': // 0x3A - method selector
				printf("%sMethod selector [UNHANDLED]\n",spaces);
				break;
			case '?': // 0x3F - unknown type
				printf("%sUnknown Type [UNHANDLED]\n",spaces);
				break;
			case '@': // 0x40 - object
				printf("%sObject\n",spaces);
				printf("%s{\n",spaces);
				break;
			case 'B': // 0x42 - bool
				printf("%sBool [UNHANDLED]\n",spaces);
				break;
			case 'C': // 0x43 - unsigned char
				{
					unsigned char	cVal = 0;
					cVal = getc(stream);
					printf("%sUnsigned Char: %c\n",spaces,cVal);
				}
				break;
			case 'I': // 0x49 - unsigned int
				printf("%sUnsigned Int [UNHANDLED]\n",spaces);
				break;
			case 'L': // 0x4C - unsigned long - note: 32 bits
				printf("%sUnsigned Long [UNHANDLED]\n",spaces);
				break;
			case 'O': // 0x4F - method - out
				printf("%sMethod Out [UNHANDLED]\n",spaces);
				break;
			case 'S': // 0x53 - unsigned short
				printf("%sUnsigned Short [UNHANDLED]\n",spaces);
				break;
			case 'Q': // 0x51 - unsigned long long
				printf("%sUnsigned Long [UNHANDLED]\n",spaces);
				break;
			case 'R': // 0x52 - method - byref
				printf("%sMethod ByRef [UNHANDLED]\n",spaces);
				break;
			case 'V': // 0x56 - method - one way
				printf("%sMethod One Way [UNHANDLED]\n",spaces);
				break;
			case '^': // 0x5E - pointer to type
				  // Note, followed by type?
				printf("%sPointer to Type [UNHANDLED]\n",spaces);
				break;
			case 'b': // 0x62 - bit field
				  // Note, followed by NUM bits?
				printf("%sBit Feild [UNHANDLED]\n",spaces);
				break;
			case 'c': // 0x63 - char
				{
					char	cVal = 0;
					cVal = getc(stream);
					printf("%sChar: %c\n",spaces,cVal);
				}
				break;
			case 'd': // 0x64 - double
				printf("%sDouble [UNHANDLED]\n",spaces);
				break;
			case 'f': // 0x66 - float
				printf("%sFloat\n",spaces);
				fread ( &classDataBuf[0], sizeof(unsigned char), 8, stream );
				break;
			case 'i': // 0x69 - integer
				{
					int	nVal = 0;
					nVal = getc(stream);
					printf("%sInteger: %d\n",spaces,nVal);
				}
				break;
			case 'n': // 0x6B - method - in
				printf("%sMethod In [UNHANDLED]\n",spaces);
				break;
			case 'o': // 0x6C - method - out
				printf("%sMethod Out [UNHANDLED]\n",spaces);
				break;
			case 'l': // 0x6C - long - 32 bits
				printf("%sLong [UNHANDLED]\n",spaces);
				break;
			case 'q': // 0x71 - long long
				printf("%sLong Long [UNHANDLED]\n",spaces);
				break;
			case 'r': // 0x72 - method - const
				printf("%sMethod Const [UNHANDLED]\n",spaces);
				break;
			case 's': // 0x73 - short
				printf("%sShort [UNHANDLED]\n",spaces);
				break;
			case 'v': // 0x76 - void
				printf("%sVoid [UNHANDLED]\n",spaces);
				break;


			case 0x81: // 2-byte length - data stream indicator
				{
					unsigned char	b[2] = {0,0};
					unsigned long	skip = 0;
					
					fread ( &b[0], sizeof(char), 2, stream );
					
					if(archiveTypeID == ARCHIVE_TYPE_BE) {
						skip = b[1]|b[0]<<8;
					}

					if(archiveTypeID == ARCHIVE_TYPE_LE) {
						skip = b[0]|b[1]<<8;
					}

					printf("\n%sMedium data stream of size: %ld\n\n",spaces,skip);

					// Actual Data buffer
					fseek ( stream, skip, SEEK_CUR );
				}
				break;
			case 0x82: // 4-byte length - data stream indicator
				{
					unsigned char	b[4] = {0,0,0,0};
					unsigned long	skip = 0;
					char		bufstr[24] = {0};
					int		idlen = 0;

					fread ( &b[0], sizeof(char), 4, stream );
					
					if(archiveTypeID == ARCHIVE_TYPE_BE) {
						skip = b[3]|b[2]<<8| b[1]<<16|b[0]<<24;
					}

					if(archiveTypeID == ARCHIVE_TYPE_LE) {
						skip = b[0]|b[1]<<8| b[2]<<16|b[3]<<24;
					}

					printf("\n%sLong data stream of size: %ld\n\n",spaces,skip);

					sprintf(&bufstr[0],"[%ldc]",skip);
					idlen = strlen(&bufstr[0]);

					c = getc( stream );

					if(c != 0x84) {
						printf("Read byte not found - expected 0x84 got 0x%02X\n",c);
					}

					c = getc( stream );

					if(c != idlen) {
						printf("Data label length mismatch - expected %d got %d\n",idlen,c);
					}

					// Character size lablel - should match bufstr (excluding NULL byte)
					fseek ( stream, idlen, SEEK_CUR );

					// Actual Data buffer
					fseek ( stream, skip, SEEK_CUR );
				}
				break;
			case 0x83:
				printf("%sRead 4 bytes ='0x%02X'\n",spaces,c);
				printf("%sUNKNOWN 4-BYTE VALUE\n",spaces);
				getc(stream);
				getc(stream);
				getc(stream);
				getc(stream);
				break;
			case 0x84:
				//printf("%sSection ='0x%02X'\n",spaces,c);
				printf("%s-mark- %d\n",spaces,mark);
				break;
			case 0x85:
				printf("%sObject S0 ='0x%02X'\n",spaces,c);
				depth+=2;
				break;
			case 0x86:
				depth-=2;
				spaces[depth] = 0;
				printf("%s}\n",spaces);
				printf("%sItem E: ='0x%02X'\n\n",spaces,c);
				break;
			case 0x92:
				printf("%sItem S1: ='0x%02X'\n",spaces,c);
				depth+=2;
				printf("%s{\n",spaces);
				break;
			case 0x93:
				printf("%sItem S2: '0x%02X'\n",spaces,c);
				break;
			case 0x95:
				printf("%sArray Start: ='0x%02X'\n",spaces,c);
				break;
			case 0x96:
				printf("%sArray Data: ='0x%02X'\n",spaces,c);
				nextArrayKey = 1;
				break;
			case 0x97:
				printf("%sArray Element: ='0x%02X'\n",spaces,c);
				break;
			case 0x98:
				printf("%sData Buffer: ='0x%02X'\n",spaces,c);
				nextDataBuffer = 1;
				break;
			default:
				printf("%sUNKNOWN: '0x%02X'\n",spaces,c);
				break;
		}

		d = c;
	}

}

int main(int argc, char **argv)
{
	FILE 		*icontainer = NULL;
	long		containerSize = 0;
	unsigned char	archiveVersion = 0;
	unsigned char	archiveFlags[2] = {0,0};
	char		archiveType[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	char		archiveTypeID = ARCHIVE_TYPE_UNKNOWN;
	unsigned char	archiveSysBytes[2] = {0,0};
	unsigned short	archiveSystem = 0;
	
	printf("icontainer2icns, (C) 2005-2008 by Thomas Lübking & Mathew Eis\n\n");

	if (argc < 2) {
		printf("\nusage: icontainer2icns foo.icontainer\n\n");
		return -1;
	}
	else if (argc > 2) {
		printf("\nusage: icontainer2icns foo.icontainer\nif your icontainer file contains spaces etc.,\nuse a system valid form (i.e. use \"my foo.icontainer\" or my\\ foo.icontainer)\n\n");
		return -1;
	}

	if( (icontainer = fopen(argv[1], "r")) == NULL ) {
		fprintf(stderr,"error while opening file %s\n",argv[1]);
		return -1;
	}

	printf("Loading iContainer...\n");
	
	// Get the containter size
	if(fseek(icontainer,0,SEEK_END) == 0)
	{
		containerSize = ftell(icontainer);
		rewind(icontainer);
	}
	else
	{
		fprintf(stderr,"Error reading archive size!\n");
		fclose(icontainer);
		return -1;
	}
	
	// Check the file size
	if(containerSize < 16)
	{
		fprintf(stderr,"Archive is too small!\n");
		fclose(icontainer);
		return -1;
	}

	// Read the header
	fread ( &archiveVersion, sizeof(unsigned char), 1, icontainer );
	fread ( &archiveFlags[0], sizeof(unsigned char), 1, icontainer );
	fread ( &archiveType[0], sizeof(char), 11, icontainer );
	fread ( &archiveFlags[1], sizeof(unsigned char), 1, icontainer );
	fread ( &archiveSysBytes[0], sizeof(unsigned char), 2, icontainer );

	if(memcmp(&archiveType[0],"streamtyped",11) == 0) {
		archiveTypeID = ARCHIVE_TYPE_LE;
		archiveSystem = archiveSysBytes[0]|archiveSysBytes[1]<< 8;
		printf("NSArchiver little endian archive, version %d, system %d\n",archiveVersion,archiveSystem);
	} else {
		if(memcmp( &archiveType[0], "typedstream", 11 ) != 0) {
			printf("Warning: Unable to determine archive endian - assuming big endian!");
		}
		archiveTypeID = ARCHIVE_TYPE_BE;
		archiveSystem = archiveSysBytes[1]|archiveSysBytes[0]<< 8;
		printf("NSArchiver big endian archive, version %d, system %d\n",archiveVersion,archiveSystem);
	}
	
	if(archiveVersion >= 5)
		printf("Warning: Archive stream version higher then expected - proceeding anyway!");
	
	if(archiveFlags[0] != 0x0B)
		printf("Warning: Byte 0  is 0x%02X (Expected 0x0B) - proceeding anyway!",archiveFlags[0]);
	
	if(archiveFlags[1] != 0x81)
		printf("Warning: Byte 13  is 0x%02X (Expected 0x1) - proceeding anyway!",archiveFlags[1]);

	decodeArchive(icontainer,archiveTypeID);
	
/*
		char	buffer[118];
		FILE	*icns = NULL;
		short	i = 0;
		char	name[256];

		if (c != 'i') {
			buffer[i] = (char)(c & 0xff);
			i = clamp(++i);
			if (icns)
			fputc(buffer[i], icns);
		}
		else
		{ // wow, lets test if this starts a new icns!
			if ((c = getc(icontainer)) == 'c') {
				if ((c = getc(icontainer)) == 'n') {
					if ((c = getc(icontainer)) == 's') { // yupp!
						if (icns)
							fclose(icns); // buffer only contains new icns info

						// generate the name
						// i - 7 .. i - 2 is the icontainer id
						// could be 56-4, so no memcpy please!
						//             printf("%s\n",buffer);
						short j;
						short k = 0;
						if (buffer[clamp(i-1)] == ']') {
							j = clamp(i-7);
							for (k = 0; k < 6; j = clamp(++j)) {
								name[k++] = buffer[j];
							}
							name[k++] = '-';
						}
						// now the name
						j = clamp(i+1);
						while (!isChar(buffer[j]) && j != i)
							j = clamp(++j);
						while (isChar(buffer[j]) && j != i) {
							name[k++] = buffer[j];
							j = clamp(++j);
						}
						name[k++] = '.'; name[k++] = 'i'; name[k++] = 'c';
						name[k++] = 'n'; name[k++] = 's'; name[k] = '\0';

						printf("extracting %s...\n",name);

						if( (icns = fopen(name, "w")) == NULL ) {
							printf("error while opening icns file %s\n", name);
							return -1;
						}

						rewind(icns);
						// write header
						fputc('i', icns); fputc('c', icns);
						fputc('n', icns); fputc('s', icns);
						// init buffer
						for (i = 0; i < 117; ++i)
							buffer[i] = (char)(getc(icontainer) & 0xff);
					}
					else { // reset
						ungetc(c, icontainer); ungetc('n', icontainer);
						ungetc('c', icontainer);
						buffer[i] = 'i'; i = clamp(++i);
						if (icns)
							fputc(buffer[i], icns);
					}
				}
				else { // reset
					ungetc(c, icontainer); ungetc('c', icontainer);
					buffer[i] = 'i'; i = clamp(++i);
					if (icns)
						fputc(buffer[i], icns);
				}
			}
			else { // reset
				ungetc(c, icontainer);
				buffer[i] = 'i'; i = clamp(++i);
				if (icns)
					fputc(buffer[i], icns);
			}
		}
*/

	fclose(icontainer);
	return 0;
}


