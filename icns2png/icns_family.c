/*
File:       icns.cpp
Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
              2007 Thomas Lübking <thomas.luebking@web.de>
              2002 Chenxiao Zhao <chenxiao.zhao@gmail.com>

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
#include <stdint.h>
#include <string.h>

#include "icns.h"

#include "endianswap.h"

/***************************** icns_create_family **************************/

int icns_create_family(icns_family_t **icnsFamilyOut)
{
	icns_family_t	*newIcnsFamily = NULL;
	icns_size_t	newIcnsFamilySize = 0;

	if(icnsFamilyOut == NULL)
	{
		fprintf(stderr,"libicns: icns_create_family: icns family reference is NULL!\n");
		return -1;
	}
	
	*icnsFamilyOut = NULL;
	
	newIcnsFamilySize = sizeof(icns_type_t) + sizeof(icns_size_t);

	newIcnsFamily = malloc(newIcnsFamilySize);
		
	if(newIcnsFamily == NULL)
	{
		fprintf(stderr,"libicns: icns_create_family: Unable to allocate memory block of size: %d!\n",newIcnsFamilySize);
		return -1;
	}
	
	newIcnsFamily->resourceType = EndianSwapNtoB(ICNS_FAMILY_TYPE,sizeof(icns_type_t));
	newIcnsFamily->resourceSize = EndianSwapNtoB(newIcnsFamilySize,sizeof(icns_size_t));
	
	*icnsFamilyOut = newIcnsFamily;
	
	return 0;
}

