/******************************************************************
 *  $Id: soap-ctx.h,v 1.1 2004/10/15 13:33:48 snowdrop Exp $
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
#ifndef cSOAP_CTX_H
#define cSOAP_CTX_H


#include <libcsoap/soap-env.h>
#include <nanohttp/nanohttp-common.h>

#define MAX_HREF_SIZE 150

typedef struct _SoapCtx
{
  SoapEnv *env;
  attachments_t *attachments;
}SoapCtx;


SoapCtx* soap_ctx_new(SoapEnv *env); /* should only be used internally */

/* Size of destination dest_href should be MAX_HREF_SIZE */
hstatus_t soap_ctx_add_file(SoapCtx* ctx, const char* filename, const char* content_type, char *dest_href);
void soap_ctx_add_files(SoapCtx* ctx, attachments_t *attachments);
void soap_ctx_free(SoapCtx* ctx);

#endif

