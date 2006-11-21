/******************************************************************
*  $Id: soap-fault.c,v 1.12 2006/11/21 20:59:02 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-xml.h"
// #include "soap-server.h"
#include "soap-fault.h"

/*
Parameters:
1- soap_env_ns
2- soap_env_enc
3- xsi_ns
4- xsd_ns
5- faultcode
6- faultstring
7- faultactor
8- detail
*/
#define  _SOAP_FAULT_TEMPLATE_ \
	"<SOAP-ENV:Envelope" \
        " xmlns:SOAP-ENV=\"%s\"" \
        " SOAP-ENV:encoding=\"%s\"" \
	" xmlns:xsi=\"%s\"" \
	" xmlns:xsd=\"%s\">" \
        " <SOAP-ENV:Header />" \
	" <SOAP-ENV:Body>" \
	"  <SOAP-ENV:Fault>"\
	"   <faultcode>%s</faultcode>"\
	"   <faultstring>%s</faultstring>"\
	"   <faultactor>%s</faultactor>"\
	"   <detail>%s</detail>"\
	"  </SOAP-ENV:Fault>" \
	" </SOAP-ENV:Body>"\
	"</SOAP-ENV:Envelope>"


static const char const *fault_vm = "VersionMismatch";
static const char const *fault_mu = "MustUnderstand";
static const char const *fault_deu = "DataEncodingUnkown";
static const char const *fault_client = "Client";
static const char const *fault_server = "Server";

xmlDocPtr
soap_fault_build(int fault_code, const char *fault_string, const char *fault_actor, const char *detail)
{

  /* variables */
  const char *faultcode;
  int bufferlen = 2000;
  char *buffer;
  xmlDocPtr fault;              /* result */

  log_verbose1("Build fault");

  switch (fault_code)
  {
  case SOAP_FAULT_VERSION_MISMATCH:
    faultcode = fault_vm;
    break;
  case SOAP_FAULT_MUST_UNDERSTAND:
    faultcode = fault_mu;
    break;
  case SOAP_FAULT_DATA_ENCODING_UNKOWN:
    faultcode = fault_deu;
    break;
  case SOAP_FAULT_RECEIVER:
    faultcode = fault_server;
    break;
  case SOAP_FAULT_SENDER:
  default:
    faultcode = fault_client;
    break;
  }

  /* calculate buffer length */
  if (fault_string)
    bufferlen += strlen(fault_string);
  if (fault_actor)
    bufferlen += strlen(fault_actor);
  if (detail)
    bufferlen += strlen(detail);

  log_verbose2("Creating buffer with %d bytes", bufferlen);
  if (!(buffer = (char *) malloc(bufferlen)))
  {
    log_error2("malloc failed (%s)", errno);
    return NULL;
  }

  sprintf(buffer, _SOAP_FAULT_TEMPLATE_,
          soap_env_ns, soap_env_enc, soap_xsi_ns,
          soap_xsd_ns, faultcode,
          fault_string ? fault_string : "error",
          fault_actor ? fault_actor : "", detail ? detail : "");

  fault = xmlParseDoc(BAD_CAST buffer);
  free(buffer);

  if (fault == NULL)
  {
    log_error1("Can not create xml document!");

    return soap_fault_build(fault_code, "Cannot create fault object in XML", soap_server_get_name(), NULL);
  }

  log_verbose2("Returning fault (%p)", fault);
  return fault;
}
