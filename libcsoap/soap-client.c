/******************************************************************
*  $Id: soap-client.c,v 1.7 2004/09/19 07:05:03 snowdrop Exp $
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
static SoapEnv *_soap_client_build_result(hresponse_t *res);
/*--------------------------------- */


int soap_client_init_args(int argc, char *argv[])

{

	return !httpc_init(argc, argv);

}

SoapEnv*
soap_client_invoke(SoapEnv *call, const char *url, const char *soap_action)
{
	/* Result document */
	SoapEnv* doc;

	/* Buffer variables*/
	xmlBufferPtr buffer;
	char *content;

	/* Transport variables */
	httpc_conn_t *conn;
	hresponse_t *res;

	/* Create buffer */
	buffer = xmlBufferCreate();
	xmlNodeDump(buffer, call->root->doc,call->root, 1 ,0);
	content = (char*)xmlBufferContent(buffer);

	/* Transport via HTTP */
	conn = httpc_new();

	/* content-type is always 'text/xml' */
	httpc_set_header(conn, HEADER_CONTENT_TYPE, "text/xml");

	if (soap_action != NULL) {
		httpc_set_header(conn, "SoapAction", soap_action);
	}
	res = httpc_post(conn, url, (int)strlen(content), content);

	/* Free buffer */
	xmlBufferFree(buffer);

	/* Build result */
	/* TODO: If res == NULL, find out where and why it is NULL! */
	doc = _soap_client_build_result(res); 

	return doc;
}


static 
SoapEnv* _soap_client_build_result(hresponse_t *res)
{
	xmlDocPtr doc;
	SoapEnv *env;

	log_verbose2("Building result (%p)", res);

	if (res == NULL)
		return soap_env_new_with_fault(Fault_Client, 
		"Response is NULL","","");

	if (res->body == NULL)
		return soap_env_new_with_fault(Fault_Client, 
		"Empty response from server!","","");



	doc = xmlParseDoc(BAD_CAST res->body);
	if (doc == NULL) {
		return soap_env_new_with_fault(Fault_Client, 
			"Response is not in XML format!","","");
	}

	env = soap_env_new_from_doc(doc);

	if (env == NULL) {
		xmlFreeDoc(doc);
		return soap_env_new_with_fault(Fault_Client, 
			"Can not create envelope","","");
	}

	return env;
}

