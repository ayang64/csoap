/******************************************************************
 *  $Id: nanohttp-common.c,v 1.1 2003/12/11 14:51:04 snowdrop Exp $
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

  pair = (hpair_t*)malloc(sizeof(hpair_t));
  pair->key = (char*)malloc(strlen(key)+1);
  pair->value = (char*)malloc(strlen(value)+1);
  pair->next = next;

  strcpy(pair->key, key);
  strcpy(pair->value, value);

  return pair;
}

hpair_t *hpairnode_parse(const char *str, const char *delim, hpair_t *next)
{
  hpair_t *pair;
  char *key, *value;
  
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
    pair->value = (char*)malloc(strlen(value)+1);
    strcpy(pair->value, value);
  }

  return pair;
}

void hpairnode_free(hpair_t *pair)
{
  if (pair == NULL) return;
  
  free(pair->key);
  free(pair->value);
  
  free(pair);
}


static
void hurl_dump(const hurl_t *url)
{
  const char *FUNC = "hurl_dump";

  if (url == NULL) {
    printf("(null)\n");
    return ;
  }

  log_debug(FUNC, "PROTOCOL : %s", url->protocol?url->protocol:"(null)");
  log_debug(FUNC, "    HOST : %s", url->host?url->host:"(null)");
  log_debug(FUNC, "    PORT : %d", url->port);
  log_debug(FUNC, " CONTEXT : %s", url->context?url->context:"(null)");
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
  const char *FUNC = "hurl_create";
  
  iprotocol = 0;
  len = strlen(urlstr);
  
  /* find protocol */
  while (urlstr[iprotocol] != ':' && urlstr[iprotocol] != '\0')
  {
    iprotocol++;
  }

  if (iprotocol == 0) {
    log_error(FUNC, "no protocol");
    return NULL;
  }

  if (iprotocol + 3 >= len) {
    log_error(FUNC, "no host");
    return NULL;
  }

  if ( urlstr[iprotocol] != ':' 
    && urlstr[iprotocol+1] != '/' 
    && urlstr[iprotocol+2] != '/')
  {
    log_error(FUNC, "no protocol");
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
    log_error(FUNC, "no host");
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
  url->host = (char*)malloc(sizeof(char)*size);
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
hresponse_t *hresponse_new(const char* buffer)
{
  hresponse_t *res;
  char *s1, *s2, *str;
  hpair_t *pair;

  const char *FUNC = "hresponse_new";

  /* create response object */
  res = (hresponse_t*)malloc(sizeof(hresponse_t));
  res->desc = "";
  res->header = NULL;
  res->body = "";

  /* *** parse spec *** */
  /* [HTTP/1.1 | 1.2] [CODE] [DESC] */
  
  /* stage 1: HTTP spec */
  str = (char*)strtok_r(buffer, " ", &s2);
  s1 = s2;
  if (str == NULL) { log_error(FUNC, "Parse error"); return NULL; }
  
  strncpy(res->spec, str, 10);

  /* stage 2: http code */
  str = (char*)strtok_r(s1, " ", &s2);
  s1 = s2;
  if (str == NULL) { log_error(FUNC, "Parse error"); return NULL; }
  
  res->errcode = atoi(str);

  /* stage 3: description text */
  str = (char*)strtok_r(s1, "\r\n", &s2);
  s1 = s2;
  if (str == NULL) { log_error(FUNC, "Parse error"); return NULL; }

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
    res->header = hpairnode_parse(str, ": ", res->header);
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


