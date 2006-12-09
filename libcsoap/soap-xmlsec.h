/******************************************************************
 *  $Id: soap-xmlsec.h,v 1.6 2006/12/09 09:35:00 m0gg Exp $
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
 * \section Web Services Security
 *
 * This module is implemented using the xmlsec1 library.
 *
 * @author	H. Ronsdorf
 * @version	$Revision: 1.6 $
 *
 * @see		http://www.w3.org/TR/SOAP-dsig/
 * @see		http://www.oasis-open.org/committees/tc_home.php?wg_abbrev=wss
 * @see		http://www.aleksey.com/xmlsec/
 *
 */

#define SOAP_SECURITY_NAMESPACE	"http://schemas.xmlsoap.org/soap/security/2000-12"
#define SOAP_SECURITY_PREFIX	"SOAP-SEC"

/**
 *
 * Commandline argument to enable WS-Security.
 *
 */
#define CSOAP_ENABLE_XMLSEC	"-CSOAPxmlsec"

/**
 *
 * Commandline argument to set the keyfile. If this argument is not specified, a
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

/** @defgroup xmlsec_errors XML-security errors
 *
 * Various errors related to XML-security
 *
 */
/*@{*/
#define XMLSEC_ERROR			5100
#define XMLSEC_ERROR_GENERIC		(XMLSEC_ERROR + 0)
#define XMLSEC_ERROR_KEYSTORE		(XMLSEC_ERROR + 10)
#define XMLSEC_ERROR_KEYMANAGER		(XMLSEC_ERROR + 20)
#define XMLSEC_ERROR_KEY		(XMLSEC_ERROR + 30)
#define XMLSEC_ERROR_CERTIFICATE	(XMLSEC_ERROR + 40)
#define XMLSEC_ERROR_INIT		(XMLSEC_ERROR + 50)
#define XMLSEC_ERROR_VERSION		(XMLSEC_ERROR + 60)
#define XMLSEC_ERROR_DLLOAD		(XMLSEC_ERROR + 70)
#define XMLSEC_ERROR_SIGN		(XMLSEC_ERROR + 80)
#define XMLSEC_ERROR_SIGN_INIT		(XMLSEC_ERROR + 90)
#define XMLSEC_ERROR_ENCRYPT		(XMLSEC_ERROR + 100)
#define XMLSEC_ERROR_ENCRYPT_INIT	(XMLSEC_ERROR + 110)
/*@}*/

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
 * Sign a XML document contained in a SOAP Envelope with the key specified on
 * the commandline. Our way to create a <SOAP-SEC:Signature> header entry is as
 * follows:
 *
 * - Prepare the target SOAP Envelope with the body and necessary headers.
 * - Create a template of a <ds:Signature> element. The template is assumed to
 *   contain empty contents for <ds:DigestValue> or <ds:SignatureValue> elements,
 *   but contains appropriate values for the elements such as
 *   <ds:SignatureMethod> and <ds:Reference> required to calculate them.
 * - Create a new header entry <SOAP-SEC:Signature> and add the template to this
 *   entry.
 * - Add the header entry <SOAP-SEC:Signature> to the SOAP Header.
 * - Add the SOAP "actor" and "mustUnderstand" attributes to the entry, if
 *   necessary.
 * - Calculate the <ds:DigestValue> and <ds:SignatureValue> elements according
 *   to the core generation of the XML-Signature specification.
 *
 * XPath filtering can be used to specify objects to be signed, as described in
 * the XML-Signature specification. However, since the SOAP message exchange
 * model allows intermediate applications to modify the Envelope (add or delete
 * a header entry, for example), XPath filtering does not always result in the
 * same objects after message delivery. Care should be taken in using XPath
 * filtering so that there is no subsequent validation failure due to such
 * modifications.
 *
 * The transform http://www.w3.org/2000/09/xmldsig#enveloped-signature defined
 * in the XML-Signature specification may be useful when signing the entire
 * Envelope including other header entries, if any.
 *
 * @param context The SOAP context to be signed.
 *
 * @return H_OK on success
 *
 * @see http://www.w3.org/TR/SOAP-dsig/
 * @see http://www.w3.org/TR/xmldsig-core/
 *
 */
extern herror_t soap_xmlsec_sign(struct SoapCtx *context);

/**
 *
 * Verify a XML documents signature contained in a SOAP Envelope. The validation
 * of a <SOAP-SEC:Signature> header entry fails if:
 *
 * - The syntax of the content of the header entry does not conform to SOAP
 *   Security Extensions: Digital Signature specification, or
 * - The validation of the signature contained in the header entry fails
 *   according to the core validation of the XML-Signature specification, or
 * - The receiving application program rejects the signature for some reason
 *   (e.g., the signature is created by an untrusted key).
 *
 * If the validation of the signature header entry fails, applications MAY report
 * the failure to the sender. It is out of the scope of this library how to deal
 * with it.
 *
 * @param context The SOAP context to be verified.
 *
 * @return H_OK on success
 *
 * @see http://www.w3.org/TR/SOAP-dsig/
 * @see http://www.w3.org/TR/xmldsig-core/
 *
 */
extern herror_t soap_xmlsec_verify(struct SoapCtx *context);

/**
 *
 * Encrypt a XML document contained in a SOAP envelope.
 *
 * @param context The SOAP context to be encrypted.
 *
 * @return H_OK on success
 *
 */
extern herror_t soap_xmlsec_encrypt(struct SoapCtx *context);

/**
 *
 * Decrupt a XML document contained in a SOAP envelope.
 *
 * @param context The SOAP context to be decrypted.
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
