/******************************************************************
 *  $Id: soap-client.h,v 1.3 2004/08/30 15:26:53 snowdrop Exp $
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
#ifndef cSOAP_CLIENT_H
#define cSOAP_CLIENT_H

#include <libcsoap/soap-env.h>


/**
	Initializes the client side soap engine
*/
int soap_client_init_args(int argc, char *argv[]);


/**
   Establish connection to the soap server and send 
   the given envelope. 

   @param env envelope to send
   @param url url to the soap server
   @soap_action value for "SoapAction:" in the 
    HTTP request header.

    @returns the result envelope. In case of failure,
     this function return an envelope with a fault object.
 */
SoapEnv* soap_client_invoke(SoapEnv *env, 
			    const char *url,
			    const char *soap_action);


#endif


