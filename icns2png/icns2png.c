/*
File:       icns2png.cpp
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

#include "apple_mactypes.h"
#include "apple_icons.h"
#include "iconvert.h"
#include "pngwriter.h"

bool ConvertIcnsFile(char *filename);

int main(int argc,char *argv[])
{
	int count;

	printf("Icon2PNG icns conversion\n");

	if(argc < 2)
	{
		printf("Usage: icns2png input.icns\n");
	}
	else
	{
		for(count = 1; count < argc; count++)
		{
			if(ConvertIcnsFile(argv[count]))
				printf("Conversion of %s failed!\n",argv[count]);
		}
	}
	
	return 0;
}

bool ConvertIcnsFile(char *filename)
{
	bool				error = false;
	char				*infilename;
	char				*outfilename;
	int					filenamelength = 0;
	int					infilenamelength = 0;
	int					outfilenamelength = 0;
	IconFamilyPtr		iconFamily = NULL;
	IconImage			iconImage; iconImage.iconData = NULL;
	IconImage			maskImage; maskImage.iconData = NULL;
	bool				byteSwap = false;
	FILE				*outfile = NULL;

	filenamelength = strlen(filename);

	infilenamelength = filenamelength;
	outfilenamelength = filenamelength;

	// Create a buffer for the input filename
	infilename = (char *)malloc(infilenamelength+1);

	// Copy the input filename into the new buffer
	strncpy(infilename,filename,infilenamelength+1);
	infilename[filenamelength] = 0;

	// See if we can find a '.'
	while(infilename[outfilenamelength] != '.' && outfilenamelength > 0)
		outfilenamelength--;

	// Caculate new filename length
	if(outfilenamelength == 0)
		outfilenamelength = strlen(filename);
	outfilenamelength += 4;

	// Create a buffer for the output filename
	outfilename = (char *)malloc(outfilenamelength+1);

	// Yes, the length should be infilenamelength+1 - we are copying the
	// string from the imput filename.
	strncpy(outfilename,filename,infilenamelength+1);

	// Add the .png extension to the filename
	outfilename[outfilenamelength-4] = '.';
	outfilename[outfilenamelength-3] = 'p';
	outfilename[outfilenamelength-2] = 'n';
	outfilename[outfilenamelength-1] = 'g';
	outfilename[outfilenamelength-0] = 0;

	printf("Converting %s to %s...\n",infilename,outfilename);
	
	// The next three functions are the big workhorses

	// ReadXIcon converts the file into an IconFamily
	error = ReadXIconFile(infilename,&iconFamily);
	
	// This one converts the IconFamily into raw image data
	if(!error)
		error = GetIconDataFromIconFamily(iconFamily,kThumbnail32BitData,&iconImage,&byteSwap);
	
	// This one converts the IconFamily into raw mask data
	if(!error)
		error = GetIconDataFromIconFamily(iconFamily,kThumbnail8BitMask,&maskImage,&byteSwap);
	
	if(iconImage.iconData != NULL)
	{
		outfile = fopen(outfilename,"w");
		if(outfile != NULL)
		{
			error = WritePNGImage(outfile,&iconImage,&maskImage);
		}
		else
		{
			fprintf(stderr,"Error opening %s for writing!\n",outfilename);
			error = true;
		}
		fclose(outfile);
	}
	else
	{
		fprintf(stderr,"Icon image is NULL!\n");
	}

	free(maskImage.iconData);
	free(iconImage.iconData);
	free(iconFamily);
	free(infilename);
	free(outfilename);
	
	return error;
}
