/******************************************************************
 * $Id: simple.c,v 1.3 2004/01/30 16:39:59 snowdrop Exp $
 *
 * CSOAP Project:  CSOAP examples project 
 * Copyright (C) 2003  Ferhat Ayaz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
 *
 * Email: ayaz@jprogrammer.net
 ******************************************************************/

#include <libcsoap/soap-call.h>


static const char *url = "http://csoap.sourceforge.net/cgi-bin/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";


int main(int argc, char *argv[])
{
  SoapCall *call;
  xmlDocPtr doc;

  call = soap_call_new(urn, method);
  soap_call_add_param(call, "xsd:string", "name", "Jonny B. Good");
  
  doc = soap_call_invoke(call, url);
  soap_xml_doc_print(doc);
  xmlFreeDoc(doc);

  free(call);

  return 0;
}





