/******************************************************************
 *  $Id: soap-env.h,v 1.2 2004/02/03 08:59:22 snowdrop Exp $
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


typedef struct _SoapEnv
{ 
  xmlNodePtr root; 
  xmlNodePtr cur;
}SoapEnv;



SoapEnv *soap_env_new_with_fault(fault_code_t faultcode, 
				 const char *faultstring,
				 const char *faultactor,
				 const char *detail);
SoapEnv *soap_env_new_with_method(const char *urn, const char *method);
SoapEnv *soap_env_new_with_response(SoapEnv *method);
SoapEnv *soap_env_new_from_doc(xmlDocPtr doc);
SoapEnv *soap_env_new_from_buffer(const char* buffer);


xmlNodePtr 
soap_env_add_item(SoapEnv* env, const char *type, 
		  const char *name, const char *value);
xmlNodePtr 
soap_env_add_itemf(SoapEnv* env, const char *type, 
		  const char *name, const char *value, ...);
xmlNodePtr 
soap_env_push_item(SoapEnv *env, const char *type,
		   const char *name);
void
soap_env_pop_item(SoapEnv* env);


xmlNodePtr
soap_env_get_body(SoapEnv* env);
xmlNodePtr
soap_env_get_method(SoapEnv* env);
xmlNodePtr
soap_env_get_fault(SoapEnv* env);
xmlNodePtr
soap_env_get_header(SoapEnv* env);


int soap_env_find_urn(SoapEnv *env, char *urn);
int soap_env_find_methodname(SoapEnv *env, char *methodname);



#endif


