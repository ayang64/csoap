/******************************************************************
 *  $Id: soap-xmlsec.h,v 1.2 2006/11/27 11:15:27 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2006 Heiko Ronsdorf
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
#ifndef __csoap_xmlsec_h
#define __csoap_xmlsec_h

/** @file
 *
 * Web Services Security
 *
 * This module is implemented using the xmlsec1 library.
 *
 * @author	H. Ronsdorf
 * @version	$Revision: 1.2 $
 *
 * @see http://www.oasis-open.org/committees/tc_home.php?wg_abbrev=wss
 *      http://www.aleksey.com/xmlsec/,
 *
 */

/**
 *
 * Commandline argument to enabled WS-Security.
 *
 */
#define CSOAP_ENABLE_XMLSEC	"-CSOAPxmlsec"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the WS-Security subsystem.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 */
extern herror_t soap_xmlsec_init_args(int argc, char **argv);

/**
 *
 * Sign a XML document contained in a SOAP Envelope
 *
 * @param envelope The SOAP envelope to be signed.
 *
 */
extern herror_t soap_xmlsec_sign(struct SoapEnv *envelope);

/**
 *
 * Encrypt a XML document contained in a SOAP envelope.
 *
 * @param envelope The SOAP envelope to be encrypted.
 *
 */
extern herror_t soap_xmlsec_encrypt(struct SoapEnv *envelope);

/**
 *
 * Frees the resources needed by the XML security subsystem.
 *
 */
extern void soap_xmlsec_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
