/******************************************************************
 *  $Id: nanohttp-common.h,v 1.18 2004/11/05 11:58:01 snowdrop Exp $
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
#ifndef NANO_HTTP_COMMON_H 
#define NANO_HTTP_COMMON_H 

#include <stdlib.h>
#include <time.h>

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"
#define HEADER_CONTENT_ID "Content-Id"
#define HEADER_CONTENT_LOCATION "Content-Location"
#define HEADER_CONTENT_TRANSFER_ENCODING "Content-Transfer-Encoding"
#define HEADER_TRANSFER_ENCODING "Transfer-Encoding"
#define HEADER_CONNECTION "Connection"

#define HEADER_HOST "Host"
#define HEADER_DATE "Date"
#define HEADER_ACCEPT "Accept"


#define NHTTPD_ARG_PORT "-NHTTPport"
#define NHTTPD_ARG_TERMSIG "-NHTTPtsig"
#define NHTTPD_ARG_MAXCONN "-NHTTPmaxconn"
#define NHTTP_ARG_LOGFILE "-NHTTPlog"
#define NHTTP_ARG_TMPDIR "-NHTTPtmpdir"

#ifndef SAVE_STR
#define SAVE_STR(str) ((str==0)?("(null)"):(str))
#endif

#define TRANSFER_ENCODING_CHUNKED "chunked"

#define BOUNDARY_LENGTH 18

#define MAX_HEADER_SIZE 4256
#define MAX_SOCKET_BUFFER_SIZE 4256
#define MAX_FILE_BUFFER_SIZE 4256

#define URL_MAX_HOST_SIZE      120
#define URL_MAX_CONTEXT_SIZE  1024

#define HSOCKET_MAX_BUFSIZE 1024

#define REQUEST_MAX_PATH_SIZE 1024
#define RESPONSE_MAX_DESC_SIZE 1024

/*
  DIME common stuff
*/
#define DIME_VERSION_1      0x08
#define DIME_FIRST_PACKAGE  0x04
#define DIME_LAST_PACKAGE   0x02
#define DIME_CHUNKED        0x01
#define DIME_TYPE_URI       0x2



/* TODO (#1#): find proper ports */
#define URL_DEFAULT_PORT_HTTP 80
#define URL_DEFAULT_PORT_HTTPS 81
#define URL_DEFAULT_PORT_FTP 120 

/* Success flag */
#define H_OK 0

/* File errors */
#define FILE_ERROR_OPEN 8000
#define FILE_ERROR_READ 8001

/* Socket errors */
#define HSOCKET_ERROR_CREATE 1001
#define HSOCKET_ERROR_GET_HOSTNAME 1002
#define HSOCKET_ERROR_CONNECT 1003
#define HSOCKET_ERROR_SEND 1004
#define HSOCKET_ERROR_RECEIVE 1005
#define HSOCKET_ERROR_BIND 1006
#define HSOCKET_ERROR_LISTEN 1007
#define HSOCKET_ERROR_ACCEPT 1008
#define HSOCKET_ERROR_NOT_INITIALIZED 1009
#define HSOCKET_ERROR_IOCTL 1010

/* URL errors */
#define URL_ERROR_UNKNOWN_PROTOCOL 1101
#define URL_ERROR_NO_PROTOCOL 1102
#define URL_ERROR_NO_HOST 1103

/* Stream errors */
#define STREAM_ERROR_INVALID_TYPE 1201
#define STREAM_ERROR_SOCKET_ERROR 1202
#define STREAM_ERROR_NO_CHUNK_SIZE 1203
#define STREAM_ERROR_WRONG_CHUNK_SIZE 1204


/* MIME errors */
#define MIME_ERROR_NO_BOUNDARY_PARAM   1301
#define MIME_ERROR_NO_START_PARAM      1302
#define MIME_ERROR_PARSE_ERROR         1303
#define MIME_ERROR_NO_ROOT_PART        1304 
#define MIME_ERROR_NOT_MIME_MESSAGE    1305


/* General errors */
#define GENERAL_INVALID_PARAM	1400
#define GENERAL_HEADER_PARSE_ERROR	1401

/* Thread errors */
#define THREAD_BEGIN_ERROR 1500

/* XML Errors */
#define XML_ERROR_EMPTY_DOCUMENT 1600
#define XML_ERROR_PARSE 1601

/*
Set Sleep function platform depended
*/
#ifdef WIN32
  #define system_sleep(seconds) Sleep(seconds*1000);
#else
  #define system_sleep(seconds) sleep(seconds);
#endif

#ifdef WIN32
#include <string.h>
char* strtok_r(char *s, const char *delim, char **save_ptr);
struct tm *localtime_r(const time_t *const timep, struct tm *p_tm);
#endif

typedef unsigned char byte_t;
typedef void* herror_t;



/**
  Indicates the version of the 
  used HTTP protocol.
*/
typedef enum _http_version { 
  HTTP_1_0, 
  HTTP_1_1 /* default */
}http_version_t;


/**
  Indicates the used method
*/
typedef enum _hreq_method 
{
  HTTP_REQUEST_POST,
  HTTP_REQUEST_GET
}hreq_method_t ;


herror_t herror_new(const char* func, 
					int errcode, const char* format, ...);
int herror_code(herror_t err);
char* herror_func(herror_t err);
char* herror_message(herror_t err);
void herror_release(herror_t err);


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


/**
  Creates a new pair with the given parameters. Both strings
  key and value will be cloned while creating the pair. 

  @param key the key of the (key,value) pair
  @param value the value of the (key,value) pair
  @param next next pair node in the linked list

  @returns A newly crated hpair_t object. Use hpair_free() 
    or hpair_free_deep() to free the pair.
*/
hpair_t  *hpairnode_new(const char* key, const char* value, hpair_t* next);


/**
  Creates a new pair from a given string. This function 
  will split 'str' with the found first delimiter 'delim'.
  The 'value' field  of the newly created pair will have 
  the value "", if no delimiter was found/
  Whitespaces (' ') will be removed from the beginnig of 
  the parsed value.

  @param str A string to parse
  @param delim a delimiter to use while splitting into key,value 
  @param next next pair node in the linked list

  @returns A newly crated hpair_t object. Use hpair_free() 
    or hpair_free_deep() to free the pair.
*/
hpair_t *hpairnode_parse(const char *str, const char *delim, hpair_t * next);


/**
  Frees a given pair.

  @param pair the pair to free
*/
void hpairnode_free(hpair_t *pair);


/**
  Makes a deep free operation. All pairnodes,
  beginning with the given pari, in the 
  linked list will be destroyed. 

  @param pair the pair to start to free the linked list
*/
void hpairnode_free_deep(hpair_t * pair);


/**
  Returns the (key,value) pair, which key is the 
  given 'key'.

  @param pair the first pair to start to search from.
  @param key key to find the in the pair.
  @returns if a value will be found, this function will 
    return the value (do not free this string) or NULL
    if no pair was found with the key 'key'.
*/
char *hpairnode_get(hpair_t *pair, const char* key);


/**
  Returns the (key,value) pair, which key is the 
  given 'key'. The case will be ignored while 
  comparing the key strings.

  @param pair the first pair to start to search from.
  @param key key to find the in the pair.
  @returns if a value will be found, this function will 
    return the value (do not free this string) or NULL
    if no pair was found with the key 'key'.
*/
char *hpairnode_get_ignore_case(hpair_t *pair, const char* key);


/**
  This function will create a new pair and fills the 
  (key,value) fields of a given pair. Note that the 'next'
  field will not be copied.

  @param src the source pair object to copy.

  @returns a newly created pair with the same (key,value) 
   pairs as in 'src'. This fields will be cloned. The'next'
   field will be set to NULL.

  @see hpairnode_copy_deep
*/
hpair_t* hpairnode_copy(const hpair_t *src);


/**
  Clones the hole linked list. 

  @param src the source pair object to copy from

  @returns the first object in the linked list. 

  @see hpairnode_copy
*/
hpair_t* hpairnode_copy_deep(const hpair_t *src);

/* Debug functions */
void hpairnode_dump_deep(hpair_t *pair);
void hpairnode_dump(hpair_t *pair);

/**
  The protocol types in enumeration 
  format. Used in some other nanohttp objects
  like hurl_t.

  @see hurl_t
*/
typedef enum _hprotocol
{
  PROTOCOL_HTTP,
  PROTOCOL_HTTPS,
  PROTOCOL_FTP
}hprotocol_t;



/**
  The URL object. A representation 
  of an URL like:<P>
   
  [protocol]://[host]:[port]/[context]

*/
typedef struct _hurl
{
  /**
    The transfer protocol. 
    Note that only PROTOCOL_HTTP is supported by nanohttp.
  */
  hprotocol_t protocol;

  /**
    The port number. If no port number was given in the URL,
    one of the default port numbers will be selected. 
    URL_HTTP_DEFAULT_PORT    
    URL_HTTPS_DEFAULT_PORT   
    URL_FTP_DEFAULT_PORT    
  */ 
  int port;

  /** The hostname */
  char host[URL_MAX_HOST_SIZE];

  /** The string after the hostname. */
  char context[URL_MAX_CONTEXT_SIZE];
}hurl_t;


/**
  Parses the given 'urlstr' and fills the given hurl_t object.

  @param obj the destination URL object to fill
  @param url the URL in string format

  @returns H_OK on success or one of the following otherwise

    URL_ERROR_UNKNOWN_PROTOCOL 
    URL_ERROR_NO_PROTOCOL 
    URL_ERROR_NO_HOST 
*/
herror_t hurl_parse(hurl_t *obj, const char* url);

/*
  Object representation of the content-type field 
  in a HTTP header: 
  <P>
  Example:<P>
  
  text/xml; key="value" key2="value2' ...
*/
typedef struct _content_type
{
  char type[128];
  hpair_t *params;
}content_type_t;


/**
  Parses the given string and creates a new ccontent_type_t
  object. 

  @param content_type_str the string representation of the
  content-type field in a HTTP header.

  @returns A newly created content_type_t object. Free this
    object with content_type_free();

  @see content_type_free
*/
content_type_t *content_type_new(const char* content_type_str);


/**
  Frees the given content_type_t object
*/
void content_type_free(content_type_t *ct);



/*
  part. Attachment
*/
typedef struct _part
{
  char id[250];
  char location[250];
  hpair_t *header;
  char content_type[128];
  char transfer_encoding[128];
  char filename[250];
  struct _part *next;
  int deleteOnExit; /* default is 0 */
}part_t;


part_t *part_new(const char *id, const char* filename, 
  const char* content_type, const char* transfer_encoding, part_t *next);
void part_free(part_t *part);




/*
  Attachments
*/
typedef struct _attachments
{
  part_t *parts;
  part_t *last;
  part_t *root_part;
}attachments_t;

attachments_t *attachments_new(); /* should be used internally */

/*
  Free a attachment. Create attachments with MIME
  and DIME (DIME is not supported yet).

  @see mime_get_attachments
*/
void attachments_free(attachments_t *message);
void attachments_add_part(attachments_t *attachments, part_t *part);


/* tmp directory for multipart/related stuff */
#define HOPTION_TMP_DIR 2
void hoption_init_args(int argc, char* argv[]);
void hoption_set(int opt, const char* value);
char *hoption_get(int opt);


/* logging stuff */
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

void log_set_file(const char *filename);
char *log_get_file();

#ifdef WIN32
  #ifndef __FUNCTION__ 
	char *VisualC_funcname(const char* file, int line); /* not thread safe!*/
    #define __FUNCTION__  VisualC_funcname(__FILE__, __LINE__)
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






