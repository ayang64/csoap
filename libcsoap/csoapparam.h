/******************************************************************
 *  $Id: csoapparam.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
#ifndef CSOAP_PARAM_H
#define CSOAP_PARAM_H

#include "libcsoap/csoapxml.h"


typedef HSOAPXML HSOAPPARAM;


#define SOAP_MAX_STRING_BUFFER  1054


/**
 * Creates a string soap parameter
 *
 * <LI>Default type: "xsd:string"
 *
 * @param parent the parent param object
 * @param name the parameter name
 * @param format the parameter value 
 * 
 * @return handle to a parameter object if
 *  the creation was successfull, NULL otherwise
 */
HSOAPPARAM SoapParamCreateString( HSOAPPARAM parent, const char *name, 
				  const char *format,  ...);


/**
 * Creates an integer soap parameter
 *
 * <LI>Default type: "xsd:integer"
 *
 * @param parent the parent param object
 * @param name the parameter name
 * @param value the parameter value 
 * 
 * @return handle to a parameter object if
 *  the creation was successfull, NULL otherwise
 */
HSOAPPARAM SoapParamCreateInt( HSOAPPARAM parent, const char *name,
			       int value);


/**
 * Creates a double soap parameter
 *
 * <LI>Default type: "xsd:double"
 *
 * @param parent the parent param object
 * @param name the parameter name
 * @param value the parameter value 
 * 
 * @return handle to a parameter object if
 *  the creation was successfull, NULL otherwise
 */
HSOAPPARAM SoapParamCreateDouble( HSOAPPARAM parent, const char *name,
				  double value);


/**
 * Sets the parameter value. 
 *
 * @param param the parameter object
 * @param type a custom type name
 * @param format value of the parameter
 */
void SoapParamSetValue(HSOAPPARAM param,const char* type,
		       const char* format,  ...);


/**
 * Get the value of a soap parameter 
 * in string format
 *
 * @param param the soap parameter object
 * @param buffer an allocated character buffer
 *  to receive the content. If this is NULL,
 *  this function will return the size only.
 * 
 * @return size of the content.
 */
int SoapParamGetString(HSOAPPARAM param, char *buffer);   


/**
 * Get the value of a soap parameter
 *  in integer format
 * 
 * @param param the soap parameter object
 * @return the param content as integer
 */
int SoapParamGetInt(HSOAPPARAM param);


/**
 * Get the value of a soap parameter
 *  in double format
 * 
 * @param param the soap parameter object
 * @return the param content as double
 */
double SoapParamGetDouble(HSOAPPARAM param);


/**
 * Get the children parameter nodes.
 * This acts actually like an xml node.
 * 
 * @param param the soap parameter object
 * @return handle to the first child node
 */
HSOAPPARAM SoapParamGetChildren(HSOAPPARAM param);


/**
 * Get the next parameter.
 * This acts actually like an xml node.
 * 
 * @param param the soap parameter object
 * @return handle to the next node
 */
HSOAPPARAM SoapParamGetNext(HSOAPPARAM param);


/**
 * Checks if the type is equals with
 * the given one.
 *
 * @param param the param to check
 * @param type the type to check
 * 
 * @return 0 if the parameter type not equals to 'type'
 */
int SoapParamTypeIs(HSOAPPARAM param, const char* type);


/**
 * Returns parameters type.
 *
 * @param param the param to return its type
 * 
 * @return returns parameters type
 */
char* SoapParamGetType(HSOAPPARAM param);


/**
 * Creates a new parameter node.
 * This is used internally.
 *
 * @param parent the parent soap parameter object
 * @param type the parameter type
 * @param ns the namespace of the parameter
 * @param name name of the parameter
 * @param format value of the parameter
 *
 * @return A newly created soap parameter object
 */
HSOAPPARAM SoapParamCreate(HSOAPPARAM parent, const char *type,
			    const char *ns, const char *name,
			    const char *format,   ...);

#endif
