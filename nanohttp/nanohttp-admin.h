/******************************************************************
 *  $Id: nanohttp-admin.h,v 1.3 2006/12/01 10:56:00 m0gg Exp $
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef __nanohttp_admin_h
#define __nanohttp_admin_h

/**
 *
 * Commandline argument to enabled the nanoHTTP admin interface. This service
 * will be reachable via the NHTTPD_ADMIN_CONTEXT.
 *
 * @see NHTTPD_ADMIN_CONTEXT
 *
 */
#define NHTTPD_ARG_ENABLE_ADMIN		"-NHTTPDadmin"

/**
 *
 * Context of the nanoHTTP admin interface.
 *
 * Example query:
 *
 *   http://localhost:10000/nhttp
 *
 * @see httpd_register
 *
 */
#define NHTTPD_ADMIN_CONTEXT		"/nhttp"

/**
 *
 * Query parameter for services.
 *
 * Example query:
 *
 *   http://localhost:10000/nhttp?services=list
 *
 */
#define NHTTPD_ADMIN_QUERY_SERVICES	"services"

/**
 *
 * Parameter to query service statistics.
 *
 * Example query:
 *
 *   http://localhost:10000/nhttp?statistics=SERVICE_CONTEXT
 *
 */
#define NHTTPD_ADMIN_QUERY_STATISTICS	"statistics"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the nanoHTTP admin interface with commandline arguments, if
 * NHTTPD_ARG_ENABLED_ADMIN was specified in the commandline arguments. This
 * service will be reachable via the NHTTP_ADMIN_CONTEXT of the nanohttp server.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 * @see NHTTPD_ADMIN_CONTEXT
 * @see NHTTPD_ARG_ENABLE_ADMIN
 *
 */
extern herror_t httpd_admin_init_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
