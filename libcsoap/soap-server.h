/******************************************************************
 *  $Id: soap-server.h,v 1.1 2004/02/03 08:10:05 snowdrop Exp $
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
#ifndef cSOAP_SERVER_H
#define cSOAP_SERVER_H

#include <libcsoap/soap-env.h>
#include <libcsoap/soap-router.h>


int soap_server_init_args(int argc, char *argv[]);
int soap_server_register_router(SoapRouter *router, const char* context);
int soap_server_run();
void soap_server_destroy();


#endif


