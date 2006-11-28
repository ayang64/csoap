/******************************************************************
 *  $Id: soap-xmlsec.h,v 1.3 2006/11/28 23:45:57 m0gg Exp $
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
 * @version	$Revision: 1.3 $
 *
 * @see http://www.w3.org/TR/SOAP-dsig/,
 *      http://www.oasis-open.org/committees/tc_home.php?wg_abbrev=wss,
 *      http://www.aleksey.com/xmlsec/
 *
 */

#define SOAP_SECURITY_NAMESPACE	"http://schemas.xmlsoap.org/soap/security/2000-12"
#define SOAP_SECURITY_PREFIX	"SOAP-SEC"

/**
 *
 * Commandline argument to enabled WS-Security.
 *
 */
#define CSOAP_ENABLE_XMLSEC	"-CSOAPxmlsec"

/**
 *
 * Commandline argument to set the keyfile. If this argument is not specified a
 * random key will be used.
 *
 */
#define CSOAP_XMLSEC_KEYFILE	"-CSOAPkeyfile"

/**
 *
 * Commandline argument to set the password of the key.
 *
 */
#define CSOAP_XMLSEC_PASSWORD	"-CSOAPpassword"

/**
 *
 * Commandline argument to set a file of certificates.
 *
 */
#define CSOAP_XMLSEC_CERTFILE	"-CSOAPcertfile"

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
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_server_init_args(int argc, char **argv);

/**
 *
 * Initializes the WS-Security subsystem.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_client_init_args(int argc, char **argv);

/**
 *
 * Sign a XML document contained in a SOAP Envelope. The key specified on the
 * commandline is used for signating the document.
 *
 * @param envelope The SOAP envelope to be signed.
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_sign(struct SoapCtx *context);

/**
 *
 * Verify a XML documents signature contained in a SOAP Envelope.
 *
 * @param envelope The SOAP envelope to be verified.
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_verify(struct SoapCtx *context);

/**
 *
 * Encrypt a XML document contained in a SOAP envelope.
 *
 * @param envelope The SOAP envelope to be encrypted.
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_encrypt(struct SoapCtx *context);

/**
 *
 * Decrupt a XML document contained in a SOAP envelope.
 *
 * @param envelope The SOAP envelope to be decrypted.
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_decrypt(struct SoapCtx *context);


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
