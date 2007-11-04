/******************************************************************
 *  $Id: soap-admin.h,v 1.7 2007/11/04 06:57:32 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2003-2006  Ferhat Ayaz
 * Copyright (C) 2007       Heiko Ronsdorf
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
#ifndef __csoap_admin_h
#define __csoap_admin_h

/** @file soap-admin.h HTTP administrator interface
 *
 * @defgroup CSOAP_ADMIN HTTP administration interface
 * @ingroup CSOAP
 */
/**@{*/

#define CSOAP_ENABLE_ADMIN		"-CSOAPadmin"

#define CSOAP_ADMIN_QUERY_ROUTERS	"routers"
#define CSOAP_ADMIN_QUERY_ROUTER	"router"
#define CSOAP_ADMIN_QUERY_SERVICES	"services"

#define CSOAP_ADMIN_QUERY_ACTIVATE	"activate"
#define CSOAP_ADMIN_QUERY_PASSIVATE	"passivate"

#define CSOAP_ADMIN_URN			"urn"
#define CSOAP_ADMIN_METHOD		"method"

#define CSOAP_ADMIN_CONTEXT             "csoap"

#ifdef __cplusplus
extern "C" {
#endif

/** This function initializes the cSOAP admin HTTP interface with
 * commandline arguments.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success.
 */
extern herror_t soap_admin_init_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
