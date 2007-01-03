/******************************************************************
*  $Id: mime_client.c,v 1.1 2007/01/03 21:31:44 m0gg Exp $
*
* CSOAP Project:  A http client/server library in C (example)
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
* Email: ferhatayaz@yahoo.com
******************************************************************/
#include <stdio.h>
#include <errno.h>

#include <nanohttp/nanohttp-client.h>

#define MAX_BUFFER_SIZE 1024

static
void show_response(hresponse_t *res)
{
  hpair_t *pair;
  char buffer[MAX_BUFFER_SIZE+1];
  int len;

  if (res == NULL) 
  {
    fprintf(stderr, "Response is NULL\n");
    return;
  }
  
  fprintf(stdout, "Version: '%d'\n", res->version);
  fprintf(stdout, "Status: %d\n", res->errcode);
  fprintf(stdout, "Desc: '%s'\n", res->desc);

  for (pair = res->header; pair; pair = pair->next)
  {
    fprintf(stdout, "%s: %s\n", pair->key, pair->value);
  }

  if (res->in == NULL)
  {
    fprintf(stderr, "No input stream!\n");
    return;
  }
  
  while (http_input_stream_is_ready(res->in))
  {
    len = http_input_stream_read(res->in, buffer, MAX_BUFFER_SIZE);
    fwrite(buffer, len, 1, stdout);
  }

  return;
}

static
int send_file(httpc_conn_t *conn, const char* filename, const char* id, const char* content_type)
{
  herror_t status;
  int size;
  FILE *f = fopen(filename, "r");
  char buffer[MAX_BUFFER_SIZE];

  if ((f = fopen(filename, "r")) == NULL)
  {
    fprintf(stderr, "cannot open file: '%s' (%s)\n", filename, strerror(errno));
    return -1;
  }

  if ((status = httpc_mime_next(conn, id, content_type, "binary")) != H_OK)
  {
    fprintf(stderr, "httpc_mime_next failed (%s)\n", herror_message(status));
    herror_release(status);
    return -1;
  }
  
  while (!feof(f))
  {
    size = fread(buffer, 1, MAX_BUFFER_SIZE, f);
    if (size == -1)
    {
      fprintf(stderr, "Cannot read file (%s)\n", strerror(errno));
      fclose(f);
      return -1;
    }
    http_output_stream_write(conn->out, buffer, size);
  }

  fclose(f);
  return 0;
}

int main(int argc, char **argv)
{
  httpc_conn_t *conn; /* Client connection object */
  hresponse_t *res; /* Response object **/

  herror_t status;
  FILE *f;
  size_t size;
  char url[50], file[50], id[50], content_type[50];
  
  char buffer[MAX_BUFFER_SIZE+1];

  /* Check usage */
  if (argc < 5)
  {
    fprintf(stderr, "usage %s <url> <file> <id> <content-type>\n", argv[0]);
    exit(1);
  }

  /* Set log level to see more information written by the library */
  // log_set_level(HLOG_VERBOSE);

  /* Initialize httpc module */
  if ((status = httpc_init(argc, argv)) != H_OK)
  {
    fprintf(stderr, "Cannot init httpc (%s)\n", herror_message(status));
    herror_release(status);
    return 1;
  }

  /* Create the client connection object */
  conn = httpc_new();

  httpc_set_header(conn, HEADER_TRANSFER_ENCODING, TRANSFER_ENCODING_CHUNKED);

  /*
   Open connection for mime
   */
  if ((status = httpc_mime_begin(conn, argv[1], argv[3], "", argv[4])) != H_OK)
  {
    log_error2("Can not start MIME: %s\n", herror_message(status));
    herror_release(status);
    exit(1);
  }

  if (!send_file(conn,  argv[2], argv[3], argv[4]))
  {
    fprintf(stderr, "send_file failed\n");
    exit(1);
  }

  while (1)
  {
    printf("Enter filename ['.' for finish]: ");
    gets(file);
    if (!strcmp(file, "."))
       break;

    printf("Enter part id:");
    gets(id);
    printf("Enter content-type:");
    gets(content_type);

    if (!send_file(conn, file, id, content_type))
       exit(1);
  }

  if ((status = httpc_mime_end(conn, &res)) != H_OK)
  {
    fprintf(stderr, "httpc_mime_end failed (%s)\n", herror_message(status));
    herror_release(status);
    exit(1);
  }

   /* Show response */
  show_response(res);

  /* clean up*/
  hresponse_free(res);
  httpc_free(conn);
  return 0;
}
