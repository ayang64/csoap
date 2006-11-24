/******************************************************************
*  $Id: nanohttp-ssl.h,v 1.21 2006/11/24 17:28:07 m0gg Exp $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SSL

#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/ssl.h>
#endif

/**
 *
 * Commandline argument to enabled SSL in the nanoHTTP server.
 *
 */
#define NHTTPD_ARG_HTTPS	"-NHTTPS"

#ifdef __cplusplus
extern "C"
{
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

extern void hssl_set_certificate(char *c);
extern void hssl_set_certpass(char *c);
extern void hssl_set_ca(char *c);

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

/*
 * Quick function for verifying a portion of the cert
 * nid is any NID_ defined in <openssl/objects.h>
 * returns non-zero if everything went ok
 */
#define CERT_SUBJECT	1

extern int verify_sn(X509 * cert, int who, int nid, char *str);

/**
 *
 * Called by framework for verification of client or server supplied
 * certificate.
 *
 */
extern void hssl_set_user_verify(int func(X509 * cert));

#ifdef __cplusplus
}
#endif

#else /* HAVE_SSL */

static inline herror_t
hssl_module_init(int argc, char **argv)
{
  return H_OK;
}

static inline void
hssl_module_destroy(void)
{
  return;
}

static inline int
hssl_enabled(void)
{
  return 0;
}

static inline herror_t
hssl_client_ssl(struct hsocket_t *sock)
{
  return H_OK;
}

static inline herror_t
hssl_server_ssl(struct hsocket_t *sock)
{
  return H_OK;
}

static inline void
hssl_cleanup(struct hsocket_t *sock)
{
  return;
}

#endif /* HAVE_SSL */

#ifdef __cplusplus
extern "C"
{
#endif

extern herror_t hssl_read(struct hsocket_t * sock, char *buf, size_t len, size_t * received);
extern herror_t hssl_write(struct hsocket_t * sock, const char *buf, size_t len, size_t * sent);

#ifdef __cplusplus
}
#endif

#endif
