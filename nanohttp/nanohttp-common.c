/******************************************************************
 *  $Id: nanohttp-common.c,v 1.4 2003/12/18 11:14:37 snowdrop Exp $
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

#include <nanohttp/nanohttp-common.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


static log_level_t loglevel = HLOG_DEBUG;

log_level_t log_set_level(log_level_t level)
{
  log_level_t old = loglevel;
  loglevel = level;
  return old;
}


log_level_t log_get_level()
{
  return loglevel;
}


static
void log_write(log_level_t level, const char *prefix,
	       const char* func, const char *format, va_list ap)
{
  char buffer[1054];
  char buffer2[1054];
  
  if (level < loglevel) return;

  sprintf(buffer, "*%s*: [%s] %s\n", prefix,  func, format);
  vsprintf(buffer2, buffer, ap);
  printf(buffer2);
  fflush(stdout); 
}

void log_verbose(const char* FUNC, const char *format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  log_write(HLOG_VERBOSE, "VERBOSE", FUNC, format, ap);
  va_end(ap);
}

void log_debug(const char* FUNC, const char *format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  log_write(HLOG_DEBUG, "DEBUG", FUNC, format, ap);
  va_end(ap);
}

void log_info(const char* FUNC, const char *format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  log_write(HLOG_INFO, "INFO", FUNC, format, ap);
  va_end(ap);
}

void log_warn(const char* FUNC, const char *format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  log_write(HLOG_WARN, "WARN", FUNC, format, ap);
  va_end(ap);
}

void log_error(const char* FUNC, const char *format, ...)
{
  va_list ap;
  
  va_start(ap, format);
  log_write(HLOG_ERROR, "ERROR", FUNC, format, ap);
  va_end(ap);
}


hpair_t *hpairnode_new(const char* key, const char* value, hpair_t *next)
{
  hpair_t *pair;

  log_verbose3("new pair ('%s','%s')", SAVE_STR(key), SAVE_STR(value));
  pair = (hpair_t*)malloc(sizeof(hpair_t));

  if (key != NULL) {
    pair->key = (char*)malloc(strlen(key)+1);
    strcpy(pair->key, key);
  } else {
    pair->key = NULL;
  }

  if (value != NULL) {
    pair->value = (char*)malloc(strlen(value)+1);
    strcpy(pair->value, value);
  } else {
    pair->value = NULL;
  }

  pair->next = next;

  return pair;
}

hpair_t *hpairnode_parse(const char *str, const char *delim, hpair_t *next)
{
  hpair_t *pair;
  char *key, *value;
  int c;

  pair = (hpair_t*)malloc(sizeof(hpair_t));
  pair->key = "";
  pair->value = "";
  pair->next = next;

  key = strtok_r(str, delim, &value);
  
  if (key != NULL) {   
    pair->key = (char*)malloc(strlen(key)+1);
    strcpy(pair->key, key);
  }

  if (value != NULL) {
    for (c=0;value[c]==' ';c++); /* skip white space */
    pair->value = (char*)malloc(strlen(&value[c])+1);
    strcpy(pair->value, &value[c]);
  }

  return pair;
}


hpair_t* hpairnode_copy(const hpair_t *src)
{
  hpair_t *pair;

  if (src == NULL) return NULL;

  pair = hpairnode_new(src->key, src->value, NULL);
  return pair;
}


hpair_t* hpairnode_copy_deep(const hpair_t *src)
{
  hpair_t *pair, *result, *next;

  if (src == NULL) return NULL;

  result = hpairnode_copy(src);

  next = src->next;
  pair = result;

  while (next != NULL) {
    pair->next = hpairnode_copy(next); 
    pair = pair->next;
    next = next->next;
  }
  
  return result;
}


void hpairnode_dump(hpair_t *pair)
{
  if (pair == NULL) {
    log_verbose1("(NULL)[]");
    return;
  }

  log_verbose5("(%p)['%s','%s','%p']", pair, 
	  SAVE_STR(pair->key), SAVE_STR(pair->value), 
	  pair->next);
}


void hpairnode_dump_deep(hpair_t *pair)
{
  hpair_t *p;
  p = pair;

  log_verbose1("-- BEGIN dump hpairnode_t --");

  while (p != NULL) {
    hpairnode_dump(p);
    p = p->next;
  }

  log_verbose1("-- END dump hpairnode_t --\n");
}


void hpairnode_free(hpair_t *pair)
{
  if (pair == NULL) return;
  
  free(pair->key);
  free(pair->value);
  
  free(pair);
}


char *hpairnode_get(hpair_t *pair, const char* key)
{
  if (key == NULL) {
    log_error1("key is NULL");
    return NULL;
  }

  while (pair != NULL) {
    if (pair->key != NULL) {
      if (!strcmp(pair->key, key)) {
	return pair->value;
      }
    }
    pair = pair->next;
  }

  return NULL;
}

static
void hurl_dump(const hurl_t *url)
{

  if (url == NULL) {
    log_error1("url is NULL!");
    return ;
  }

  log_verbose2("PROTOCOL : %s", SAVE_STR(url->protocol));
  log_verbose2("    HOST : %s", SAVE_STR(url->host));
  log_verbose2("    PORT : %d", url->port);
  log_verbose2(" CONTEXT : %s", SAVE_STR(url->context));
}


hurl_t* hurl_new(const char* urlstr)
{
  int iprotocol;
  int ihost;
  int iport;
  int len;
  int size;
  hurl_t *url;
  char tmp[8];
  
  iprotocol = 0;
  len = strlen(urlstr);
  
  /* find protocol */
  while (urlstr[iprotocol] != ':' && urlstr[iprotocol] != '\0')
  {
    iprotocol++;
  }

  if (iprotocol == 0) {
    log_error1("no protocol");
    return NULL;
  }

  if (iprotocol + 3 >= len) {
    log_error1("no host");
    return NULL;
  }

  if ( urlstr[iprotocol] != ':' 
    && urlstr[iprotocol+1] != '/' 
    && urlstr[iprotocol+2] != '/')
  {
    log_error1("no protocol");
    return NULL;
  }

  /* find host */
  ihost = iprotocol + 3;
  while (urlstr[ihost] != ':'
    && urlstr[ihost] != '/'
    && urlstr[ihost] != '\0')
  {
    ihost++;
  }

  if (ihost == iprotocol + 1) {
    log_error1("no host");
    return NULL;
  }

  /* find port */
  iport = ihost;
  if (ihost + 1 < len) {
    if (urlstr[ihost] == ':') {
      while (urlstr[iport] != '/' && urlstr[iport] != '\0')  {
       iport++;
      }
    }
  }

  url = (hurl_t*)malloc(sizeof(hurl_t));

  url->protocol = (char*)malloc(sizeof(char)*iprotocol+1);
  strncpy(url->protocol, urlstr, iprotocol);
  url->protocol[iprotocol] = '\0';

  size = ihost - iprotocol - 3;
  url->host = (char*)malloc(sizeof(char)*size + 1);
  strncpy(url->host, &urlstr[iprotocol+3], size);
  url->host[size] = '\0';

  if (iport > ihost)
  {
    size = iport - ihost;
    strncpy(tmp, &urlstr[ihost+1], size);
    url->port = atoi(tmp);
  } else {
    url->port = 80;
  }

  len = strlen(urlstr);
  if (len > iport )
  {
    size = len - iport;
    url->context = (char*)malloc(sizeof(char)*size+1);
    strncpy(url->context, &urlstr[iport], size);
    url->context[size]='\0';
  } else {
    url->context = NULL;
  }

  hurl_dump(url);

  return url;
}


void hurl_free(hurl_t *url)
{
  if (url == NULL) return;

  free(url->protocol);
  free(url->host);
  free(url->context);

  free(url);
}


/* response stuff */


/* -------------------------------------------
   FUNCTION: hresponse_new
 ---------------------------------------------*/
hresponse_t *hresponse_new()
{
  hresponse_t *res;

  /* create response object */
  res = (hresponse_t*)malloc(sizeof(hresponse_t));
  res->spec[0] = '\0';
  res->errcode = -1;
  res->desc = NULL;
  res->header = NULL;
  res->body = NULL;
  res->bodysize = 0;

  return res;
}


/* -------------------------------------------
   FUNCTION: hresponse_new_from_buffer
 ---------------------------------------------*/
hresponse_t *hresponse_new_from_buffer(const char* buffer)
{
  hresponse_t *res;
  char *s1, *s2, *str;
  hpair_t *pair;

  /* create response object */
  res = hresponse_new();

  /* *** parse spec *** */
  /* [HTTP/1.1 | 1.2] [CODE] [DESC] */
  
  /* stage 1: HTTP spec */
  str = (char*)strtok_r(buffer, " ", &s2);
  s1 = s2;
  if (str == NULL) { log_error1("Parse error"); return NULL; }
  
  strncpy(res->spec, str, 10);

  /* stage 2: http code */
  str = (char*)strtok_r(s1, " ", &s2);
  s1 = s2;
  if (str == NULL) { log_error1("Parse error"); return NULL; }
  
  res->errcode = atoi(str);

  /* stage 3: description text */
  str = (char*)strtok_r(s1, "\r\n", &s2);
  s1 = s2;
  if (str == NULL) { log_error1("Parse error"); return NULL; }

  res->desc = (char*)malloc(strlen(str)+1);
  strcpy(res->desc, str);
  res->desc[strlen(str)] = '\0';

  /* *** parse header *** */
  /* [key]: [value] */
  for (;;) {
    str = strtok_r(s1, "\n", &s2);
    s1 = s2;

    /* check if header ends without body */
    if (str == NULL) {
      return res;
    }

    /* check also for end of header */
    if (!strcmp(str, "\r")) {
      break;
    }

    str[strlen(str)-1] = '\0';
    res->header = hpairnode_parse(str, ":", res->header);
  }

  /* *** Save body *** */
  res->body = s1;

  /* return response object */
  return res;
}


void hresponse_free(hresponse_t *res)
{
  /* not implemented yet!*/
}








