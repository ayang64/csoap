/******************************************************************
 *  $Id: csoapfault.h,v 1.1 2003/11/12 13:22:58 snowdrop Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#ifndef CSOAP_FAULT_H
#define CSOAP_FAULT_H


#include "libcsoap/csoapxml.h"
#include "libcsoap/csoapparam.h"

typedef HSOAPXML HSOAPFAULT;


/**
 * Returns the [faultcode]
 *
 * @param fault handle to a soap fault object
 * @return an allocated string or NULL 
 * if no [faultcode] was set.
 */
char* SoapFaultGetFaultCode(HSOAPFAULT fault);			    


/**
 * Returns the [faultstring]
 *
 * @param fault handle to a soap fault object
 * @return an allocated string or NULL 
 * if no [faultstring] was set.
 */
char* SoapFaultGetFaultString(HSOAPFAULT fault);			    


/**
 * Returns the [faultactor]
 *
 * @param fault handle to a soap fault object
 * @return an allocated string or NULL 
 * if no [faultactor] was set.
 */
char* SoapFaultGetFaultActor(HSOAPFAULT fault);			    


/**
 * Returns the [detail]
 *
 * @param fault handle to a soap fault object
 * @return handle to a soap param object
 */
HSOAPPARAM SoapFaultGetDetail(HSOAPFAULT fault);			    


/**
 * Sets the a [faultcode] field.
 * This will be called internally.
 *
 * @param fault  handle to a soap fault object
 * @param faultcode the faultcode to set
 */
void SoapFaultSetFaultCode(HSOAPFAULT fault, const char* faultCode);


/**
 * Sets the a [faultstring] field.
 * This will be called internally.
 *
 * @param fault  handle to a soap fault object
 * @param faultstring the faultstring to set
 */
void SoapFaultSetFaultString(HSOAPFAULT fault, const char* faultString);


/**
 * Sets the a [faultactor] field.
 * This will be called internally.
 *
 * @param fault  handle to a soap fault object
 * @param faultactor the faultactor to set
 */
void SoapFaultSetFaultActor(HSOAPFAULT fault, const char* faultActor);


/**
 * Sets the a [faultdetail] field.
 * This will be called internally.
 *
 * @param fault  handle to a soap fault object
 * @param faultcode the detail to set
 */
void SoapFaultSetFaultDetail(HSOAPFAULT fault, HSOAPPARAM param);


/**
 * Prints a fault object into the
 * given file. This is for debug puposes.
 *
 * @param f the file to print into
 * @param fault the fault object to print
 */
void SoapFaultPrint(FILE* f, HSOAPFAULT fault);


#endif

