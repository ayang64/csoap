/******************************************************************
 *  $Id: formatter.h,v 1.1 2004/06/02 11:17:03 snowdrop Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2003  Ferhat Ayaz
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
 * Boston, MA  02111-1307, USA.
 * 
 * Email: ayaz@jprogrammet.net
 ******************************************************************/
#ifndef XSD2C_FORMATTER_H
#define XSD2C_FORMATTER_H

#include "obj.h"

#include <stdio.h>

void writeComplexTypeHeaderFile(FILE* f, HCOMPLEXTYPE obj);
void writeComplexTypeSourceFile(FILE* f, HCOMPLEXTYPE obj);

extern int formatter_generate_sax_serializer;

#endif
