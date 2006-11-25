/******************************************************************
*  $Id: soap-xmlsec.c,v 1.3 2006/11/25 17:03:20 m0gg Exp $
*
* CSOAP Project:  A SOAP client/server library in C
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

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include <xmlsec/errors.h>

#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-logging.h>

#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-server.h"
#include "soap-addressing.h"
#include "soap-xmlsec.h"

static pthread_mutex_t _soap_xmlsec_lock;
static xmlSecKeyPtr _soap_xmlsec_sign_key;
static int _soap_xmlsec_enabled;

static void _soap_xmlsec_error_callback(const char *file, int line, const char *func, const char *errorObject, const char *errorSubject, int reason, const char *msg)
{
  log_error5("xmlsec error func=\"%s\" obj=\"%s\" sub=\"%s\" %s", func, errorObject, errorSubject, msg);

  return;
}

static herror_t
_soap_xmlsec_load_key(void)
{
  char *key = "key.pem";
  char *cert = "cert.pem";

  if ((_soap_xmlsec_sign_key = xmlSecCryptoAppKeyLoad(key, xmlSecKeyDataFormatPem, "password", NULL, NULL)) == NULL)
  {
    log_error2("xmlSecCryptoAppKeyLoad(\"%s\") failed", key);
    return herror_new("_soap_xmlsec_load_key", 0, "xmlSecCryptoAppKeyLoad(\"%s\") failed", key);
  }

  if (xmlSecCryptoAppKeyCertLoad(_soap_xmlsec_sign_key, cert, xmlSecKeyDataFormatPem) < 0)
  {
    log_error2("xmlSecCryptoAppKeyCertLoad(\"%s\") failed", cert);
    return herror_new("_soap_xmlsec_load_key", 0, "xmlSecCryptoAppKeyCertLoad(\"%s\") failed", cert);
  }

  if (xmlSecKeySetName(_soap_xmlsec_sign_key, soap_server_get_name()) < 0)
  {
    log_error1("xmlSecKeySetName failed");
    return herror_new("_soap_xmlsec_load_key", 0, "xmlSecKeySetName failed");
  }

  return H_OK;
}

herror_t
soap_xmlsec_init_args(int argc, char **argv)
{
  int err, i;
  herror_t status;

  _soap_xmlsec_enabled = 0;
  for (i=0; i<argc; i++)
    if (!strcmp(argv[i], CSOAP_ENABLE_XMLSEC))
      _soap_xmlsec_enabled = 1;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  log_info1("initializing xmlsec1");

  xmlSecErrorsDefaultCallbackEnableOutput(0);
  xmlSecErrorsSetCallback(_soap_xmlsec_error_callback);

  if (xmlSecInit() < 0)
  {
    log_error1("xmlSecInit failed");
    return herror_new("soap_xmlsec_init_args", 0, "xmlSecInit failed");
  }

  if (xmlSecCheckVersion() != 1)
  {
    log_error1("xmlSecCheckVersion failed, wrong xmlsec version");
    return herror_new("soap_xmlsec_init_args", 0, "Wrong xmlsec version");
  }

#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
  log_verbose2("loading \"%s\" dynamic", XMLSEC_CRYPTO);
  if (xmlSecCryptoDLLoadLibrary(BAD_CAST XMLSEC_CRYPTO) < 0)
  {
    log_error1("xmlSecCryptoDLLoadLibrary failed");
    return herror_new("soap_xmlsec_init_args", 0, "xmlSecCryptoDLLoadLibrary failed");
  }
#endif

  if (xmlSecCryptoAppInit(NULL) < 0)
  {
    log_error1("xmlSecCryptoAppInit failed");
    return herror_new("soap_xmlsec_init_args", 0, "xmlSecCryptoAppInit failed");
  }

  if (xmlSecCryptoInit() < 0)
  {
    log_error1("xmlSecCryptoInit failed");
    return herror_new("soap_xmlsec_init_args", 0, "xmlSecCryptoInit failed");
  }

  if ((err = pthread_mutex_init(&_soap_xmlsec_lock, NULL)) < 0)
  {
    log_error1("pthread_mutex_init failed");
    return herror_new("soap_xmlsec_init_args", 0, "pthread_mutex_init failed %s", strerror(err));
  }

  if ((status = _soap_xmlsec_load_key()) != H_OK)
  {
    log_error1("_soap_xmlsec_create_sign_context failed");
    return status;
  }

  return H_OK;
}

herror_t soap_xmlsec_sign(struct SoapEnv *envelope)
{
  xmlNodePtr signNode;
  xmlNodePtr refNode;
  xmlNodePtr keyInfoNode;
  xmlSecDSigCtxPtr dsigCtx;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  pthread_mutex_lock(&_soap_xmlsec_lock);

  if (!(signNode = xmlSecTmplSignatureCreate(envelope->root->doc, xmlSecTransformExclC14NId, xmlSecTransformRsaSha1Id, NULL)))
  {
    log_error1("xmlSecTmplSignatureCreate failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureCreate failed");
  }

  if (!(refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id, NULL, NULL, NULL)))
  {
    log_error1("xmlSecTmplSignatureAddReference failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureAddReference failed");
  }

  if (!(keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL)))
  {
    log_error1("xmlSecTmplSignatureEnsureKeyInfo failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureEnsureKeyInfo failed");
  }

  if (xmlSecTmplKeyInfoAddKeyName(keyInfoNode, NULL) == NULL)
  {
    log_error1("xmlSecTmplKeyInfoAddKeyName failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecTmplKeyInfoAddKeyName failed");
  }

  if (!(dsigCtx = xmlSecDSigCtxCreate(NULL)))
  {
    log_error1("xmlSecDSigCtxCreate failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecDSigCtxCreate failed");
  }

  dsigCtx->signKey = _soap_xmlsec_sign_key;
  
  if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0)
  {
    log_error1("xmlSecDSigCtxSign failed");
    return herror_new("soap_xmlsec_sign", 0, "xmlSecDSigCtxSign failed");
  }
  xmlAddChild(envelope->header, signNode);

  pthread_mutex_unlock(&_soap_xmlsec_lock);

  return H_OK;
}

herror_t soap_xmlsec_encrypt(struct SoapEnv *envelope)
{
  if (!_soap_xmlsec_enabled)
    return H_OK;

  return H_OK;
}

void soap_xmlsec_destroy(void)
{
  xmlSecCryptoShutdown();

  xmlSecCryptoAppShutdown();

  xmlSecShutdown();

  return;
}
