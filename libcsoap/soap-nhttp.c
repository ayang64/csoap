/******************************************************************
*  $Id: soap-nhttp.c,v 1.12 2007/01/01 18:58:05 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
* Copyright (C) 2003  Ferhat Ayaz
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
* Email: ayaz@jprogrammer.net
******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
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

#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/xpath.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-response.h>

#include <nanohttp/nanohttp-client.h>
#include <nanohttp/nanohttp-server.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-client.h"
#include "soap-transport.h"
#include "soap-addressing.h"
#include "soap-xml.h"
#include "soap-router.h"
#include "soap-server.h"

#include "soap-admin.h"
#include "soap-wsil.h"

#include "soap-nhttp.h"

static herror_t
_soap_nhttp_send_document(httpd_conn_t *conn, xmlDocPtr doc)
{
  char length[16];
  xmlChar *buf;
  int size;

  xmlDocDumpMemory(doc, &buf, &size);

  sprintf(length, "%d", size);
  httpd_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");
  httpd_set_header(conn, HEADER_CONTENT_LENGTH, length);
  httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);

  http_output_stream_write(conn->out, buf, size);

  xmlFree(buf);

  return H_OK;
}

static herror_t
_soap_nhttp_send_fault(httpd_conn_t *conn, const char *message)
{
  xmlDocPtr doc;
  herror_t ret;

  doc = soap_fault_build(SOAP_FAULT_SENDER, message, soap_transport_get_name(), NULL);
  ret = _soap_nhttp_send_document(conn, doc);
  xmlFreeDoc(doc);

  return ret;
}

static int
_soap_nhttp_xml_io_read(void *ctx, char *buffer, int len)
{
  int ret;
  struct http_input_stream_t *in;
 
  in = (struct http_input_stream_t *)ctx;
  if (!http_input_stream_is_ready(in))
    return 0;

  if ((ret = http_input_stream_read(in, buffer, len)) == -1)
    return 0;

  return ret;
}

static int
_soap_nhttp_xml_io_close(void *ctx)
{
  /* nothing */
  return 0;
}

static herror_t
_soap_nhttp_env_new_from_stream(struct http_input_stream_t *in, struct SoapEnv **out)
{
  xmlDocPtr doc;

  doc = xmlReadIO(_soap_nhttp_xml_io_read, _soap_nhttp_xml_io_close, in, "", NULL, 0);
  if (in->err != H_OK)
    return in->err;

  if (doc == NULL)
    return herror_new("_soap_nhttp_env_new_from_stream", XML_ERROR_PARSE, "Trying to parse invalid XML");

  return soap_env_new_from_doc(doc, out);
}

static void
soap_nhttp_process(httpd_conn_t * conn, struct hrequest_t * req)
{
  char *action;
  struct SoapEnv *env;
  struct SoapCtx *request;
  struct SoapCtx *response;
  herror_t err;

  if (req->method == HTTP_REQUEST_GET)
  {
    struct SoapRouter *router;

    router = soap_server_find_router(req->path);
    if (router && router->description)
    {
      _soap_nhttp_send_document(conn, router->description);
      return;
    }
  }

  if (req->method != HTTP_REQUEST_POST)
  {
    httpd_send_not_implemented(conn, "I only speak with 'POST' method.");
    return;
  }

  if ((err = _soap_nhttp_env_new_from_stream(req->in, &env)) != H_OK)
  {
    _soap_nhttp_send_fault(conn, herror_message(err));
    herror_release(err);
    return;
  }

  if (env == NULL)
  {
    _soap_nhttp_send_fault(conn, "Cannot receive POST data!");
    return;
  }

  request = soap_ctx_new(env);

  if ((action = hpairnode_get_ignore_case(req->header, SOAP_NHTTP_SOAP_ACTION)))
  {
    soap_addressing_set_action_string(env, action);
  }

  /* xmlDocFormatDump(stdout, ctx->env->root->doc, 1); */

  soap_ctx_add_files(request, req->attachments);

  /* only local part is interesting...
  soap_addressing_set_to_address_string(ctx->env, req->path); */

  soap_transport_process(request, &response);

  _soap_nhttp_send_document(conn, response->env->root->doc);

  soap_ctx_free(response);

  soap_ctx_free(request);

  return;
}

herror_t
soap_nhttp_server_init_args(int argc, char **argv)
{
  herror_t err;
 
  if ((err = httpd_init(argc, argv)) != H_OK)
  {
    log_error2("httpd_init failed (%s)", herror_message(err));
    return err;
  }

  if ((err = soap_wsil_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_wsil_init_args failed (%s)", herror_message(err));
    return err;
  }

  if ((err = soap_admin_init_args(argc, argv)) != H_OK)
  {
    log_error2("soap_admin_init_args failed (%s)", herror_message(err));
    return err;
  }

  return H_OK;
}

static herror_t
_soap_nhttp_client_build_result(hresponse_t * res, struct SoapEnv ** env)
{
  log_verbose2("Building result (%p)", res);

  if (res == NULL)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "hresponse_t is NULL");

  if (res->in == NULL)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "Empty response from server");

  if (res->errcode != 200)
    return herror_new("_soap_client_build_result",
                      GENERAL_INVALID_PARAM, "HTTP code is not OK (%i)", res->errcode);

  return _soap_nhttp_env_new_from_stream(res->in, env);
}

static herror_t
_soap_nhttp_client_invoke(void *unused, struct SoapCtx *request, struct SoapCtx **response)
{
  herror_t status;

  /* Buffer variables */
  xmlChar *buffer;
  int size;
  char tmp[15];
  char *action;
  char *url;
  struct SoapEnv *res_env;

  /* Transport variables */
  httpc_conn_t *conn;
  hresponse_t *res;

  /* multipart/related start id */
  char start_id[150];
  static int counter = 1;
  struct part_t *part;

  /* for copy attachments */
  char href[MAX_HREF_SIZE];

  /* log_verbose1("nanohttp client"); */

  xmlDocDumpMemory(request->env->root->doc, &buffer, &size);

  /* xmlDocFormatDump(stdout, request->env->root->doc, 1); */

  /* Transport via HTTP */
  if (!(conn = httpc_new()))
  {
    return herror_new("soap_client_invoke", SOAP_ERROR_CLIENT_INIT, "Unable to create HTTP client!");
  }

  if ((action = soap_addressing_get_action_string(request->env)))
    httpc_set_header(conn, SOAP_NHTTP_SOAP_ACTION, action);
  else
    httpc_set_header(conn, SOAP_NHTTP_SOAP_ACTION, "");
  log_verbose2("action is \"%s\"", action);
  free(action);

  httpc_set_header(conn, HEADER_CONNECTION, "Close");

  if (!(url = soap_addressing_get_to_address_string(request->env)))
    return herror_new("soap_nhttp_client_invoke", 0, "Missing client URL");
  log_verbose2("url is \"%s\"", url);

  if (!request->attachments)
  {
    /* content-type is always 'text/xml' */
    httpc_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");

    sprintf(tmp, "%d", size);
    httpc_set_header(conn, HEADER_CONTENT_LENGTH, tmp);

    if ((status = httpc_post_begin(conn, url)) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }

    if ((status = http_output_stream_write(conn->out, buffer, size)) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }

    if ((status = httpc_post_end(conn, &res)) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }
  }
  else
  {
    httpc_set_header(conn, HEADER_TRANSFER_ENCODING, TRANSFER_ENCODING_CHUNKED);

    sprintf(start_id, "289247829121218%d", counter++);
    if ((status = httpc_mime_begin(conn, url, start_id, "", "text/xml")) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }

    if ((status = httpc_mime_next(conn, start_id, "text/xml", "binary")) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }

    if ((status = http_output_stream_write(conn->out, buffer, size)) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }

    for (part=request->attachments->parts; part; part=part->next)
    {
      if ((status = httpc_mime_send_file(conn, part->id, part->content_type, part->transfer_encoding, part->filename)) != H_OK)
      {
        log_error2("httpc_mime_send_file failed (%s)", herror_message(status));
	httpc_close_free(conn);
	xmlFree(buffer);
	return status;
      }
    }
    
    if ((status = httpc_mime_end(conn, &res)) != H_OK)
    {
      httpc_close_free(conn);
      xmlFree(buffer);
      return status;
    }
  }

  xmlFree(buffer);
  free(url);

  if ((status = _soap_nhttp_client_build_result(res, &res_env)) != H_OK)
  {
    hresponse_free(res);
    httpc_close_free(conn);
    return status;
  }

  *response = soap_ctx_new(res_env);
  /* soap_ctx_add_files(*response, res->attachments) */

  if (res->attachments != NULL)
  {
    part = res->attachments->parts;
    while (part)
    {
      soap_ctx_add_file(*response, part->filename, part->content_type, href);
      part->deleteOnExit = 0;
      part = part->next;
    }
  }

  hresponse_free(res);
  httpc_close_free(conn);

  /* log_verbose1("done"); */

  return H_OK;
}

herror_t
soap_nhttp_client_init_args(int argc, char **argv)
{
  herror_t status;

  if ((status = httpc_init(argc, argv)) != H_OK)
  {
    log_error2("httpc_init failed (%s)", herror_message(status));
    return status;
  }

  soap_transport_add("https", NULL, _soap_nhttp_client_invoke);
  soap_transport_add("http", NULL, _soap_nhttp_client_invoke);

  return H_OK;
}

herror_t
soap_nhttp_register(const char *context)
{
  herror_t status;

  if ((status = httpd_register(context, soap_nhttp_process)) != H_OK)
  {
    log_error2("httpd_register_secure failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t
soap_nhttp_server_run(void)
{
  return httpd_run();
}

void
soap_nhttp_server_destroy(void)
{
  httpd_destroy();

  return;
}

void
soap_nhttp_client_destroy(void)
{
  httpc_destroy();

  return;
}

short
soap_nhttp_get_port(void)
{
  return httpd_get_port();
}

const char *
soap_nhttp_get_protocol(void)
{
  return httpd_get_protocol();
}
