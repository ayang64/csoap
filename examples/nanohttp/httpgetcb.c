/******************************************************************
 *  $Id: httpgetcb.c,v 1.1 2003/12/11 14:52:14 snowdrop Exp $
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
 * Email: ayaz@jprogrammer.net
 ******************************************************************/
#include <nanohttp/nanohttp-client.h>

#include <stdio.h>

FILE *fd;

void my_start_callback(httpc_conn_t *conn, void *userdata, 
		    hpair_t *header, const char *spec,
		    int errcode, const char *desc)
{
  hpair_t *pair;

  log_debug2("Spec  : '%s'", spec);
  log_debug2("Status: %d", errcode);
  log_debug2("Desc  : '%s'", desc);

  pair = header;
  while (pair != NULL) {
    log_debug3("%s: %s", pair->key, pair->value);
    pair = pair->next;
  }

  log_debug1("\n");
}

void my_callback(int counter, httpc_conn_t* conn,
		 void *userdata, int size, char *buffer)
{
  /* print body */
  if (size > 0) fwrite(buffer, size, 1, fd);
}


int main(int argc, char *argv[])
{
  httpc_conn_t *conn;

  if (argc < 3) {
    fprintf(stderr, "usage %s <url> <filename>\n", argv[0]);
    exit(1);
  }

  fd = fopen(argv[2], "w");
  if (fd == NULL) {
    fprintf(stderr, "can not open '%s'\n", argv[2]);
    exit(1);
  }

  conn = httpc_new();
  httpc_get_cb(conn, argv[1], my_start_callback, my_callback, NULL);
  httpc_free(conn);

  fclose(fd);

  log_debug1("finished");

  return 0;
}







