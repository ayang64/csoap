/******************************************************************
 *  $Id: nanohttp-reqres.h,v 1.1 2004/09/19 07:05:03 snowdrop Exp $
 *
 * CSOAP Project:  A http client/server library in C
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
 * Email: ferhatayaz@yahoo.com
 ******************************************************************/
#ifndef NANO_HTTP_REQRES_H 
#define NANO_HTTP_REQRES_H 

#include <nanohttp/nanohttp-stream.h>

/*
  request object
 */
typedef struct hrequest
{
  char *method;
  char *path;
  char *spec;
  hpair_t *query;
  hpair_t *header;
}hrequest_t;

hrequest_t *hrequest_new_from_buffer(char *data);
void hrequest_free(hrequest_t *req);

/* response object */
typedef struct hresponse
{
  char spec[10];
  int errcode;
  char *desc;
  hpair_t *header;
  char *body;
  long bodysize;
  http_input_stream_t *in;
}hresponse_t;

/*
  PARAMS
  buffer: The hole received data from socket.
 */
hresponse_t *hresponse_new_from_buffer(const char* buffer);
hresponse_t *hresponse_new_from_socket(hsocket_t sock);
hresponse_t *hresponse_new();
void hresponse_free(hresponse_t *res); 



#endif






