/******************************************************************
 *  $Id: soap-wsil.h,v 1.3 2006/11/27 10:52:39 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2006 H. Ronsdorf
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
 * Email: hero@persua.de
 ******************************************************************/
#ifndef __csoap_wsil_h
#define __csoap_wsil_h

/** @file
 *
 * WS-Inspection
 *
 * The WS-Inspection specification provides an XML format for assisting in the
 * inspection of a site for available services and a set of rules for how
 * inspection related information should be made available for consumption. A
 * WS-Inspection document provides a means for aggregating references to
 * pre-existing service description documents which have been authored in any
 * number of formats. These inspection documents are then made available at the
 * point-of-offering for the service as well as through references which may be
 * placed within a content medium such as HTML.
 *
 * Specifications have been proposed to describe Web Services at different levels
 * and from various perspectives. It is the goal of the proposed Web Services
 * Description Language (WSDL) to describe services at a functional level. The
 * Universal Description, Discovery, and Integration (UDDI) schema aims at
 * providing a more business-centric perspective. What has not yet been provided
 * by these proposed standards is the ability to tie together, at the point of
 * offering for a service, these various sources of information in a manner which
 * is both simple to create and use. the WS-Inspection specification addresses
 * this need by defining an XML grammar which facilitates the aggregation of
 * references to different types of service description documents, and then
 * provides a well defined pattern of usage for instances of this grammar. By
 * doing this, the WS-Inspection specification provides a means by which to
 * inspect sites for service offerings.
 *
 * Repositories already exist where descriptive information about Web services
 * has been gathered together. The WS-Inspection specification provides
 * mechanisms with which these existing repositories can be referenced and
 * utilized, so that the information contained in them need not be duplicated if
 * such a duplication is not desired.
 *
 * @author	H. Ronsorf
 * @version	$Revision: 1.3 $
 *
 * @see http://www-128.ibm.com/developerworks/library/specification/ws-wsilspec/,
 *      http://www.w3.org/TR/wsdl,
 *      http://www.uddi.org/
 *
 */

/**
 *
 * Commandline argument to enabled automatic WSIL generation.
 *
 */
#define CSOAP_ENABLE_WSIL	"-CSOAPwsil"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the WSIL HTTP interface with commandline arguments. The generated
 * WSIL document can be seen at http://servername/inspection.wsil.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 * @see CSOAP_ENABLE_WSIL
 *
 */
extern herror_t soap_wsil_init_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
