/******************************************************************
 *  $Id: soap-ctx.c,v 1.14 2006/11/25 15:06:57 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
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
 * Email: ferhatayaz@jprogrammer.net
 ******************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <libxml/tree.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-fault.h"
#include "soap-env.h"
#include "soap-ctx.h"

struct SoapCtx *
soap_ctx_new(struct SoapEnv * env)     /* should only be used internally */
{
  struct SoapCtx *ctx;
 
  if (!(ctx = (struct SoapCtx *) malloc(sizeof(struct SoapCtx))))
  {
    log_error2("malloc failed (%s)", strerror(errno));
    return NULL;
  }

  ctx->env = env;
  ctx->attachments = NULL;

  return ctx;
}

void
soap_ctx_add_files(struct SoapCtx *ctx, struct attachments_t *attachments)
{
  struct part_t *part;
  char href[MAX_HREF_SIZE];

  if (attachments == NULL)
    return;

  part = attachments->parts;
  while (part)
  {
    soap_ctx_add_file(ctx, part->filename, part->content_type, href);
    part = part->next;
  }

  return;
}

herror_t
soap_ctx_add_file(struct SoapCtx * ctx, const char *filename,
                  const char *content_type, char *dest_href)
{
  char cid[250];
  char id[250];
  struct part_t *part;
  static int counter = 1;
  FILE *test = fopen(filename, "r");
  if (!test)
    return herror_new("soap_ctx_add_file", FILE_ERROR_OPEN,
                      "Can not open file '%s'", filename);

  fclose(test);

  /* generate an id */
  sprintf(id, "005512345894583%d", counter++);
  sprintf(dest_href, "cid:%s", id);
  sprintf(cid, "<%s>", id);

  /* add part to context */
  part = part_new(cid, filename, content_type, NULL, NULL);
  if (!ctx->attachments)
    ctx->attachments = attachments_new();
  attachments_add_part(ctx->attachments, part);

  return H_OK;
}

struct part_t *
soap_ctx_get_file(struct SoapCtx * ctx, xmlNodePtr node)
{
  xmlChar *prop;
  char href[MAX_HREF_SIZE];
  char buffer[MAX_HREF_SIZE];
  struct part_t *part;

  if (!ctx->attachments)
    return NULL;

  prop = xmlGetProp(node, "href");

  if (!prop)
    return NULL;

  strcpy(href, (const char *) prop);
  if (!strncmp(href, "cid:", 4))
  {
    for (part = ctx->attachments->parts; part; part = part->next)
    {
      sprintf(buffer, "<%s>", href + 4);
      if (!strcmp(part->id, buffer))
        return part;

    }
  }
  else
  {
    for (part = ctx->attachments->parts; part; part = part->next)
    {
      if (!strcmp(part->location, href))
        return part;

    }
  }

  return NULL;
}

void
soap_ctx_free(struct SoapCtx * ctx)
{
  if (!ctx)
    return;

  if (ctx->attachments)
    attachments_free(ctx->attachments);

  if (ctx->env)
    soap_env_free(ctx->env);

  free(ctx);

  return;
}

herror_t
soap_ctx_new_with_method(const char *urn, const char *method, struct SoapCtx ** out)
{
  struct SoapEnv *env;
  herror_t err;

  if ((err = soap_env_new_with_method(urn, method, &env)) != H_OK)
    return err;

  *out = soap_ctx_new(env);

  return H_OK;
}
