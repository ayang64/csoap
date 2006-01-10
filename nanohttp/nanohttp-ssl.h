/*
 * Copyright (C) 2001-2005  Rochester Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Author:		Matt Campbell
 * Contrib:		
 * Descrip:		Common ssl routines
 */

/* Do enter only if --with-ssl was specified by the configure script */
#ifdef HAVE_SSL

#ifdef TRU64
#include <arpa/inet.h>
typedef unsigned int uint32_t;
#endif

#ifndef __NANOHTTP_SSL_H_
#define __NANOHTTP_SSL_H_

#include <openssl/ssl.h>

#ifdef WIN32
typedef unsigned int uint32_t;
#else
#include <unistd.h>
#endif

#define SSL_SERVER	0
#define SSL_CLIENT	1

#define CERT_SUBJECT	0
#define CERT_ISSUER		1

typedef struct Con
{
  SSL *ssl;
  int sock;
} Con;

/*
 * Callback for password checker
 */

//static int pw_cb(char* buf, int num, int rwflag, void *userdata);

/*
 * Initialize the context
 */

SSL_CTX *initialize_ctx(char *keyfile, char *password, char *calist);

/*
 * Quick function for verifying a portion of the cert
 * nid is any NID_ defined in <openssl/objects.h>
 * returns non-zero if everything went ok
 */

int verify_sn(X509 * cert, int who, int nid, char *str);

/*
 * Called by framework for verify
 */

//static int verify_cb(int prev_ok, X509_STORE_CTX* ctx);

/*
 * This function MUST be implemented by user client/server code somewhere
 */

int user_verify(X509 * cert);

/*
 * Create the ssl socket and return it
 * pass in the context and an open socket
 */

SSL *init_ssl(SSL_CTX * ctx, int sock, int type);

/*
 * Close the ssl connection (socket is still left open)
 */

void ssl_cleanup();

#endif

#endif /* HAVE_SSL */
