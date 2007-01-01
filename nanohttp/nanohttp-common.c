/******************************************************************
*  $Id: nanohttp-common.c,v 1.38 2007/01/01 22:54:46 m0gg Exp $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include "nanohttp-logging.h"
#include "nanohttp-error.h"
#include "nanohttp-common.h"

hpair_t *
hpairnode_new(const char *key, const char *value, hpair_t * next)
{
  hpair_t *pair;

  log_verbose3("new pair ('%s','%s')", SAVE_STR(key), SAVE_STR(value));
  if (!(pair = (hpair_t *) malloc(sizeof(hpair_t))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  if (key != NULL)
  {
    pair->key = strdup(key);
  }
  else
  {
    pair->key = NULL;
  }

  if (value != NULL)
  {
    pair->value = strdup(value);
  }
  else
  {
    pair->value = NULL;
  }

  pair->next = next;

  return pair;
}

hpair_t *
hpairnode_parse(const char *str, const char *delim, hpair_t * next)
{
  hpair_t *pair;
  char *key, *value;

  pair = (hpair_t *) malloc(sizeof(hpair_t));
  pair->key = "";
  pair->value = "";
  pair->next = next;

  key = strtok_r((char *) str, delim, &value);

  if (key != NULL)
  {
    pair->key = strdup(key);
  }
  if (value != NULL)
  {
    /* skip white space */
    for (; *value == ' '; value++) ;

    pair->value = strdup(value);
  }
  return pair;
}

hpair_t *
hpairnode_copy(const hpair_t * src)
{
  hpair_t *pair;

  if (src == NULL)
    return NULL;

  pair = hpairnode_new(src->key, src->value, NULL);
  return pair;
}

hpair_t *
hpairnode_copy_deep(const hpair_t * src)
{
  hpair_t *pair, *result, *next;

  if (src == NULL)
    return NULL;

  result = hpairnode_copy(src);

  next = src->next;
  pair = result;

  while (next != NULL)
  {
    pair->next = hpairnode_copy(next);
    pair = pair->next;
    next = next->next;
  }

  return result;
}

void
hpairnode_dump(const hpair_t * pair)
{
  if (pair == NULL)
  {
    log_verbose1("(NULL)[]");
    return;
  }
  log_verbose5("(%p)['%s','%s','%p']", pair,
               SAVE_STR(pair->key), SAVE_STR(pair->value), pair->next);

  return;
}

void
hpairnode_dump_deep(const hpair_t * pairs)
{
  const hpair_t *p;

  log_verbose1("-- BEGIN dump_deep hpair_t --");
  for (p = pairs; p != NULL; p = p->next)
  {
    hpairnode_dump(p);
  }
  log_verbose1("-- END dump_deep hpair_t --\n");

  return;
}

void
hpairnode_free(hpair_t * pair)
{
  if (pair == NULL)
    return;

  if (pair->key)
    free(pair->key);

  if (pair->value)
    free(pair->value);

  free(pair);

  return;
}


void
hpairnode_free_deep(hpair_t * pair)
{
  hpair_t *tmp;

  while (pair != NULL)
  {
    tmp = pair->next;
    hpairnode_free(pair);
    pair = tmp;
  }

  return;
}

char *
hpairnode_get_ignore_case(hpair_t * pair, const char *key)
{
  if (key == NULL)
  {
    log_error1("key is NULL");
    return NULL;
  }

  while (pair != NULL)
  {
    if (pair->key != NULL)
    {
      if (!strcasecmp(pair->key, key))
      {
        return pair->value;
      }
    }
    pair = pair->next;
  }

  return NULL;
}

char *
hpairnode_get(hpair_t * pair, const char *key)
{
  if (key == NULL)
  {
    log_error1("key is NULL");
    return NULL;
  }
  while (pair != NULL)
  {
    if (pair->key != NULL)
    {
      if (!strcmp(pair->key, key))
      {
        return pair->value;
      }
    }
    pair = pair->next;
  }

  return NULL;
}

/* Content-type stuff */

content_type_t *
content_type_new(const char *content_type_str)
{
  hpair_t *pair = NULL, *last = NULL;
  content_type_t *ct;
  char ch, key[256], value[256];
  int inQuote = 0, i = 0, c = 0, begin = 0, len;
  int mode = 0;
  /* 0: searching ';' 1: process key 2: process value */


  /* Create object */
  ct = (content_type_t *) malloc(sizeof(content_type_t));
  ct->params = NULL;

  len = strlen(content_type_str);
  while (i <= len)
  {
    if (i != len)
      ch = content_type_str[i++];
    else
    {
      ch = ' ';
      i++;
    }

    switch (mode)
    {
    case 0:

      if (ch == ';')
      {
        ct->type[c] = '\0';
        c = 0;
        mode = 1;
      }
      else if (ch != ' ' && ch != '\t' && ch != '\r')
        ct->type[c++] = ch;
      break;

    case 1:

      if (ch == '=')
      {
        key[c] = '\0';
        c = 0;
        mode = 2;
      }
      else if (ch != ' ' && ch != '\t' && ch != '\r')
        key[c++] = ch;
      break;

    case 2:

      if (ch != ' ')
        begin = 1;

      if ((ch == ' ' || ch == ';') && !inQuote && begin)
      {
        value[c] = '\0';

        pair = hpairnode_new(key, value, NULL);
        if (ct->params == NULL)
          ct->params = pair;
        else
          last->next = pair;
        last = pair;

        c = 0;
        begin = 0;
        mode = 1;
      }
      else if (ch == '"')
        inQuote = !inQuote;
      else if (begin && ch != '\r')
        value[c++] = ch;

      break;

    }
  }

  return ct;
}

void
content_type_free(content_type_t * ct)
{
  if (!ct)
    return;

  hpairnode_free_deep(ct->params);
  free(ct);

  return;
}

struct part_t *
part_new(const char *id, const char *filename, const char *content_type, const char *transfer_encoding, struct part_t * next)
{
  struct part_t *part;
 
  if (!(part = (struct part_t *) malloc(sizeof(struct part_t))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  part->header = NULL;
  part->next = next;
  part->deleteOnExit = 0;
  strcpy(part->id, id);
  strcpy(part->filename, filename);
  if (content_type)
    strcpy(part->content_type, content_type);
  else
    part->content_type[0] = '\0';

  part->header = hpairnode_new(HEADER_CONTENT_ID, id, part->header);
  /* TODO (#1#): encoding is always binary. implement also others! */
/*  part->header = hpairnode_new(HEADER_CONTENT_TRANSFER_ENCODING, "binary", part->header);*/

  strcpy(part->transfer_encoding,
         transfer_encoding ? transfer_encoding : "binary");

  if (content_type)
  {
    part->header =
      hpairnode_new(HEADER_CONTENT_TYPE, content_type, part->header);
  }
  else
  {
    /* TODO (#1#): get content-type from mime type list */
  }

  return part;
}

void
part_free(struct part_t * part)
{
  if (part == NULL)
    return;

  if (part->deleteOnExit)
  {
    remove(part->filename);
  }

  hpairnode_free_deep(part->header);

  free(part);
}

struct attachments_t *
attachments_new(void)               /* should be used internally */
{
  struct attachments_t *attachments;
 
  if (!(attachments = (struct attachments_t *) malloc(sizeof(struct attachments_t))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  attachments->parts = NULL;
  attachments->last = NULL;
  attachments->root_part = NULL;

  return attachments;
}

void
attachments_add_part(struct attachments_t *attachments, struct part_t *part)
{
  if (!attachments)
    return;

  if (attachments->last)
    attachments->last->next = part;
  else
    attachments->parts = part;

  attachments->last = part;

  return;
}

/*
  Free a mime message 
*/
void
attachments_free(struct attachments_t *message)
{
  struct part_t *tmp, *part;

  if (!message)
    return;

  part = message->parts;
  while (part)
  {
    tmp = part->next;
    part_free(part);
    part = tmp;
  }

  if (message->root_part)
    part_free(message->root_part);

  free(message);

  return;
}


#ifdef WIN32

/* strtok_r() */
char *
strtok_r(char *s, const char *delim, char **save_ptr)
{
  char *token;

  if (s == NULL)
    s = *save_ptr;

  /* Scan leading delimiters.  */
  s += strspn(s, delim);
  if (*s == '\0')
    return NULL;

  /* Find the end of the token.  */
  token = s;
  s = strpbrk(token, delim);
  if (s == NULL)
    /* This token finishes the string.  */
    *save_ptr = strchr(token, '\0');
  else
  {
    /* Terminate the token and make *SAVE_PTR point past it.  */
    *s = '\0';
    *save_ptr = s + 1;
  }
  return token;
}

/* localtime_r() */
struct tm *
localtime_r(const time_t * const timep, struct tm *p_tm)
{
  static struct tm *tmp;
  tmp = localtime(timep);
  if (tmp)
  {
    memcpy(p_tm, tmp, sizeof(struct tm));
    tmp = p_tm;
  }
  return tmp;
}

#endif
