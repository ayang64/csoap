/******************************************************************
 * $Id: simple.c,v 1.1 2003/11/13 10:53:00 snowdrop Exp $
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

#include <libcsoap/csoap.h>


static const char *url = "http://csoap.sourceforge.net/cgi-bin/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";


int main(int argc, char *argv[])
{
  /* Objects to do a soap call */ 
  HSOAPCALL call;
  HSOAPRES result;
  HSOAPPARAM res;
  HSOAPFAULT fault;

  char *str;

  /* Initialize CSOAP */
  SoapInit(argc, argv);

  /* Create a soap call object */
  call = SoapCallCreate((argc>1)?argv[1]:url, urn, method);

  /* Add the only parameter for the soap call */
  SoapCallAddStringParam(call, "name", "John B. Good");
 
  /* Now invoke the call. Here we try to      */
  /* establish a network (http) communication */
  result = SoapCallInvoke(call);

  /* Check if the call was succesfull*/
  if (result == NULL) {
    fprintf(stderr, "Can not send the soap call\n");
    exit(1);
  }

  /* Now check if we received a fault object */
  if (fault = SoapResGetFault(result)) {
		
    /* Use the default printer */
    SoapFaultPrint(stdout, fault);

  } else {
    
    /* Print the only string in the response */
    res = SoapResGetParamChildren(result);

    if (res != NULL) {
      str = SoapParamToString(res);
      fprintf(stdout, "Server: '%s'\n", str);
      free(str);
    } else {
      fprintf(stderr, "Corrupt response!\n");
    }
  }

  return 0;
}

