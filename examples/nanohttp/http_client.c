/******************************************************************
*  $Id: http_client.c,v 1.10 2007/11/03 22:40:09 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C (example)
* Copyright (C) 2003-2004  Ferhat Ayaz
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nanohttp/nanohttp-logging.h>
#include <nanohttp/nanohttp-client.h>

#define MAX_BUFFER_SIZE 1024

static int show_http_status_code = 0;
static int show_headers = 0;
static int show_content = 1;

static char *username = NULL;
static char *password = NULL;

static void show_response(hresponse_t *res)
{
  unsigned char buffer[MAX_BUFFER_SIZE+1];
  int len;

  if (!res)
  {
    fprintf(stderr, "hresponse_t is NULL!\n");
    return;
  }
  
  if (res->errcode != 200 || show_http_status_code)
    printf("HTTP Status: %d \"%s\"\n", res->errcode, res->desc);

  if (show_headers)
  {
    hpair_t *pair;

    printf("HTTP Headers:\n");
    for (pair = res->header; pair; pair=pair->next)
      printf("  %s: %s\n", pair->key, pair->value);
  }

  if (!res->in)
  {
    fprintf(stderr, "No input stream!\n");
    return;
  }

  while (http_input_stream_is_ready(res->in))
  {
    len = http_input_stream_read(res->in, buffer, MAX_BUFFER_SIZE);

    if (show_content)
      fwrite(buffer, len, 1, stdout);
  }
  return;
}

int main(int argc, char **argv)
{
  httpc_conn_t *conn; /* Client connection object */
  hresponse_t *res; /* Response object **/
  herror_t status;
  int i;

  /* check usage */
  if (argc < 2)
  {
    fprintf(stderr, "usage: %s [-headers] [-status] [-noout] [-username name] [-password secret] <url>\n", argv[0]);
    exit(1);
  }

  /* XXX: this is not safe... */
  for (i=0; i<argc; i++)
  {
    if (!strcmp("-headers", argv[i]))
      show_headers = 1;
    else if (!strcmp("-status", argv[i]))
      show_http_status_code = 1;
    else if (!strcmp("-noout", argv[i]))
      show_content = 0;
    else if (!strcmp("-username", argv[i]))
      username = argv[i+1];
    else if (!strcmp("-password", argv[i]))
      password = argv[i+1];
  }

  /* Set log level to see more information written by the library */
  nanohttp_log_set_loglevel(NANOHTTP_LOG_INFO);

  /* Initialize httpc module */
  if (httpc_init(argc, argv))
  {
    fprintf(stderr, "Cannot inititialize HTTP client\n");
    exit(1);
  }

  /* Create the client connection object */
  if (!(conn = httpc_new()))
  {
    fprintf(stderr, "httpc_new failed\n");
    exit(1);
  }
  
  /* set the credentials, if specified */
  if (username || password)
  {
    httpc_set_basic_authorization(conn, username, password);
  }

  /* Send GET method and receive response */
  if ((status = httpc_get(conn, &res, argv[argc-1])) != H_OK)
  {
    fprintf(stderr, "httpc_get failed (%s)\n", herror_message(status));
    herror_release(status);
    exit(1);
  }

  /* Show response */
  show_response(res);

  /* Clean up */
  hresponse_free(res);

  httpc_free(conn);

  return 0;
}
