/******************************************************************
 *  $Id: soap-env.h,v 1.4 2004/05/14 09:27:52 snowdrop Exp $
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
#ifndef cSOAP_ENV_H
#define cSOAP_ENV_H

#include <libcsoap/soap-xml.h>
#include <libcsoap/soap-fault.h>


/**
   The SOAP envelope object. 
 */
typedef struct _SoapEnv
{ 
  xmlNodePtr root; /** Pointer to the firts xml element (envelope) */
  xmlNodePtr cur; /** Pointer to the current xml element. (stack) */
}SoapEnv;


/* -------------------------------------------------------------- */
/*  Envelope creation methods                                     */
/* -------------------------------------------------------------- */

/**
   Creates an envelope with a fault object. 

   @param faultcode The fault code @see fault_code_t
   @param faultstring A fault message
   @param faultactor The fault actor (This can be NULL)
   @param detail The detail of the error (This can be NULL)

   @returns A Soap envelope object like follows 

   <pre>
   <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
        xmlns:xsi="..."
        xmlns:xsd="...">
      <SOAP-ENV:Body>

	 <Fault>
	  <faultcode>...</faultcode>
	  <faultstring>...</faultstring>
	  <faultactor>...</faultactor>
	  <faultdetail>..</faultdetail>
	 </Fault>

      </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
   
   </pre>

 */
SoapEnv *soap_env_new_with_fault(fault_code_t faultcode, 
				 const char *faultstring,
				 const char *faultactor,
				 const char *detail);

/**
   Creates an envelope with a method to invoke a soap service.
   Use this function to create a client call.

   @param urn The urn of the soap service to invoke
   @param method The method name of the soap service

   @returns A Soap envelope object like follows 

   <pre>
   <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
        xmlns:xsi="..."
        xmlns:xsd="...">
      <SOAP-ENV:Body>

       <m:[method] xmlns:m="[urn]">
       </m:[method]>

      </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
   
   </pre>

 */
SoapEnv *soap_env_new_with_method(const char *urn, const char *method);


/**
   Creates a soap envelope with a response. 
   Use this function to create a response envelope object
   for a request. This function is only relevant for soap 
   service implementors. 

   @see example csoap/simpleserver.c

   @param req The request object. A response object will be created
    to this request.

   @returns A Soap envelope object like follows 

   <pre>
   <SOAP-ENV:Envelope xmlns:SOAP-ENV="..." SOAP-ENV:encoding="..."
        xmlns:xsi="..."
        xmlns:xsd="...">
      <SOAP-ENV:Body>

       <m:[req-method]Response xmlns:m="[req-urn]">
       </m:[req-method]>

      </SOAP-ENV:Body>
   </SOAP-ENV:Envelope>
   
   </pre>


 */
SoapEnv *soap_env_new_with_response(SoapEnv *req);


/**
   Creates an envelope from a given libxml2 xmlDoc 
   pointer.

   @param doc the xml document pointer
   @returns A Soap envelop object if success, 
    NULL otherwise.
 */
SoapEnv *soap_env_new_from_doc(xmlDocPtr doc);


/**
   Create an envelop object from a string. 
   The string must be in xml format.

   @param buffer The string to parse into a envelope.
   @returns A soap envelope object if success or
    NULL if the string can not be parsed or the string
    does not represent an soap envelope in xml format.
 */
SoapEnv *soap_env_new_from_buffer(const char* buffer);



/* ------------------------------------------------------ */
/*     XML build and stack function                       */
/* ------------------------------------------------------ */


/**
   Adds a new xml node under the current parent.

   <pre>
    <m:[name] type=[type]>[value]</m:[name]>
   </pre>

   @param env The envelope object 
   @param type Type of the parameter. Something like "xsd:string" or
    "xsd:int" or custom types. 
   @param name Name of the xml node
   @param value Text value of the xml node

   @returns The added xmlNode pointer.
   
   @see tutorial 
 */
xmlNodePtr 
soap_env_add_item(SoapEnv* env, const char *type, 
		  const char *name, const char *value);


/**
   Same as soap_env_add_item() with c style arguments
   like in printf(). "value" is the format string.
   <br>
   <b>Important: </b> The totally length of value (incl. args)
    must be lower the 1054.

   @see soap_env_add_item
 */
xmlNodePtr 
soap_env_add_itemf(SoapEnv* env, const char *type, 
		  const char *name, const char *value, ...);


/**
   Push the current xml node in the soap envelope one level 
   deeper. Here an example:
   
   <pre>
   soap_env_push_item(env, "my:custom", "Person");
    soap_env_add_item(env, "xsd:string", "name", "Mickey");
    soap_env_add_item(env, "xsd:string", "lastname", "Mouse");
   soap_env_pop_item(env);
   </pre>

   This will create the xml like follows.

   <pre>
   <Person type="my:custom">
    <name>Mickey</name>
    <lastname>Mouse</lastname>
   </Person>
   </pre>

   @returns The added xmlNode pointer.
   
   @see tutorial 
 */
xmlNodePtr 
soap_env_push_item(SoapEnv *env, const char *type,
		   const char *name);

/**
   Sets the xml pointer 1 level higher. 

   @param env The envelope object
   @see soap_env_push_item
 */
void
soap_env_pop_item(SoapEnv* env);

/**
   Free the envelope. 

   @param env The envelope object
 */
void
soap_env_free(SoapEnv *env);


/* --------------------------------------------------- */
/*      XML node finder functions                      */
/* --------------------------------------------------- */


/**
   Gets the xml node pointing to SOAP Body.
 */
xmlNodePtr
soap_env_get_body(SoapEnv* env);


/**
   Get the xml node pointing to SOAP method (call)
 */
xmlNodePtr
soap_env_get_method(SoapEnv* env);


/**
   Get the xml node pointing to SOAP Fault
 */
xmlNodePtr
soap_env_get_fault(SoapEnv* env);


/**
   Get the xml node pointing to SOAP Header
 */
xmlNodePtr
soap_env_get_header(SoapEnv* env);


int soap_env_find_urn(SoapEnv *env, char *urn);
int soap_env_find_methodname(SoapEnv *env, char *methodname);



#endif


