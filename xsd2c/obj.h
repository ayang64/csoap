/******************************************************************
 *  $Id: obj.h,v 1.1 2004/06/02 11:17:03 snowdrop Exp $
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
#ifndef XSD2C_OBJ_H
#define XSD2C_OBJ_H


typedef struct FIELD* HFIELD;
typedef struct COMPLEXTYPE* HCOMPLEXTYPE;
typedef int (*CT_ENUM)(HCOMPLEXTYPE);

struct FIELD
{
  char *name;
  char *type;
  int flag;
  int minOccurs;
  int maxOccurs;
  char *attrName;
  HFIELD next;
  HCOMPLEXTYPE parentObj;
};

struct COMPLEXTYPE
{
  char *type;
  char *base_type;
  HFIELD head;
  HFIELD tail;
  HCOMPLEXTYPE next;
};


void objInitModule();
void objFreeModule();

HCOMPLEXTYPE objCreateComplexType(const char* typename);
void objSetBaseType(HCOMPLEXTYPE obj, const char* typename);
HFIELD objAddElement(HCOMPLEXTYPE obj, const char* name, const char* type, int flag, int mino, int maxo);
HFIELD objAddAttribute(HCOMPLEXTYPE obj, const char* name, const char* type, int flag);

HCOMPLEXTYPE objRegistryGetComplexType(const char* typename);
void objRegistryEnumComplexType(CT_ENUM callback);

#endif
