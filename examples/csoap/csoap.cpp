// csoap.cpp : Defines the entry point for the console application.
//

/******************************************************************
* $Id: csoap.cpp,v 1.1 2004/08/26 17:00:18 rans Exp $
*
* CSOAP Project:  CSOAP examples project
* Copyright (C) 2003  Ferhat Ayaz
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
*
* Email: ayaz@jprogrammer.net
******************************************************************/

#include "stdafx.h"

#include <libcsoap/soap-client.h>

//static const char *url = "http://csoap.sourceforge.net/cgi-bin/csoapserver";

static const char *url = "http://localhost:10000/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";

/**
* print_xpath_nodes:
* @nodes:		the nodes set.
* @output:		the output file handle.
*
* Prints the @nodes content to @output.
*/
void print_xpath_nodes(xmlNodeSetPtr nodes, FILE* output) {
	xmlNodePtr cur;
	int size;
	int i;

	size = (nodes) ? nodes->nodeNr : 0;

	fprintf(output, "Result (%d nodes):\n", size);
	for(i = 0; i < size; ++i) {
		if(nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
			xmlNsPtr ns;

			ns = (xmlNsPtr)nodes->nodeTab[i];
			cur = (xmlNodePtr)ns->next;
			if(cur->ns) { 
				fprintf(output, "= namespace \"%s\"=\"%s\" for node %s:%s\n", 
					ns->prefix, ns->href, cur->ns->href, cur->name);
			} else {
				fprintf(output, "= namespace \"%s\"=\"%s\" for node %s\n", 
					ns->prefix, ns->href, cur->name);
			}
		} else if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
			cur = nodes->nodeTab[i];   	    
			if(cur->ns) { 
				fprintf(output, "= element node \"%s:%s\"\n", 
					cur->ns->href, cur->name);
			} else {
				fprintf(output, "= element node \"%s\"\n", 
					cur->name);
			}
		} else {
			cur = nodes->nodeTab[i];    
			fprintf(output, "= node \"%s\": type %d\n", cur->name, cur->type);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SoapEnv *env, *res;
	xmlXPathObjectPtr xpathObj; 
//	const char xpathExpr[]="//*";
	const char xpathExpr[]="//Envelope/*";
	log_set_level(HLOG_VERBOSE);

	env = soap_env_new_with_method(urn, method);
	soap_env_add_item(env, "xsd:string", "name", "Jonny B. Good");

	if (argc > 1)
		res = soap_client_invoke(env, argv[1], "");
	else
		res = soap_client_invoke(env, url, "");

	xpathObj = soap_xpath_eval(res->root->doc,xpathExpr);
	soap_env_get_body(env);
	if(xpathObj!=NULL)
	{
		print_xpath_nodes(xpathObj->nodesetval, stdout);
	}
	soap_xml_doc_print(res->root->doc);

	soap_env_free(res);
	soap_env_free(env);
	xmlXPathFreeObject(xpathObj);

	printf("Press ENTER to Continue");
	char c;
	gets(&c);
	return 0;
}