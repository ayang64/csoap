/******************************************************************
 *  $Id: nanohttp-socket.h,v 1.5 2004/01/21 12:28:20 snowdrop Exp $
 *
 * CSOAP Project:  A http client/server library in C
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
#ifndef NANO_HTTP_SOCKET_H 
#define NANO_HTTP_SOCKET_H 


#define HSOCKET_OK 0
#define HSOCKET_CAN_NOT_CREATE 1001
#define HSOCKET_CAN_NOT_GET_HOSTNAME 1002
#define HSOCKET_CAN_NOT_CONNECT 1003
#define HSOCKET_CAN_NOT_SEND 1004
#define HSOCKET_CAN_NOT_RECEIVE 1005
#define HSOCKET_CAN_NOT_BIND 1006
#define HSOCKET_CAN_NOT_LISTEN 1007
#define HSOCKET_CAN_NOT_ACCEPT 1008

#define HSOCKET_MAX_BUFSIZE 1024

typedef int hsocket_t;

/*
  PROTOTYPE:
  int my_recv_cb(hsocket_t sock, char *buffer, int size, void *userdata);
  returns 1 to continue 0 to stop receiving.
 */
typedef int (*hsocket_recv_callback)(hsocket_t, char *, int, void*);


/*
  hsocket_module_init
  Returns 0 if success.
  >0 if fail.
 */
int hsocket_module_init();
void hsocket_module_destroy();


/*
  hsocket_init
  Returns 0 if success.
  >0 if fail.
 */
int hsocket_init(hsocket_t *sock);
void hsocket_free(hsocket_t sock);


/*
  hsocket_open: create and connect a socket
  Returns 0 if success
  >0 if fail.
 */
int hsocket_open(hsocket_t *sock, const char* host, int port);
void hsocket_close(hsocket_t sock);

/*
  hsocket_bind: create and bind a socket
  Returns 0 if success
  >0 if fail.
 */
int hsocket_bind(hsocket_t *sock, int port);

/*
  Listen to socket. Must be called after bind
 */
int hsocket_listen(hsocket_t sock, int n);

int hsocket_accept(hsocket_t sock, hsocket_t *dest);


/*
  hsocket_nsend
  sends n bytes of data
  Returns 0 if success
  >0 if fail
 */
int hsocket_nsend(hsocket_t sock, const char* buffer, int n); 

/*
  hsocket_send
  sends strlen(buffer) bytes of data
  Returns 0 if success
  >0 if fail
 */
int hsocket_send(hsocket_t sock, const char* buffer); 


/*
  hsocket_recv
  receives everything quequed on the socket.
  Sets *buffer to the received buffer.
  Sets size to the received size.
  You must free the buffer manually with free().
  If buffer is non zero, this functions assumes that
  buffer is valid and just reallocates the given buffer.
  If buffer is zero (like in the following example),
  the buffer will be allocated first.

  Example:

   int size;
   char *buffer;
   hsocket_t sock;
   
   buffer = 0; 
   sock = ...

   if (!hsocket_recv(sock, &buffer, &size)) {
    printf("Received total: %d\n", size);
    printf("Received: '%s'\n", buffer);

    free(buffer);
   } else {
    printf("Error receiving data\n");
   }

  Returns 0 if success
  >0 if fail
 */
int hsocket_recv(hsocket_t sock, char** buffer, int *size); 

int hsocket_recv_limit(hsocket_t sock, char** buffer, 
		       const char* delim, char **rest, 
		       int *totalBuffer, int *totalRest);

/*
  returns 1 to continue, 0 to break;
 */
int hsocket_recv_cb(hsocket_t sock, 
		    hsocket_recv_callback cb, void *userdata);

int hsocket_read(hsocket_t sock, char* buffer, int total, int force);


/* ======================================== */
/*  Buffered socket                         */
/* ======================================== */
typedef struct _bufsocket
{
  hsocket_t sock;
  char *buffer;
  int bufsize;
  int cur;
}hbufsocket_t;


int hbufsocket_read(hbufsocket_t *bufsock, char *buffer, int size);

#endif











