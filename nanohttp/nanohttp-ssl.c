/******************************************************************
*  $Id: nanohttp-ssl.c,v 1.20 2006/03/07 16:20:37 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

#ifdef HAVE_SSL
#ifdef HAVE_OPENSSL_RAND_H
#include <openssl/rand.h>
#endif

#ifdef HAVE_OPENSSL_ERR_H
#include <openssl/err.h>
#endif
#endif

#include "nanohttp-common.h"
#include "nanohttp-socket.h"
#include "nanohttp-ssl.h"

#ifdef HAVE_SSL

static char *certificate = NULL;
static char *certpass = NULL;
static char *ca_list = NULL;
static SSL_CTX *context = NULL;

static int enabled = 0;
static int initialized = 0;


static void
_hssl_superseed (void)
{
  int buf[256], i;

  srand (time (NULL));

  for (i = 0; i < 256; i++)
  {
    buf[i] = rand ();
  }
  RAND_seed ((unsigned char *) buf, sizeof (buf));

  return;
}


static char *
_hssl_get_error(SSL *ssl, int ret)
{
  switch(SSL_get_error(ssl, ret))
  {
    case SSL_ERROR_NONE:
      return "None";
    case SSL_ERROR_ZERO_RETURN:
      return "Zero return";
    case SSL_ERROR_WANT_READ:
      return "Want read";
    case SSL_ERROR_WANT_WRITE:
      return "Want write";
    case SSL_ERROR_WANT_X509_LOOKUP:
      return "Want x509 lookup";
    case SSL_ERROR_SYSCALL:
      return "Syscall failed";
    case SSL_ERROR_SSL:
      return "SSL error";
    default:
      return "Unkown";
  }
}


static int
pw_cb (char *buf, int num, int rwflag, void *userdata)
{
  if (num < (int) strlen (certpass) + 1)
    return (0);

  strcpy(buf, certpass);
  return strlen(certpass);
}


int
verify_sn (X509 * cert, int who, int nid, char *str)
{
  char name[256];
  char buf[256];

  memset (name, '\0', 256);
  memset (buf, '\0', 256);

  if (who == CERT_SUBJECT)
  {
    X509_NAME_oneline (X509_get_subject_name (cert), name, 256);
  }
  else
  {
    X509_NAME_oneline (X509_get_issuer_name (cert), name, 256);
  }

  buf[0] = '/';
  strcat (buf, OBJ_nid2sn (nid));
  strcat (buf, "=");
  strcat (buf, str);

  if (strstr (name, buf))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


#ifdef NOUSER_VERIFY
static int
user_verify (X509 * cert)
{
  /* TODO: Make sure that the client is providing a client cert,
     or that the Module is providing the Module cert */

  /* connect to anyone */

  log_verbose1 ("Validating certificate.");
  return 1;
}
#endif

static int
verify_cb (int prev_ok, X509_STORE_CTX * ctx)
{
/*
    if ((X509_STORE_CTX_get_error(ctx) = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN))
    {
        log_verbose1("Self signed cert in chain");
        return 1;
    }
*/
#ifdef NOUSER_VERIFY            /* ifdef's added by Ferhat. because of
                                   unresolved reference while compiling */
  if (X509_STORE_CTX_get_error_depth(ctx) == 0)
  {
    return user_verify (X509_STORE_CTX_get_current_cert(ctx));
  }
  else
  {
#endif
    log_verbose1 ("Cert ok (prev)");
    return prev_ok;
#ifdef NOUSER_VERIFY
  }
#endif
}


static void
_hssl_parse_arguments(int argc, char **argv)
{

  int i;

  for (i=1; i<argc; i++)
  {
    if (!strcmp(argv[i-1], NHTTP_ARG_CERT))
    {
      certificate = argv[i];
    }
    else if (!strcmp(argv[i-1], NHTTP_ARG_CERTPASS))
    {
      certpass = argv[i];
    }
    else if (!strcmp(argv[i-1], NHTTP_ARG_CA))
    {
      ca_list = argv[i];
    }
    else if (!strcmp(argv[i-1], NHTTP_ARG_HTTPS))
    {
      enabled = 1;
    }
  }

  return;
}


static void
_hssl_library_init(void)
{
  /* Global system initialization */
  log_verbose1 ("Initializing library");

  SSL_library_init();

  SSL_load_error_strings();
  ERR_load_crypto_strings();

  OpenSSL_add_ssl_algorithms();

  initialized = 1;

  return;
}


static herror_t
hssl_context_init(void)
{
  log_verbose3("enabled=%i, certificate=%p", enabled, certificate);

  if (!enabled || !certificate)
    return H_OK;

  if (certpass == NULL)
    certpass = "";
 
  /* Create our context */
  if (!(context = SSL_CTX_new(SSLv23_method())))
  {
    log_error1 ("Cannot create SSL context");
    return herror_new("hssl_context_init", HSSL_ERROR_CONTEXT, "Unable to create SSL context");
  }

  if (!(SSL_CTX_use_certificate_file (context, certificate, SSL_FILETYPE_PEM)))
  {
    SSL_CTX_free(context);
    log_error2 ("Cannot read certificate file: \"%s\"", certificate);
    return herror_new("hssl_context_init", HSSL_ERROR_CERTIFICATE, "Unable to use SSL certificate \"%s\"", certificate);
  }

  SSL_CTX_set_default_passwd_cb(context, pw_cb);

  if (!(SSL_CTX_use_PrivateKey_file(context, certificate, SSL_FILETYPE_PEM)))
  {
      SSL_CTX_free(context);
      log_error2 ("Cannot read key file: \"%s\"", certificate);
      return herror_new("hssl_context_init", HSSL_ERROR_PEM, "Unable to use private key");
  }

  if (ca_list != NULL && *ca_list != '\0')
  {
    if (!(SSL_CTX_load_verify_locations(context, ca_list, NULL)))
    {
      SSL_CTX_free(context);
      log_error2 ("Cannot read CA list: \"%s\"", ca_list);
      return herror_new("hssl_context_init", HSSL_ERROR_CA_LIST, "Unable to read certification authorities \"%s\"");
    }

    SSL_CTX_set_client_CA_list (context, SSL_load_client_CA_file (ca_list));
    log_verbose1 ("Certification authority contacted");
  }
  SSL_CTX_set_verify(context, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, verify_cb);

  log_verbose1("Verify callback registered");

  SSL_CTX_set_mode(context, SSL_MODE_AUTO_RETRY);

  SSL_CTX_set_session_cache_mode(context, SSL_SESS_CACHE_OFF);

  _hssl_superseed();

  return H_OK;
}


static void
_hssl_context_destroy(void)
{
  if (context)
  {
      SSL_CTX_free(context);
      context = NULL;
  }
  return;
}


herror_t
hssl_module_init(int argc, char **argv)
{
  _hssl_parse_arguments(argc, argv);

  if (!initialized)
  {
    if (enabled)
    {
      _hssl_library_init();
      log_verbose1("SSL enabled");
    }
    else
    {
      log_verbose1("SSL _not_ enabled");
    }
  }

  return hssl_context_init();
}


void
hssl_module_destroy(void)
{
  _hssl_context_destroy();

  return;
}


herror_t
hssl_client_ssl(hsocket_t *sock)
{
  SSL *ssl;
  int ret;

  log_verbose1 ("Starting SSL client initialization");

  if (!(ssl = SSL_new(context)))
  {
    log_error1("Cannot create new SSL object");
    return herror_new("hssl_client_ssl", HSSL_ERROR_CLIENT, "SSL_new failed");
  }

  SSL_set_fd (ssl, sock->sock);

  if ((ret = SSL_connect(ssl)) <= 0)
  {
    herror_t err;

    log_error2 ("SSL connect error (%s)", _hssl_get_error(ssl, -1));
    err = herror_new("hssl_client_ssl", HSSL_ERROR_CONNECT, "SSL_connect failed (%s)", _hssl_get_error(ssl, ret));
    SSL_free (ssl);
    return err;
  }

  /* SSL_connect should take care of this for us.
  if (SSL_get_peer_certificate(ssl) == NULL)
  {
    log_error1("No certificate provided");
    SSL_free(ssl);
    return herror_new("hssl_client_ssl", HSSL_ERROR_CERTIFICATE, "No certificate provided");
  }

  if (SSL_get_verify_result(ssl) != X509_V_OK)
  { 
    log_error1("Certificate did not verify");
    SSL_free(ssl);
    return herror_new("hssl_client_ssl", HSSL_ERROR_CERTIFICATE, "Verfiy certificate failed");
  } */

  log_verbose1 ("SSL client initialization completed");
 
  /* XXX: why???
  if ((status = hsocket_block(sock, sock->block)) != H_OK)
  {
    log_error2("Cannot make socket non-blocking (%s)", herror_message(status));
    SSL_free(ssl);
    return status;
  } */

  sock->ssl = ssl;
  
  return H_OK;
}


herror_t
hssl_server_ssl(hsocket_t *sock)
{
  SSL *ssl;
  int ret;

  if (!enabled)
    return H_OK;

  log_verbose2("Starting SSL initialization for socket %d", sock->sock);

  if (!(ssl = SSL_new(context)))
  {
    log_warn1("SSL_new failed");
    return herror_new("hssl_server_ssl", HSSL_ERROR_SERVER, "Cannot create SSL object");
  }
  SSL_set_fd(ssl, sock->sock);

  if ((ret = SSL_accept(ssl)) <= 0)
  {
    herror_t err;

    log_error2 ("SSL_accept failed (%s)", _hssl_get_error(ssl, ret));

    err = herror_new("hssl_server_ssl", HSSL_ERROR_SERVER, "SSL_accept failed (%s)", _hssl_get_error(ssl, ret));
    SSL_free (ssl);

    return err;
  }

  sock->ssl = ssl;

  return H_OK;
}


void
hssl_cleanup(hsocket_t *sock)
{

  if (sock->ssl)
  {
    SSL_shutdown (sock->ssl);
    SSL_free (sock->ssl);
    sock->ssl = NULL;
  }

  return;
}


herror_t hssl_read(hsocket_t *sock, char *buf, size_t len, size_t *received)
{
  int count;

  log_verbose4("sock->sock=%d sock->ssl=%p, len=%li", sock->sock, sock->ssl, len);

  if (sock->ssl)
  {
    if ((count = SSL_read(sock->ssl, buf, len)) == -1)
      return herror_new("SSL_read", HSOCKET_ERROR_RECEIVE, "SSL_read failed (%s)", _hssl_get_error(sock->ssl, count));
  }
  else
  {
    if ((count = recv(sock->sock, buf, len, 0)) == -1)
      return herror_new("hssl_read", HSOCKET_ERROR_RECEIVE, "recv failed (%s)", strerror(errno));
  }
  *received = count;

  return H_OK;
}


herror_t hssl_write(hsocket_t *sock, const char *buf, size_t len, size_t *sent)
{
  int count;

  log_verbose4("sock->sock=%d, sock->ssl=%p, len=%li", sock->sock, sock->ssl, len);

  if (sock->ssl)
  {
    if ((count = SSL_write(sock->ssl, buf, len)) == -1)
      return herror_new("SSL_write", HSOCKET_ERROR_SEND, "SSL_write failed (%s)", _hssl_get_error(sock->ssl, count));
  }
  else
  {
    if ((count = send(sock->sock, buf, len, 0)) == -1)
      return herror_new("hssl_write", HSOCKET_ERROR_SEND, "send failed (%s)", strerror(errno));
  }
  *sent = count;

  return H_OK;
}

#else

herror_t hssl_read(hsocket_t *sock, char *buf, size_t len, size_t *received)
{
  int count;

  if ((count = recv(sock->sock, buf, len, 0)) == -1)
    return herror_new("hssl_read", HSOCKET_ERROR_RECEIVE, "recv failed (%s)", strerror(errno));
  *received = count;
  return H_OK;
}


herror_t hssl_write(hsocket_t *sock, const char *buf, size_t len, size_t *sent)
{
  int count;

  if ((count = send(sock->sock, buf, len, 0)) == -1)
    return herror_new("hssl_write", HSOCKET_ERROR_SEND, "send failed (%s)", strerror(errno));
  *sent = count;
  return H_OK;
}

#endif
