/******************************************************************
*  $Id: nanohttp-ssl.h,v 1.31 2007/11/03 22:40:15 m0gg Exp $
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

/** @page nanohttp_ssl_page How to use SSL with nanoHTTP
 *
 * @section nanohttp_ssl_toc_sec Table of contents
 *
 * - @ref nanohttp_ssl_overview_sec
 * - @ref nanohttp_ssl_configuration_sec
 * - @ref nanohttp_ssl_key_management_sec
 *   - @ref nanohttp_ssl_key_generation_sec
 *   - @ref nanohttp_ssl_cert_generation_sec
 *   - @ref nanohttp_ssl_ca_generation_sec
 *     - @ref nanohttp_ssl_ca_dir_sec
 *     - @ref nanohttp_ssl_ca_key_sec
 *     - @ref nanohttp_ssl_sign_sec
 * - @ref nanohttp_ssl_cmdline_sec
 * - @ref nanohttp_ssl_verification_sec
 * - @ref nanohttp_ssl_faq_sec
 *
 * @section nanohttp_ssl_overview_sec How to create an SSL enabled HTTP service
 *
 * There are two basic steps involved in using nanoHTTP to create a SSL enabled
 * HTTP services:
 *
 * -# @ref nanohttp_ssl_configuration_sec
 * -# @ref nanohttp_ssl_key_management_sec
 *
 * Additionally you may:
 *
 * -# @ref nanohttp_ssl_cmdline_sec
 * -# @ref nanohttp_ssl_verification_sec
 *
 * @section nanohttp_ssl_configuration_sec Compilation with SSL support
 *
 * nanoHTTP uses OpenSSL for its SSL support, therefore OpenSSL must be installed
 * first. To compile in SSL support, simply provide the --with-ssl argument to
 * configure.
 *
 * @code
 * $ ./configure --with-ssl
 * @endcode
 *
 * @section nanohttp_ssl_key_management_sec Create an SSL key/certificate
 *
 * @subsection nanohttp_ssl_key_generation_sec Simple key generation
 *
 * @code
 * $ openssl req -nodes -days 1825 -subj "/CN=`hostname`" -newkey rsa:1024 -keyout sslkey.pem -out sslreq.pem
 * @endcode
 *
 * @subsection nanohttp_ssl_cert_generation_sec Generate a key with a certificate
 *
 * @subsection nanohttp_ssl_a_sec Create a key and a certification request
 * @subsection nanohttp_ssl_b_sec Post the sslreq.pem to your favorite CA
 * @subsection nanohttp_ssl_c_sec Join your key with the certificate from yout CA
 *
 * @code
 * $ cat ssl.cert >> sslkey.pem
 * @endcode
 *
 * @subsection nanohttp_ssl_ca_generation_sec Generate a certification authority
 *
 * @subsubsection nanohttp_ssl_ca_dir_sec Create the directory structure
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
 * @subsubsection nanohttp_ssl_ca_key_sec Generate the CA key
 *
 * @code
 * $ openssl req -x509 -nodes -days 1826 -subj "/CN=myCa" -newkey rsa:1024 -keyout ca/private/cakey.pem -out ca/cacert.pem
 * @endcode
 *
 * @subsubsection nanohttp_ssl_sign_sec Sign a certification request
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
 * @section nanohttp_ssl_verification_sec Write a certificate verification routine
 *
 * Your verification script must take an X509 * as its only argument and it
 * should return an int. The CSOAP library provides a helper function verify_sn()
 * to assist in writing certificate verification routines. You by no means need
 * to use it! verify_sn() takes the following arguments:
 *
 * - X509 *cert - a pointer to the X509 cert passed into your verify function 
 * - int who - one of two values, CERT_SUBJECT or CERT_ISSUER to specify if you
 *   wish to verify the issuer line or the subject line in the certificate file. 
 * - int nid - the NID of the attribute you wish to compare
 *   (see http://www.openssl.org/docs/crypto/OBJ_nid2obj.html) 
 * - char *str - the string you wish to compare
 *
 * @code
 * int my_user_verify(X509* cert)
 * {
 *   ASN1_TIME *notAfter = X509_get_notAfter(cert);
 *         
 *   if (X509_cmp_current_time(notAfter) <= 0)
 *   {
 *     fprintf(stderr,"SSL Certificate has expired");
 *     return 0;
 *   }   
 *                                     
 *   if (!verify_sn(cert, CERT_ISSUER, NID_commonName, "My Common Name") )
 *   {
 *     fprintf(stderr, "issuer commonName does not match");
 *     return 0;
 *   }
 * 
 *   if (!verify_sn(cert, CERT_ISSUER, NID_organizationName, "My Organization") )
 *   {
 *     fprintf(stderr, "issuer organizationName does not match");
 *     return 0;
 *   }   
 * 
 *   if (!verify_sn(cert, CERT_SUBJECT, NID_commonName, "My Web Service") )
 *   {
 *     fprintf(stderr, "subject commonName does not match");
 *     return 0;
 *   }
 *
 *   if (!verify_sn(cert, CERT_SUBJECT, NID_organizationName, "My Organization") )
 *   {
 *     fprintf(stderr, "subject organizationName does not match");
 *     return 0;
 *   }
 *
 *   fprintf(stderr, "Certificate checks out");
 *   return 1;
 * }
 *
 * @endcode
 *
 * To register your verification function, simply:
 *
 * @code
 * hssl_set_verify_cert(my_user_verify);
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

/** @file nanohttp-ssl.h SSL wrapper
 *
 * @defgroup NANOHTTP_SSL SSL wrapper
 * @ingroup NANOHTTP
 *
 */
/**@{*/

/**
 *
 * Commandline argument to enabled SSL in the nanoHTTP server.
 *
 */
#define NHTTPD_ARG_HTTPS	"-NHTTPS"

/** @defgroup NANOHTTP_SSL_ERRORS SSL Errors
 * @ingroup NANOHTTP_ERRORS
 */
/**@{*/

#define HSSL_ERROR			1700
#define HSSL_ERROR_CA_LIST		(HSSL_ERROR + 10)
#define HSSL_ERROR_CONTEXT		(HSSL_ERROR + 20)
#define HSSL_ERROR_CERTIFICATE		(HSSL_ERROR + 30)
#define HSSL_ERROR_PEM			(HSSL_ERROR + 40)
#define HSSL_ERROR_CLIENT		(HSSL_ERROR + 50)
#define HSSL_ERROR_SERVER		(HSSL_ERROR + 60)
#define HSSL_ERROR_CONNECT		(HSSL_ERROR + 70)

#define HSSL_ERROR_SSLCLOSE		(HSSL_ERROR + 80)
#define HSSL_ERROR_SSLCTX		(HSSL_ERROR + 90)

/**@}*/

#ifdef __cplusplus
extern "C" {
#endif

/** This function initializes the SSL module.
 *
 * @return H_OK on success
 *
 * @see hss_module_destroy()
 */
extern herror_t hssl_module_init(int argc, char **argv);

/** This functions destroys the SSL module.
 *
 * @see hssl_module_init()
 */
extern void hssl_module_destroy(void);

/** This function sets the SSL certificate to be used.
 */
extern void hssl_set_certificate(const char *filename);

/** This function sets the password for the SSL certificate.
 *
 * @see hssl_set_certificate()
 */
extern void hssl_set_certpass(const char *password);

/** This function sets the filename for a certification authority
 * list.
 */
extern void hssl_set_ca_list(const char *filename);

/** This function enables SSL in the nanoHTTP server. You have to
 * call this function before calling httpd_run().
 */
extern void hssl_enable(void);

/** This function returns if SSL is enabled in the nanoHTTP server.
 *
 * @return 1 if enabled, 0 otherwise.
 */
extern int hssl_enabled(void);

/** This function initializes an SSL client socket.
 *
 * @return H_OK on success.
 *
 * @see hsl_cleanup()
 */
extern herror_t hssl_client_ssl(struct hsocket_t * sock);

/** This function initializes an SSL server socket.
 *
 * @return H_OK on success.
 *
 * @see hssl_cleanup()
 */
extern herror_t hssl_server_ssl(struct hsocket_t * sock);

/** This function cleans up an SSL socket.
 *
 * @see hssl_client_ssl
 * @see httl_server_ssl
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

/** This function reads from an SSL socket.
 *
 * @see hsocket_read()
 */
extern herror_t hssl_read(struct hsocket_t * sock, char *buf, size_t len, size_t * received);

/** This function writes to an SSL socket.
 *
 * @see hsocket_write()
 */
extern herror_t hssl_write(struct hsocket_t * sock, const char *buf, size_t len, size_t * sent);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
