/******************************************************************
 *  $Id: nanohttp-common.h,v 1.11 2004/09/19 07:05:03 snowdrop Exp $
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
#ifndef NANO_HTTP_COMMON_H 
#define NANO_HTTP_COMMON_H 


#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"
#define HEADER_CONTENT_ID "Content-Id"
#define HEADER_CONTENT_TRANSFER_ENCODING "Content-Transfer-Encoding"
#define HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HEADER_CONNECTION "Connection"

#define HEADER_HOST "Host"
#define HEADER_DATE "Date"
#define HEADER_ACCEPT "Accept"

#ifndef SAVE_STR
#define SAVE_STR(str) ((str==0)?("(null)"):(str))
#endif

#define TRANSFER_ENCODING_CHUNKED "chunked"

#define MAX_HEADER_SIZE 4256
#define MAX_SOCKET_BUFFER_SIZE 4256

/*
Set Sleep function platform depended
*/
#ifdef WIN32
  #define system_sleep(seconds) Sleep(seconds*1000);
#else
  #define system_sleep(seconds) sleep(seconds);
#endif

typedef unsigned char byte_t;

/*
  string function to compare strings ignoring case
  Returns 1 if s1 equals s2 and 0 otherwise.  
 */
int strcmpigcase(const char *s1, const char *s2);



/*
  hpairnode_t represents a pair (key, value) pair.
  This is also a linked list.
 */
typedef struct hpair hpair_t;
struct hpair
{
  char *key;
  char *value;
  hpair_t *next;
};


hpair_t *hpairnode_new(const char* key, const char* value, hpair_t* next);
void hpairnode_free(hpair_t *pair);
char *hpairnode_get(hpair_t *pair, const char* key);
char *hpairnode_get_ignore_case(hpair_t *pair, const char* key);
hpair_t* hpairnode_copy(const hpair_t *src);
hpair_t* hpairnode_copy_deep(const hpair_t *src);
void hpairnode_dump_deep(hpair_t *pair);
void hpairnode_dump(hpair_t *pair);

typedef enum http_version { 
  HTTP_1_0, 
  HTTP_1_1 /* default */
}http_version_t;

typedef enum hreq_method 
{
  HTTP_REQUEST_POST,
  HTTP_REQUEST_GET
}hreq_method ;


typedef struct hurl
{
  char *protocol;
  char *host;
  int port;
  char *context;
}hurl_t;


hurl_t* hurl_new(const char* urlstr);
void hurl_free(hurl_t *url);



/*
  DIME common stuff
*/
#define DIME_VERSION_1      0x08
#define DIME_FIRST_PACKAGE  0x04
#define DIME_LAST_PACKAGE   0x02
#define DIME_CHUNKED        0x01
#define DIME_TYPE_URI       0x2

typedef struct _DIME_PACKAGE
{
  char version; /* Specifies the version of the DIME message */
  char first_record; /* Specifies that this record is the first record of the message */
  char last_recored; /* Specifies that this record is the last record of the message */
  char chunked; /* Specifies that the contents of the message have been chunked */
  char type_t; /* Specifies the structure and format of the TYPE field */
  char *options; /* Contains any optional information used by a DIME parser */
  char *id; /* Contains a URI for uniquely identifying a DIME payload with any 
                 additional padding; */
  char *type; /* Specifies the encoding for the record based on a type reference URI or a MIME media-type */
  int data_size; /* Specifies the length (in bytes) of the DATA */
  unsigned char* data; /* Contains the actual data payload for the record; 
                          format of the data depends on the type specified for the record */
}DIME_PACKAGE;


/* logging stuff*/
typedef enum log_level
{
  HLOG_VERBOSE,
  HLOG_DEBUG,
  HLOG_INFO,
  HLOG_WARN,
  HLOG_ERROR,
  HLOG_FATAL
}log_level_t;



log_level_t log_set_level(log_level_t level);
log_level_t log_get_level();


#ifdef WIN32
  #ifndef __MINGW32__ 
    #define __FUNCTION__  "***"
  #endif
#endif

#define log_verbose1(a1) log_verbose(__FUNCTION__, a1)
#define log_verbose2(a1,a2) log_verbose(__FUNCTION__, a1,a2)
#define log_verbose3(a1,a2,a3) log_verbose(__FUNCTION__, a1,a2,a3)
#define log_verbose4(a1,a2,a3,a4) log_verbose(__FUNCTION__, a1,a2,a3,a4)
#define log_verbose5(a1,a2,a3,a4,a5) log_verbose(__FUNCTION__, a1,a2,a3,a4,a5)

#define log_debug1(a1) log_debug(__FUNCTION__, a1)
#define log_debug2(a1,a2) log_debug(__FUNCTION__, a1,a2)
#define log_debug3(a1,a2,a3) log_debug(__FUNCTION__, a1,a2,a3)
#define log_debug4(a1,a2,a3,a4) log_debug(__FUNCTION__, a1,a2,a3,a4)
#define log_debug5(a1,a2,a3,a4,a5) log_debug(__FUNCTION__, a1,a2,a3,a4,a5)

#define log_info1(a1) log_info(__FUNCTION__, a1)
#define log_info2(a1,a2) log_info(__FUNCTION__, a1,a2)
#define log_info3(a1,a2,a3) log_info(__FUNCTION__, a1,a2,a3)
#define log_info4(a1,a2,a3,a4) log_info(__FUNCTION__, a1,a2,a3,a4)
#define log_info5(a1,a2,a3,a4,a5) log_info(__FUNCTION__, a1,a2,a3,a4,a5)

#define log_warn1(a1) log_warn(__FUNCTION__, a1)
#define log_warn2(a1,a2) log_warn(__FUNCTION__, a1,a2)
#define log_warn3(a1,a2,a3) log_warn(__FUNCTION__, a1,a2,a3)
#define log_warn4(a1,a2,a3,a4) log_warn(__FUNCTION__, a1,a2,a3,a4)
#define log_warn5(a1,a2,a3,a4,a5) log_warn(__FUNCTION__, a1,a2,a3,a4,a5)

#define log_error1(a1) log_error(__FUNCTION__, a1)
#define log_error2(a1,a2) log_error(__FUNCTION__, a1,a2)
#define log_error3(a1,a2,a3) log_error(__FUNCTION__, a1,a2,a3)
#define log_error4(a1,a2,a3,a4) log_error(__FUNCTION__, a1,a2,a3,a4)
#define log_error5(a1,a2,a3,a4,a5) log_error(__FUNCTION__, a1,a2,a3,a4,a5)

void log_verbose(const char* FUNC, const char *format, ...);
void log_debug(const char* FUNC, const char *format, ...);
void log_info(const char* FUNC, const char *format, ...);
void log_warn(const char* FUNC, const char *format, ...);
void log_error(const char* FUNC, const char *format, ...);




#endif






