// This is the main project file for VC++ application project
// generated using an Application Wizard.

#include "stdafx.h"
#include <libcsoap/soap-server.h>


static const char *url = "/csoapserver";
static const char *urn = "urn:examples";
static const char *method = "sayHello";


void add_name(xmlNodePtr node, SoapEnv *env)
{
	char *name;
	name = (char*)xmlNodeListGetString(node->doc,
		node->xmlChildrenNode, 1);


	if (!name) return;

	soap_env_add_itemf(env,"xsd:string", "echo",
		"Hello '%s'", name);

	xmlFree((xmlChar*)name);

}


SoapEnv* say_hello(SoapEnv *request)
{

	SoapEnv *env;
	xmlNodePtr method, node;

	env = soap_env_new_with_response(request);

	method = soap_env_get_method(request);
	node = soap_xml_get_children(method);

	while (node) {
		add_name(node, env);
		node = soap_xml_get_next(node);
	}

	return env;
}

int _tmain(int argc, _TCHAR* argv[])
{

	SoapRouter *router;

	log_set_level(HLOG_VERBOSE);

	if (!soap_server_init_args(argc, argv)) {
		return 0;
	}

	router = soap_router_new();
	soap_router_register_service(router, say_hello, method, urn);
	soap_server_register_router(router, url);

	log_info1("send SIGTERM to shutdown");
	soap_server_run();

	log_info1("shutting down\n");
	soap_server_destroy();

	return 0;
}
