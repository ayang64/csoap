/******************************************************************
*  $Id: soap-xmlsec.c,v 1.4 2006/11/28 23:45:57 m0gg Exp $
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#endif

#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmlenc.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include <xmlsec/errors.h>

#include <nanohttp/nanohttp-logging.h>
#include <nanohttp/nanohttp-error.h>
#include <nanohttp/nanohttp-common.h>
#include <nanohttp/nanohttp-stream.h>
#include <nanohttp/nanohttp-request.h>
#include <nanohttp/nanohttp-response.h>
#include <nanohttp/nanohttp-client.h>
#include <nanohttp/nanohttp-server.h>

#include "soap-env.h"
#include "soap-ctx.h"
#include "soap-service.h"
#include "soap-router.h"
#include "soap-server.h"
#include "soap-transport.h"
#include "soap-addressing.h"
#include "soap-xmlsec.h"

static int _soap_xmlsec_enabled = 0;
static pthread_mutex_t _soap_xmlsec_lock;

static char *_soap_xmlsec_keyfile = NULL;
static char *_soap_xmlsec_password = NULL;
static char *_soap_xmlsec_certfile = NULL;

static xmlSecKeysMngrPtr _soap_xmlsec_key_manager = NULL;
static xmlSecKeyPtr _soap_xmlsec_key = NULL;

static void _soap_xmlsec_error_callback(const char *file, int line, const char *func, const char *errorObject, const char *errorSubject, int reason, const char *msg)
{
  log_error5("xmlsec error func=\"%s\" obj=\"%s\" sub=\"%s\" %s", func, errorObject, errorSubject, msg);

  return;
}

/**
 *
 * decrypt_file:
 *
 * @mngr:               the pointer to keys manager.
 * @enc_file:           the encrypted XML  file name.
 *
 * Decrypts the XML file #enc_file using DES key from #key_file and
 * prints results to stdout.
 *
 * @Returns 0 on success or a negative value if an error occurs.
 *
 */
static int _soap_xmlsec_decrypt_file(xmlSecKeysMngrPtr mngr, const char *enc_file)
{
  xmlDocPtr  doc = NULL;
  xmlNodePtr  node = NULL;
  xmlSecEncCtxPtr  encCtx = NULL;
  int    res = -1;

  /* load template */
  doc = xmlParseFile(enc_file);
  if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)) {
    fprintf(stderr, "Error: unable to parse file \"%s\"\n", enc_file);
    goto done;
  }

  /* find start node */
  node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeEncryptedData, xmlSecEncNs);
  if (node == NULL) {
    fprintf(stderr, "Error: start node not found in \"%s\"\n", enc_file);
    goto done;
  }

  /* create encryption context */
  encCtx = xmlSecEncCtxCreate(mngr);
  if (encCtx == NULL) {
    fprintf(stderr, "Error: failed to create encryption context\n");
    goto done;
  }

  /* decrypt the data */
  if ((xmlSecEncCtxDecrypt(encCtx, node) < 0) || (encCtx->result == NULL)) {
    fprintf(stderr, "Error: decryption failed\n");
    goto done;
  }

  /* print decrypted data to stdout */
  if (encCtx->resultReplaced != 0) {

    fprintf(stdout, "*** after decryption ***\n");
    xmlDocFormatDump(stdout, doc, 1);
    fprintf(stdout, "************************\n");

  }
  else {
    fprintf(stdout, "Decrypted binary data (%d bytes):\n", xmlSecBufferGetSize(encCtx->result));
    if (xmlSecBufferGetData(encCtx->result) != NULL) {
      fwrite(xmlSecBufferGetData(encCtx->result),
             1,
             xmlSecBufferGetSize(encCtx->result),
             stdout);
    }
  }
  fprintf(stdout, "\n");

  /* success */
  res = 0;

done:
  /* cleanup */
  if (encCtx != NULL) {
    xmlSecEncCtxDestroy(encCtx);
  }

  if (doc != NULL) {
    xmlFreeDoc(doc);
  }
  return (res);
}

static int _soap_xmlsec_encrypt_file(xmlSecKeysMngrPtr mngr, const char *enc_file, const char *key_name)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr encDataNode = NULL;
  xmlNodePtr keyInfoNode = NULL;
  xmlNodePtr encKeyNode = NULL;
  xmlNodePtr keyInfoNode2 = NULL;
  xmlSecEncCtxPtr encCtx = NULL;

  doc = xmlParseFile(enc_file);
  if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL))
  {
    log_error2("xmlParseFile failed (%s)", enc_file);
    return -1;
  }

  fprintf(stdout, "*** before encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "*************************\n");

  encDataNode = xmlSecTmplEncDataCreate(doc, xmlSecTransformDes3CbcId, NULL, xmlSecTypeEncElement, NULL, NULL);
  if (encDataNode == NULL)
  {
    log_error1("xmlSecTmplEnvDataCreate failed");
    return -1;
  }

  if (xmlSecTmplEncDataEnsureCipherValue(encDataNode) == NULL)
  {
    log_error1("failed to add CipherValue node");
    return -1;
  }

  keyInfoNode = xmlSecTmplEncDataEnsureKeyInfo(encDataNode, NULL);
  if (keyInfoNode == NULL)
  {
    log_error1("failed to add KeyInfo node");
    return -1;
  }

  encKeyNode = xmlSecTmplKeyInfoAddEncryptedKey(keyInfoNode, xmlSecTransformRsaOaepId, NULL, NULL, NULL);
  if (encKeyNode == NULL)
  {
    log_error1("failed to add KeyInfo");
    return -1;
  }

  if (xmlSecTmplEncDataEnsureCipherValue(encKeyNode) == NULL)
  {
    log_error1("failed to add CipherValue node");
    return -1;
  }

  keyInfoNode2 = xmlSecTmplEncDataEnsureKeyInfo(encKeyNode, NULL);
  if (keyInfoNode2 == NULL)
  {
    log_error1("failed to add key info (2)\n");
    return -1;
  }

  // log_error2("adding key \"%s\"", key_name);
  if (xmlSecTmplKeyInfoAddKeyName(keyInfoNode2, key_name) == NULL)
  {

    log_error1("failed to add key name");
    return -1;
  }

  encCtx = xmlSecEncCtxCreate(mngr);
  if (encCtx == NULL)
  {
    log_error1("failed to create encryption context");
    return -1;
  }

  encCtx->encKey = xmlSecKeyGenerate(xmlSecKeyDataDesId, 192, xmlSecKeyDataTypeSession);
  if (encCtx->encKey == NULL)
  {
    log_error1("failed to generate session key");
    return -1;
  }

  fprintf(stdout, "*** before encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "*************************\n");

  if (xmlSecEncCtxXmlEncrypt(encCtx, encDataNode, xmlDocGetRootElement(doc)) < 0)
  // if (xmlSecEncCtxXmlEncrypt(encCtx, encDataNode, get_soap_body_content(xmlDocGetRootElement(doc))) < 0)
  {
    log_error1("encryption failed");
    return -1;
  }

  {
    FILE *fp;

    fp = fopen("test-enc-new.xml", "w");
    xmlDocDump(fp, doc);
    fclose(fp);
  }
  fprintf(stdout, "*** after encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "************************\n");

  if (encCtx != NULL)
    xmlSecEncCtxDestroy(encCtx);

  if (encDataNode != NULL)
    xmlFreeNode(encDataNode);

  if (doc != NULL)
    xmlFreeDoc(doc);

  return 0;
}

/**
 *
 * files_keys_store_find_key:
 *
 * @store:              the pointer to simple keys store.
 * @name:               the desired key name.
 * @keyInfoCtx:         the pointer to <dsig:KeyInfo/> node processing context.
 *
 * Lookups key in the @store. The caller is responsible for destroying
 * returned key with #xmlSecKeyDestroy function.
 *
 * Returns pointer to key or NULL if key not found or an error occurs.
 *
 */
static xmlSecKeyPtr
_soap_xmlsec_files_keys_store_find_key(xmlSecKeyStorePtr store, const xmlChar * name, xmlSecKeyInfoCtxPtr keyInfoCtx)
{
  xmlSecKeyPtr  key;
  xmlURI *uri;
  char *file;

  fprintf(stderr, "trying to find key \"%s\"\n", name);

  /*
   * it's possible to do not have the key name or desired key type *
   * but we could do nothing in this case
   */
  if (name == NULL)
  {
    printf("key name is NULL\n");
    return NULL;
  }

  if (keyInfoCtx->keyReq.keyId == xmlSecKeyDataIdUnknown)
         {
    printf("keyReq.keyID == xmlSecKeyDataIdUnkown\n");
    return NULL;
  }

  if (!(uri = xmlParseURI(name)))
  {
    printf("xmlParseURI failed\n");
    return NULL;
  }

  // printf("uri->scheme=\"%s\"\n", uri->scheme);
  // printf("uri->server=\"%s\"\n", uri->server);
  // printf("uri->port=\"%i\"\n", uri->port);
  // printf("uri->path=\"%s\"\n", uri->path);

  {
          char *tmp;
    char buf[4096];
    FILE *fp;

    if (!(tmp = getenv("CSOAP_TEMP")))
    {
      tmp = "/tmp";
    }
    // printf("tmp = \"%s\"\n", tmp);

    sprintf(buf, "%s/csoap-XXXXXX-key.pem", tmp);
    // printf("buf = \"%s\"\n", buf);

    if (mkstemps(buf, 8) < 0)
    {
      printf("mkstemps failed (%s)", strerror(errno));
      return NULL;
    }

    // printf("fopen(\"%s\")\n", buf);
    file = strdup(buf);

    if (!(fp = fopen(buf, "w")))
    {
      printf("fopen failed (%s)\n", strerror(errno));
      return NULL;
    }

    if (!strcmp("https", uri->scheme) || !strcmp("http", uri->scheme))
    {
      size_t len;
      httpc_conn_t *conn;
      hresponse_t *res;
      herror_t status;
      
      conn = httpc_new();

      if ((status = (httpc_get(conn, &res, name))) != H_OK)
      {
        log_error2("httpc_get failed (%s)", herror_message(status));
        herror_release(status);
        return NULL;
      }

      while (http_input_stream_is_ready(res->in))
      {
        len = http_input_stream_read(res->in, buf, 4096);
        fwrite(buf, len, 1, fp);
      }
      hresponse_free(res);

      httpc_free(conn);
    }
    fclose(fp);
  }

  if ((keyInfoCtx->keyReq.keyId == xmlSecKeyDataDsaId) || (keyInfoCtx->keyReq.keyId == xmlSecKeyDataRsaId)) {
    /*
     * load key from a pem file, if key is not found then it's an
     * error (is it?)
     */
    key = xmlSecCryptoAppKeyLoad(file, xmlSecKeyDataFormatPem, NULL, NULL, NULL);
    if (key == NULL) {
      fprintf(stderr, "Error: failed to load public pem key from \"%s\"\n", name);
      return (NULL);
    }
  }
  else {
    /*
     * otherwise it's a binary key, if key is not found then it's
     * an error (is it?)
     */
    key = xmlSecKeyReadBinaryFile(keyInfoCtx->keyReq.keyId, file);
    if (key == NULL) {
      fprintf(stderr, "Error: failed to load key from binary file \"%s\"\n", name);
      return (NULL);
    }
  }

  /* set key name */
  if (xmlSecKeySetName(key, name) < 0) {
    fprintf(stderr, "Error: failed to set key name for key from \"%s\"\n", name);
    xmlSecKeyDestroy(key);
    return (NULL);
  }

  if (unlink(file) < 0)
  {
    log_error2("unlink file failed (%s)", strerror(errno));
  }
  free(file);

  return (key);
}

static xmlSecKeyStoreKlass _soap_xmlsec_files_keys_store_klass =
{
  sizeof(xmlSecKeyStoreKlass),
  sizeof(xmlSecKeyStore),
  BAD_CAST "uri-based-keys-store",  /* const xmlChar* name; */
  NULL,      /* xmlSecKeyStoreInitializeMethod initialize; */
  NULL,      /* xmlSecKeyStoreFinalizeMethod finalize; */
  _soap_xmlsec_files_keys_store_find_key,  /* xmlSecKeyStoreFindKeyMethod findKey; */
  /* reserved for the future */
  NULL,      /* void* reserved0; */
  NULL,      /* void* reserved1; */
};

static inline xmlSecKeyStoreId
_soap_xmlsec_files_keys_store_get_klass(void)
{
  return (&_soap_xmlsec_files_keys_store_klass);
}

static herror_t 
_soap_xmlsec_create_key_manager(void)
{
  xmlSecKeyStorePtr keysStore;

  /* create files based keys store */
  keysStore = xmlSecKeyStoreCreate(_soap_xmlsec_files_keys_store_get_klass());
  if (keysStore == NULL)
  {
    log_error1("failed to create keys store");
    return herror_new("_soap_xmlxec_create_key_manager", 0, "failed to create keys store");
  }

  /* create keys manager */
  if ((_soap_xmlsec_key_manager = xmlSecKeysMngrCreate()) == NULL)
  {
    log_error1("failed to create keys manager");
    xmlSecKeyStoreDestroy(keysStore);
    return herror_new("_soap_xmlsec_key_manager", 0, "failed to create keys manager");
  }

  /*
   * add store to keys manager, from now on keys manager destroys the
   * store if needed
   */
  if (xmlSecKeysMngrAdoptKeysStore(_soap_xmlsec_key_manager, keysStore) < 0)
  {
    log_error1("failed to add keys store to keys manager");
    xmlSecKeyStoreDestroy(keysStore);
    xmlSecKeysMngrDestroy(_soap_xmlsec_key_manager);
    _soap_xmlsec_key_manager = NULL;
    return herror_new("_soap_xmlsec_create_key_manager", 0, "failed to add keys store to keys manager");
  }

  /* initialize crypto library specific data in keys manager */
  if (xmlSecCryptoKeysMngrInit(_soap_xmlsec_key_manager) < 0)
  {
    log_error1("failed to initialize crypto data in keys manager");
    xmlSecKeysMngrDestroy(_soap_xmlsec_key_manager);
    _soap_xmlsec_key_manager = NULL;
    return herror_new("_soap_xmlsec_create_key_manager", 0, "failed to initialize crypto data in keys manager");
  }

  /* set the get key callback */
  _soap_xmlsec_key_manager->getKey = xmlSecKeysMngrGetKey;

  return H_OK;
}

static herror_t
_soap_xmlsec_load_key(void)
{
  int err;
  xmlChar keyName[256];

  if ((_soap_xmlsec_key = xmlSecCryptoAppKeyLoad(_soap_xmlsec_keyfile, xmlSecKeyDataFormatPem, _soap_xmlsec_password, NULL, NULL)) == NULL)
  {
    log_error2("xmlSecCryptoAppKeyLoad(\"%s\") failed", _soap_xmlsec_keyfile);
    return herror_new("_soap_xmlsec_load_key", 0, "xmlSecCryptoAppKeyLoad(\"%s\") failed", _soap_xmlsec_keyfile);
  }

  if (_soap_xmlsec_certfile)
  {
    if (xmlSecCryptoAppKeyCertLoad(_soap_xmlsec_key, _soap_xmlsec_certfile, xmlSecKeyDataFormatPem) < 0)
    {
      log_error2("xmlSecCryptoAppKeyCertLoad(\"%s\") failed", _soap_xmlsec_certfile);
      xmlSecKeyDestroy(_soap_xmlsec_key);
      return herror_new("_soap_xmlsec_load_key", 0, "xmlSecCryptoAppKeyCertLoad(\"%s\") failed", _soap_xmlsec_certfile);
    }
  }

  xmlStrPrintf(keyName, 256, "%s/key.pem", soap_server_get_name());
  log_error2("keyName is \"%s\"", keyName);
  if ((err = xmlSecKeySetName(_soap_xmlsec_key, keyName)) < 0)
  {
    xmlSecKeyDestroy(_soap_xmlsec_key);
    log_error3("xmlSecKeySetName(\"%s\") failed (%i)", keyName, err);
    return herror_new("_soap_xmlsec_load_key", 0, "xmlSecKeySetName(\"%s\") failed (%i)", keyName, err);
  }

  xmlSecKeyDebugXmlDump(_soap_xmlsec_key, stdout);
  xmlSecKeyDataDebugXmlDump(xmlSecKeyGetValue(_soap_xmlsec_key), stdout);

  return H_OK;
}

static void
_soap_xmlsec_key_service(httpd_conn_t *conn, struct hrequest_t *req)
{
  char buf[4096];
  size_t len;
  int fd;

  httpd_set_header(conn, HEADER_CONTENT_TYPE, "text/plain");
  if (!(fd = open(_soap_xmlsec_keyfile, O_RDONLY)))
  {
    httpd_send_header(conn, 404, HTTP_STATUS_404_REASON_PHRASE);
    http_output_stream_write_string(conn->out, "Public key not found!\n");
  }
  else
  {
    httpd_send_header(conn, 200, HTTP_STATUS_200_REASON_PHRASE);

    while ((len = read(fd, buf, 4096)) > 0)
      http_output_stream_write(conn->out, buf, len);
  }

  return;
}

static herror_t
_soap_xmlsec_publish_key(void)
{
  herror_t status;

  if ((status = httpd_register("/key.pem", _soap_xmlsec_key_service)) != H_OK)
  {
    log_error2("Cannot register key service (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

static inline void
_soap_xmlsec_parse_arguments(int argc, char **argv)
{
  int i;

  for (i=1; i<=argc; i++)
  {
    if (!strcmp(argv[i-1], CSOAP_ENABLE_XMLSEC))
    {
      _soap_xmlsec_enabled = 1;
    }
    else if (!strcmp(argv[i-1], CSOAP_XMLSEC_KEYFILE))
    {
      _soap_xmlsec_keyfile = strdup(argv[i]);
    }
    else if (!strcmp(argv[i-1], CSOAP_XMLSEC_PASSWORD))
    {
      _soap_xmlsec_password = strdup(argv[i]);
    }
    else if (!strcmp(argv[i-1], CSOAP_XMLSEC_CERTFILE))
    {
      _soap_xmlsec_certfile = strdup(argv[i]);
    }
  }
  return;
}

herror_t
_soap_xmlsec_init(void)
{
  static int initialized = 0;
  int err;
  herror_t status;

  if (initialized)
    return H_OK;

  log_info1("initializing xmlsec1");

  xmlSecErrorsDefaultCallbackEnableOutput(1);
  xmlSecErrorsSetCallback(_soap_xmlsec_error_callback);

  if (xmlSecInit() < 0)
  {
    log_error1("xmlSecInit failed");
    return herror_new("soap_xmlsec_init", 0, "xmlSecInit failed");
  }

  if (xmlSecCheckVersion() != 1)
  {
    log_error1("xmlSecCheckVersion failed, wrong xmlsec version");
    return herror_new("soap_xmlsec_init", 0, "Wrong xmlsec version");
  }

#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
  log_verbose2("loading \"%s\" dynamic", XMLSEC_CRYPTO);
  if (xmlSecCryptoDLLoadLibrary(BAD_CAST XMLSEC_CRYPTO) < 0)
  {
    log_error1("xmlSecCryptoDLLoadLibrary failed");
    return herror_new("soap_xmlsec_init", 0, "xmlSecCryptoDLLoadLibrary failed");
  }
#endif

  if (xmlSecCryptoAppInit(NULL) < 0)
  {
    log_error1("xmlSecCryptoAppInit failed");
    return herror_new("soap_xmlsec_init", 0, "xmlSecCryptoAppInit failed");
  }

  if (xmlSecCryptoInit() < 0)
  {
    log_error1("xmlSecCryptoInit failed");
    return herror_new("soap_xmlsec_init", 0, "xmlSecCryptoInit failed");
  }

  if ((status = _soap_xmlsec_create_key_manager()) != H_OK)
  {
    log_error2("_soap_xmlsec_create_key_manager failed (%s)", herror_message(status));
    return status;
  }

  if ((err = pthread_mutex_init(&_soap_xmlsec_lock, NULL)) < 0)
  {
    log_error2("pthread_mutex_init failed (%s)", strerror(err));
    return herror_new("soap_xmlsec_init", 0, "pthread_mutex_init failed (%s)", strerror(err));
  }

  if ((status = _soap_xmlsec_load_key()) != H_OK)
  {
    log_error2("_soap_xmlsec_load_key failed (%s)", herror_message(status));
    return status;
  }

  initialized = 1;

  return H_OK;
}

herror_t
soap_xmlsec_client_init_args(int argc, char **argv)
{
  _soap_xmlsec_parse_arguments(argc, argv);

  if (!_soap_xmlsec_enabled)
    return H_OK;

  return _soap_xmlsec_init();
}

herror_t
soap_xmlsec_server_init_args(int argc, char **argv)
{
  herror_t status;

  _soap_xmlsec_parse_arguments(argc, argv);

  if (!_soap_xmlsec_enabled)
    return H_OK;

  if ((status = _soap_xmlsec_init()) != H_OK)
  {
    log_error2("_soap_xmlsec_init failed (%s)", herror_message(status));
    return status;
  }

  if ((status = _soap_xmlsec_publish_key()) != H_OK)
  {
    log_error2("_soap_xmlsec_publish_key failed (%s)", herror_message(status));
    return status;
  }

  return H_OK;
}

herror_t soap_xmlsec_sign(struct SoapCtx *context)
{
  xmlNodePtr signature;
  xmlNsPtr ns;
  xmlNodePtr signNode;
  xmlNodePtr refNode;
  xmlNodePtr keyInfoNode;
  xmlSecDSigCtxPtr dsigCtx;
  herror_t ret;
  struct SoapEnv *envelope;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  envelope = context->env;

  ret = H_OK;

  pthread_mutex_lock(&_soap_xmlsec_lock);

  if (!(signNode = xmlSecTmplSignatureCreate(envelope->root->doc, xmlSecTransformExclC14NId, xmlSecTransformRsaSha1Id, NULL)))
  {
    log_error1("xmlSecTmplSignatureCreate failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureCreate failed");
    goto out;
  }

  signature = xmlNewNode(NULL, "Signature");
  ns = xmlNewNs(signature, SOAP_SECURITY_NAMESPACE, SOAP_SECURITY_PREFIX);
  xmlSetNs(signature, ns);

  xmlSetNsProp(signature, envelope->root->ns, BAD_CAST "actor", soap_transport_get_name());
  xmlSetNsProp(signature, envelope->root->ns, BAD_CAST "mustUnderstand", BAD_CAST "1");

  xmlAddChild(envelope->header, signature);

  xmlAddChild(signature, signNode);

  if (!(refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id, "#Body", NULL, NULL)))
  {
    log_error1("xmlSecTmplSignatureAddReference failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureAddReference failed");
    goto out;
  }

  if (!(keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL)))
  {
    log_error1("xmlSecTmplSignatureEnsureKeyInfo failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecTmplSignatureEnsureKeyInfo failed");
    goto out;
  }

  if (xmlSecTmplKeyInfoAddKeyName(keyInfoNode, soap_server_get_name()) == NULL)
  {
    log_error1("xmlSecTmplKeyInfoAddKeyName failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecTmplKeyInfoAddKeyName failed");
    goto out;
  }

  if (!(dsigCtx = xmlSecDSigCtxCreate(_soap_xmlsec_key_manager)))
  {
    log_error1("xmlSecDSigCtxCreate failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecDSigCtxCreate failed");
    goto out;
  }

  dsigCtx->signKey = _soap_xmlsec_key;

  if (xmlSecDSigCtxSign(dsigCtx, signNode) < 0)
  {
    log_error1("xmlSecDSigCtxSign failed");
    ret = herror_new("soap_xmlsec_sign", 0, "xmlSecDSigCtxSign failed");
    goto out;
  }

out:

  pthread_mutex_unlock(&_soap_xmlsec_lock);

  return H_OK;
}

herror_t soap_xmlsec_encrypt(struct SoapCtx *context)
{
  struct SoapEnv *envelope;
  xmlNodePtr encDataNode = NULL;
  xmlNodePtr keyInfoNode = NULL;
  xmlNodePtr encKeyNode = NULL;
  xmlNodePtr keyInfoNode2 = NULL;
  xmlSecEncCtxPtr encCtx = NULL;
  xmlURI *to;
  xmlChar buf[256];
  xmlDocPtr doc;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  envelope = context->env;

  doc = context->env->root->doc;

  fprintf(stdout, "*** before encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "*************************\n");

  encDataNode = xmlSecTmplEncDataCreate(doc, xmlSecTransformDes3CbcId, NULL, xmlSecTypeEncElement, NULL, NULL);
  if (encDataNode == NULL)
  {
    log_error1("xmlSecTmplEnvDataCreate failed");
    return herror_new("soap_xmlsec_encrypt", 0, "xmlSecTmplEnvDataCreate failed");
  }

  if (xmlSecTmplEncDataEnsureCipherValue(encDataNode) == NULL)
  {
    log_error1("failed to add CipherValue node");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to add CipherValue node");
  }

  keyInfoNode = xmlSecTmplEncDataEnsureKeyInfo(encDataNode, NULL);
  if (keyInfoNode == NULL)
  {
    log_error1("failed to add KeyInfo node");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to add KeyInfo node");
  }

  encKeyNode = xmlSecTmplKeyInfoAddEncryptedKey(keyInfoNode, xmlSecTransformRsaOaepId, NULL, NULL, NULL);
  if (encKeyNode == NULL)
  {
    log_error1("failed to add KeyInfo");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to add KeyInfo");
  }

  if (xmlSecTmplEncDataEnsureCipherValue(encKeyNode) == NULL)
  {
    log_error1("failed to add CipherValue node");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to add CipherValue node");
  }

  keyInfoNode2 = xmlSecTmplEncDataEnsureKeyInfo(encKeyNode, NULL);
  if (keyInfoNode2 == NULL)
  {
    log_error1("failed to add key info (2)\n");
    return herror_new("soap_xmlsec_enrypt", 0, "failed to add key info (2)");
  }

  to = soap_addressing_get_to_address(envelope);
  xmlStrPrintf(buf, 256, "http://%s:%i/key.pem", to->server, to->port);
  log_error2("adding key \"%s\"", buf);
  xmlFreeURI(to);

  if (xmlSecTmplKeyInfoAddKeyName(keyInfoNode2, buf) == NULL)
  {
    log_error2("failed to add key name \"%s\"", buf);
    return herror_new("soap_xmlsec_encrypt", 0, "failed to add key name \"%s\"", buf);
  }

  encCtx = xmlSecEncCtxCreate(_soap_xmlsec_key_manager);
  if (encCtx == NULL)
  {
    log_error1("failed to create encryption context");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to create encryption context");
  }

  encCtx->encKey = xmlSecKeyGenerate(xmlSecKeyDataDesId, 192, xmlSecKeyDataTypeSession);
  if (encCtx->encKey == NULL)
  {
    log_error1("failed to generate session key");
    return herror_new("soap_xmlsec_encrypt", 0, "failed to generate session key");
  }

  fprintf(stdout, "*** before encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "*************************\n");

  if (xmlSecEncCtxXmlEncrypt(encCtx, encDataNode, soap_env_get_method(envelope)) < 0)
  // if (xmlSecEncCtxXmlEncrypt(encCtx, encDataNode, get_soap_body_content(xmlDocGetRootElement(doc))) < 0)
  {
    log_error1("encryption failed");
    return herror_new("soap_xmlsec_encrypt", 0, "encryption failed");
  }

/*  {
    FILE *fp;

    fp = fopen("test-enc-new.xml", "w");
    xmlDocDump(fp, doc);
    fclose(fp);
  } */

  fprintf(stdout, "*** after encryption ***\n");
  xmlDocFormatDump(stdout, doc, 1);
  fprintf(stdout, "************************\n");

  if (encCtx != NULL)
    xmlSecEncCtxDestroy(encCtx);

  return H_OK;
}

herror_t soap_xmlsec_decrypt(struct SoapCtx *context)
{
  struct SoapEnv *envelope;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  envelope = context->env;

  log_error1("not implemented");
  return herror_new("soap_xmlsec_decrupt", 0, "not implemented");
}

herror_t soap_xmlsec_verify(struct SoapCtx *context)
{
  struct SoapEnv *envelope;

  if (!_soap_xmlsec_enabled)
    return H_OK;

  envelope = context->env;

  log_error1("not implemented");
  return herror_new("soap_xmlsec_verify", 0, "not implemented");
}

void soap_xmlsec_destroy(void)
{
  if (!_soap_xmlsec_enabled)
    return;

  xmlSecKeysMngrDestroy(_soap_xmlsec_key_manager);

  xmlSecCryptoShutdown();

  xmlSecCryptoAppShutdown();

  xmlSecShutdown();

#ifndef XMLSEC_NO_XSLT
  xsltCleanupGlobals();
#endif

  return;
}
