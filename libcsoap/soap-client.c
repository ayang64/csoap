/******************************************************************
*  $Id: soap-client.c,v 1.9 2004/10/20 14:17:36 snowdrop Exp $
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
#include <libcsoap/soap-client.h>
#include <nanohttp/nanohttp-client.h>
#include <string.h>

/*--------------------------------- */
static int _block_socket = 0;
static SoapEnv *_soap_client_build_result(hresponse_t *res);
/*--------------------------------- */

void soap_client_block_socket(int block)
{
	_block_socket = block;
}

int soap_client_get_blockmode()
{
	return _block_socket;
}



int soap_client_init_args(int argc, char *argv[])

{

	return !httpc_init(argc, argv);

}


static
long _file_get_size(const char* filename)
{
  FILE *f = fopen(filename, "r");
  long size;

  if (!f) return -1;

  fseek(f, 0, SEEK_END);
  size=ftell(f);  
  fclose(f);
  return size;
}

SoapCtx*
soap_client_invoke(SoapCtx *call, const char *url, const char *soap_action)
{
  /* Status */
  hstatus_t status;

	/* Result document */
	SoapEnv* doc;

	/* Result Context */
	SoapCtx *ctx;

	/* Buffer variables*/
	xmlBufferPtr buffer;
	char *content;
	char tmp[15];

	/* Transport variables */
	httpc_conn_t *conn;
	hresponse_t *res;

	/* multipart/related start id*/
	char start_id[150];
	static int counter=1;
	part_t *part;
	int file_count=0;

	/* Create buffer */
	buffer = xmlBufferCreate();
	xmlNodeDump(buffer, call->env->root->doc,call->env->root, 1 ,0);
	content = (char*)xmlBufferContent(buffer);

	/* Transport via HTTP */
	conn = httpc_new();
	conn->block = soap_client_get_blockmode();

	/* Set soap action */
	if (soap_action != NULL) {
		httpc_set_header(conn, "SoapAction", soap_action);
	}

	/* check for attachments */
	if (!call->attachments)
	{
  	/* content-type is always 'text/xml' */
  	httpc_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");
  
    sprintf(tmp, "%d", (int)strlen(content));
    httpc_set_header(conn, HEADER_CONTENT_LENGTH, tmp);
    status = httpc_post_begin(conn, url);
  
    if (status != H_OK) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client, 
          			"Can not begin post envelope","",""));
    }
  
    status = http_output_stream_write_string(conn->out, content);
    if (status != H_OK) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client,   
  		         	"Can not post envelope","",""));
    }
  
  /*	res = httpc_post(conn, url, (int)strlen(content), content);*/
    res = httpc_post_end(conn);
    if (res == NULL) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		         soap_env_new_with_fault(Fault_Client, 
           			conn->errmsg,"",""));
    }
	} 
	else
	{
	
	  /* Use content-length transport */
    /*for (part=call->attachments->parts; part; part=part->next) {
      file_size = _file_get_size(part->filename);
      if (file_size == -1) [
        httpc_free(conn);
      	xmlBufferFree(buffer);
    		return soap_ctx_new(
    		        soap_env_new_with_fault(Fault_Client,   
    		         	"Can not open file!","",""));
      }
      
      total_size += file_size + BOUDARY_LENGTH;
    }*/

    /* Use chunked transport */
    httpc_set_header(conn, HEADER_TRANSFER_ENCODING, TRANSFER_ENCODING_CHUNKED);
    
  	sprintf(start_id, "289247829121218%d", counter++);
	  status = httpc_mime_begin(conn, url, start_id, "", "text/xml");
    if (status != H_OK) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client,   
  		         	"Can not begin MIME transport","",""));
    }
  
    status = httpc_mime_next(conn, start_id, "text/xml", "binary");
    if (status != H_OK) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client,   
  		         	"MIME transport error","",""));
    }

    status = http_output_stream_write(conn->out, content, strlen(content));
    if (status != H_OK) {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client,   
  		         	"MIME transport error","",""));
    }

    
    for (part=call->attachments->parts; part; part=part->next) 
    {
        status = httpc_mime_send_file(conn, part->id, 
                part->content_type, part->transfer_encoding, part->filename);
        if (status != H_OK) {
          log_error2("Send file failed. Status:%d", status);
          httpc_free(conn);
        	xmlBufferFree(buffer);
      		return soap_ctx_new(
      		        soap_env_new_with_fault(Fault_Client,   
      		         	"MIME transport error while sending file","",""));
        }
    }

    res = httpc_mime_end(conn);
    if (!res)
    {
      httpc_free(conn);
    	xmlBufferFree(buffer);
  		return soap_ctx_new(
  		        soap_env_new_with_fault(Fault_Client,   
  		         	"MIME transport error","",""));
    }
	}

	/* Free buffer */
	xmlBufferFree(buffer);

	/* Build result */
	/* TODO: If res == NULL, find out where and why it is NULL! */
	doc = _soap_client_build_result(res); 

	/* Create Context */
	ctx = soap_ctx_new(doc);
	soap_ctx_add_files(ctx, res->attachments);

	return ctx;
}


static 
SoapEnv* _soap_client_build_result(hresponse_t *res)
{
	SoapEnv *env;

	log_verbose2("Building result (%p)", res);

	if (res == NULL)
		return soap_env_new_with_fault(Fault_Client, 
		"Response is NULL","","");


	if (res->in == NULL)
		return soap_env_new_with_fault(Fault_Client, 
		"Empty response from server!","","");


/*
	doc = xmlParseDoc(BAD_CAST res->body);
	if (doc == NULL) {
		return soap_env_new_with_fault(Fault_Client, 
			"Response is not in XML format!","","");
	}

	env = soap_env_new_from_doc(doc);
*/
  env = soap_env_new_from_stream(res->in);

	if (env == NULL) {
/*		xmlFreeDoc(doc);*/
		return soap_env_new_with_fault(Fault_Client, 
			"Can not create envelope","","");
	}

	return env;
}


SoapCtx *soap_client_ctx_new(const char *urn, const char *method)
{
  SoapCtx *ctx = soap_ctx_new(soap_env_new_with_method(urn, method));

  return ctx;
}

