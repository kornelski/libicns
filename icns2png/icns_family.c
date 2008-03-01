/*
File:       icns_family.c
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
#include "icns_internals.h"

/***************************** icns_create_family **************************/

int icns_create_family(icns_family_t **iconFamilyOut)
{
	icns_family_t	*newIconFamily = NULL;
	icns_type_t		iconFamilyType = ICNS_NULL_TYPE;
	icns_size_t		iconFamilySize = 0;

	if(iconFamilyOut == NULL)
	{
		icns_print_err("icns_create_family: icon family reference is NULL!\n");
		return ICNS_STATUS_NULL_PARAM;
	}
	
	*iconFamilyOut = NULL;
	
	iconFamilyType = ICNS_FAMILY_TYPE;
	iconFamilySize = sizeof(icns_type_t) + sizeof(icns_size_t);

	newIconFamily = malloc(iconFamilySize);
		
	if(newIconFamily == NULL)
	{
		icns_print_err("icns_create_family: Unable to allocate memory block of size: %d!\n",iconFamilySize);
		return ICNS_STATUS_NO_MEMORY;
	}
	
	ICNS_WRITE_UNALIGNED(&(newIconFamily->resourceType), iconFamilyType, icns_type_t);
	ICNS_WRITE_UNALIGNED(&(newIconFamily->resourceSize), iconFamilySize, icns_size_t);

	*iconFamilyOut = newIconFamily;
	
	return ICNS_STATUS_OK;
}


