/******************************************************************
*  $Id: nanohttp-ssl.h,v 1.28 2007/01/05 09:03:34 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C
* Copyright (C) 2001-2005  Rochester Institute of Technology
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
* Author: Matt Campbell
******************************************************************/
#ifndef __nanohttp_ssl_h
#define __nanohttp_ssl_h

/** @page nanohttp_ssl_page How to use SSL with nanoHTTP/cSOAP
 *
 * @section nanohttp_ssl_toc_sec Table of contents
 *
 * - @ref nanohttp_ssl_key_generation_sec
 * - @ref nanohttp_ssl_cert_generation_sec
 * - @ref nanohttp_ssl_ca_generation_sec
 *   - @ref nanohttp_ssl_ca_dir_sec
 *   - @ref nanohttp_ssl_ca_key_sec
 *   - @ref nanohttp_ssl_sign_sec
 * - @ref nanohttp_ssl_cmdline_sec
 * - @ref nanohttp_ssl_faq_sec
 *
 * @section nanohttp_ssl_key_generation_sec Simple key generation
 *
 * @code
 * $ openssl req -nodes -days 1825 -subj "/CN=`hostname`" -newkey rsa:1024 -keyout sslkey.pem -out sslreq.pem
 * @endcode
 *
 * @section nanohttp_ssl_cert_generation_sec Generate a key with a certificate
 *
 * @subsection nanohttp_ssl_a_sec Create a key and a certification request
 * @subsection nanohttp_ssl_b_sec Post the sslreq.pem to your favorite CA
 * @subsection nanohttp_ssl_c_sec Join your key with the certificate from yout CA
 *
 * @code
 * $ cat ssl.cert >> sslkey.pem
 * @endcode
 *
 * @section nanohttp_ssl_ca_generation_sec Generate a certification authority
 *
 * @subsection nanohttp_ssl_ca_dir_sec Create the directory structure
 *
 * @code
 * $ mkdir ca
 * $ echo '01' > $1 ca/serial
 * $ touch ca/index.txt
 * $ mkdir ca/crl
 * $ mkdir ca/newcerts
 * $ mkdir ca/private
 * $ chmod 700 ca/private
 * @endcode
 *
 * @subsection nanohttp_ssl_ca_key_sec Generate the CA key
 *
 * @code
 * $ openssl req -x509 -nodes -days 1826 -subj "/CN=myCa" -newkey rsa:1024 -keyout ca/private/cakey.pem -out ca/cacert.pem
 * @endcode
 *
 * @subsection nanohttp_ssl_sign_sec Sign a certification request
 *
 * @code
 * $ openssl ca -in sslreq.pem -out ssl.cert
 * @endcode
 *
 * @section nanohttp_ssl_cmdline_sec Commandline arguments at startup
 *
 * @code
 * -NHTTPS                 Enable https protocol in the nanoHTTP server
 * 
 * -NHTTPcert CERTfile     A file containing a certificate chain from file. The
 *                         certificates must be in PEM format and must be sorted
 *                         starting with the subject's certificate (actual client
 *                         or server certificate), followed by intermediate CA
 *                         certificates if applicable, and ending at the highest
 *                         level (root) CA.
 * 
 * -NHTTPcertpass password The password to be used during decryption of the
 *                         certificate.
 * 
 * -NHTTPCA CAfile         File pointing to a file of CA certificates in PEM
 *                         format. The file can contain several CA certificates
 *                         identified by
 * 
 *                          -----BEGIN CERTIFICATE-----
 *                          ... (CA certificate in base64 encoding) ...
 *                          -----END CERTIFICATE-----
 * 
 *                         sequences. Before, between, and after the certificates
 *                         text is allowed which can be used e.g. for descriptions
 *                         of the certificates. 
 * @endcode
 *
 * @section nanohttp_ssl_faq_sec Frequently asked questions
 *
 * - Howto hide the password
 *
 *   You can use the following functions before calling httpd_init, httpc_init
 *   and accordingly soap_server_init, soap_client_init. The are roughly the same
 *   then the commandline versions.
 *
 *   @code
 *   hssl_enable(void)
 *   hssl_set_certificate(const char *CERTfile)
 *   hssl_set_certpass(const char *pass)
 *   hssl_set_ca_list(const char *CAfile)
 *   @endcode
 * 
 *   NOTE: If you use this functions an specify the commandline arguments, then
 *   the commandline arguments take precedence.
 *
 * - What else?
 *
 *   @code
 *   hssl_enabled(void)
 *   @endcode
 *
 */

/**
 *
 * Commandline argument to enabled SSL in the nanoHTTP server.
 *
 */
#define NHTTPD_ARG_HTTPS	"-NHTTPS"

/**
 *
 * @defgroup ssl_errors_group SSL Errors
 *
 */
/*@{*/

#define HSSL_ERROR			1700
#define HSSL_ERROR_CA_LIST		(HSSL_ERROR + 10)
#define HSSL_ERROR_CONTEXT		(HSSL_ERROR + 20)
#define HSSL_ERROR_CERTIFICATE		(HSSL_ERROR + 30)
#define HSSL_ERROR_PEM			(HSSL_ERROR + 40)
#define HSSL_ERROR_CLIENT		(HSSL_ERROR + 50)
#define HSSL_ERROR_SERVER		(HSSL_ERROR + 60)
#define HSSL_ERROR_CONNECT		(HSSL_ERROR + 70)

/*@}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initialization of the SSL module
 *
 * @return H_OK on success
 *
 */
extern herror_t hssl_module_init(int argc, char **argv);

/**
 *
 * Shutdown of the SSL module.
 *
 */
extern void hssl_module_destroy(void);

/**
 *
 * Sets the SSL certificate to be used.
 *
 */
extern void hssl_set_certificate(const char *filename);

/**
 *
 * Sets the password for the SSL certificate.
 *
 * @see hssl_set_certificate
 *
 */
extern void hssl_set_certpass(const char *password);

/**
 *
 * Sets the filename for a certification authority list.
 *
 */
extern void hssl_set_ca_list(const char *filename);

/**
 *
 * Enabled SSL in the nanoHTTP server. You have to call this function before
 * calling httpd_run.
 *
 * @see httpd_run
 *
 */
extern void hssl_enable(void);

/**
 *
 * Check if SSL is enabled in the nanoHTTP server.
 *
 * @return 1 if enabled, 0 if disabled.
 *
 */
extern int hssl_enabled(void);

/**
 *
 * SSL client socket initialization.
 *
 * @return H_OK on success.
 *
 */
extern herror_t hssl_client_ssl(struct hsocket_t * sock);

/**
 *
 * SSL server socket initialization.
 *
 * @return H_OK on success.
 *
 */
extern herror_t hssl_server_ssl(struct hsocket_t * sock);

/**
 *
 * SSL socket cleanup.
 *
 */
extern void hssl_cleanup(struct hsocket_t * sock);

/**
 *
 * Quick function for verifying a portion of the cert nid is any NID_ defined
 * in <openssl/objects.h> returns non-zero if everything went ok
 *
 */
extern int verify_sn(X509 * cert, int who, int nid, char *str);

/**
 *
 * Called by framework for verification of client or server supplied certificate.
 *
 */
extern void hssl_set_user_verify(int func(X509 * cert));

/**
 *
 * Read from a SSL socket.
 *
 * @see hsocket_read
 *
 */
extern herror_t hssl_read(struct hsocket_t * sock, char *buf, size_t len, size_t * received);

/**
 *
 * Write to a SSL socket.
 *
 * @see hsocket_write
 *
 */
extern herror_t hssl_write(struct hsocket_t * sock, const char *buf, size_t len, size_t * sent);

#ifdef __cplusplus
}
#endif

#endif
