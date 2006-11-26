/******************************************************************
*  $Id: soap-nudp.c,v 1.5 2006/11/26 20:13:05 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-server.h"
#include "soap-transport.h"
#include "soap-addressing.h"
#include "soap-nudp.h"

static short _soap_nudp_port = NUDP_DEFAULT_PORT;
static int _soap_nudp_socket;
static pthread_t _soap_nudp_thread;
static pthread_attr_t _soap_nudp_attr;

static short
_soap_nudp_server_set_port(void)
{
  struct servent *entry;

  if (!(entry = getservbyname("soap", "udp")))
  {
    log_warn1("getservbyname returned NULL");
    _soap_nudp_port = NUDP_DEFAULT_PORT;  
  }
  else
  {
    _soap_nudp_port = entry->s_port;
  }
  return _soap_nudp_port;
}

static void
_soap_nudp_server_parse_arguments(int argc, char **argv)
{
  int i;

  for (i=1; i<argc; i++)
  {
    if (!strcmp(argv[i - 1], NUDP_ARG_PORT))
    {
      _soap_nudp_port = atoi(argv[i]);
    }
  }

  log_verbose2("socket bind to port \"%d\"", _soap_nudp_port);

  return;
}

static herror_t
_soap_nudp_send_document(int socket, xmlDocPtr doc, const struct sockaddr *addr, socklen_t addr_len)
{
  xmlChar *buf;
  int size;

  xmlDocDumpMemory(doc, &buf, &size);
  if (sendto(socket, buf, size, 0, addr, addr_len) < 0)
  {
    log_error2("sendto failed (%s)", strerror(errno));
    return herror_new("soap_nudp_client_invoke", 0, "Cannot send message");
  }
  xmlFree(buf);

  return H_OK;
}

static herror_t
_soap_nudp_receive_document(int socket, xmlDocPtr *doc, struct sockaddr *addr, socklen_t *addr_len)
{
  int cnt;
  char buf[4096];

  if ((cnt = recvfrom(socket, buf, 4096, 0, addr, addr_len)) < 0)
  {
    log_error2("recvfrom failed (%s)", strerror(errno));
    return herror_new("_soap_nudp_receive_document", 0, "Receiving document failed");
  }

  if (!(*doc = xmlReadDoc(buf, NULL, NULL, XML_PARSE_NONET)))
  {
    log_error1("xmlReadDoc failed");
    return herror_new("_soap_nudp_receive_document", 0, "Cannot parse received data");
  }

  return H_OK;
}

static herror_t
_soap_nudp_client_invoke(void *unused, struct SoapCtx *req, struct SoapCtx **res)
{
  xmlURI *to;
  xmlDocPtr doc;
  int sd;
  herror_t status;
  struct sockaddr_in addr;

  if (!(to = soap_addressing_get_to_address(req->env)))
  {
    log_error1("soap_addressing_get_to_address returned NULL");
    return herror_new("soap_nudp_client_invoke", 0, "Destination address is missing");
  }

  bzero(&addr, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  if (to->port == 0)
    addr.sin_port = htons(NUDP_DEFAULT_PORT);
  else
    addr.sin_port = htons(to->port);

  if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1)
  {
    log_error2("inet_pton failed (%s)", strerror(errno));
    return herror_new("soap_nudp_client_invoke", 0, "Cannot resolve destination address");
  }

  if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    log_error2("socket failed (%s)", strerror(errno));
    return herror_new("soap_nudp_client_invoke", 0, "Cannot create socket");
  }

  _soap_nudp_send_document(sd, req->env->root->doc, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

  if ((status = _soap_nudp_receive_document(sd, &doc, NULL, NULL)) != H_OK)
  {
    log_error2("_soap_nudp_receive_document failed (%s)", herror_message(status));
    return status;
  }

  if ((status = soap_env_new_from_doc(doc, &(*res)->env)) != H_OK)
  {
    log_error2("soap_env_new_from_doc failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
soap_nudp_server_init_args(int argc, char **argv)
{
  struct sockaddr_in addr;

  _soap_nudp_server_set_port();

  _soap_nudp_server_parse_arguments(argc, argv);
 
  if ((_soap_nudp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    log_error2("socket failed (%s)", strerror(errno));
    return herror_new("soap_nudp_server_init_args", 0, "Cannot create socket (%s)", strerror(errno));
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(_soap_nudp_port);

  if (bind(_soap_nudp_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
  {
    log_error2("bind failed (%s)", strerror(errno));
    return herror_new("soap_nudp_server_init_args", 0, "Cannot bind socket (%s)", strerror(errno));
  }

  return H_OK;
}

herror_t
soap_nudp_register(const void *data)
{
  return H_OK;
}

void *
soap_nudp_server_run(void *unused)
{
  xmlDocPtr doc;
  struct sockaddr addr;
  socklen_t addr_len;
  struct SoapCtx *req;
  struct SoapCtx *res;
  xmlURI *to;

  for(;;)
  {
    _soap_nudp_receive_document(_soap_nudp_socket, &doc, &addr, &addr_len);

    xmlDocFormatDump(stdout, doc, 1);

    req = soap_ctx_new(NULL);

    soap_env_new_from_doc(doc, &(req->env));

    /* only local part is interesting */
    to = soap_addressing_get_to_address(req->env);
    soap_addressing_set_to_address_string(req->env, to->path);

    xmlFreeDoc(doc);

    soap_transport_process(req, &res);

    _soap_nudp_send_document(_soap_nudp_socket, res->env->root->doc, &addr, addr_len);

    soap_ctx_free(res);

    soap_ctx_free(req);
  }
 
  return NULL;
}

herror_t
soap_nudp_server_run_threaded(void)
{
  int err;
  
  if ((err = pthread_create(&_soap_nudp_thread, &_soap_nudp_attr, soap_nudp_server_run, NULL)) < 0)
  {
    log_error2("pthread_create failed (%s)", strerror(err));
    return herror_new("soap_nudp_server_run_threaded", 0, "pthread_create failed (%s)", strerror(err));
  }

  return H_OK;
}

void
soap_nudp_server_destroy(void)
{

  close(_soap_nudp_socket);

  return;
}


herror_t
soap_nudp_client_init_args(int argc, char **argv)
{
  soap_transport_add("soap.udp", NULL, _soap_nudp_client_invoke);

  return H_OK;
}

void
soap_nudp_client_destroy(void)
{
  return;
}
