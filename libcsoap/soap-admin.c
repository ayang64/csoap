

#define SOAP_ADMIN_QUERY_ROUTERS "routers"
#define SOAP_ADMIN_QUERY_ROUTER "router"
#define SOAP_ADMIN_QUERY_SERVICES "services"


static void
_soap_admin_send_title(httpd_conn_t* conn, const char* title)
{
  httpd_send_header(conn, 200, "OK");
  http_output_stream_write_string(conn->out,
   "<html><head><style>");
  http_output_stream_write_string(conn->out,
   ".logo {"
   " color: #005177;"
   " background-color: transparent;"
   " font-family: Calligraphic, arial, sans-serif;"
   " font-size: 36px;"
   "}");
  http_output_stream_write_string(conn->out,
   "</style></head><body><span class=\"logo\">csoap</span> ");
  http_output_stream_write_string(conn->out, title);
  http_output_stream_write_string(conn->out, "<hr />");
}


static void
_soap_admin_list_routers(httpd_conn_t* conn)
{
  SoapRouterNode *node;
  char buffer[1024];

  _soap_admin_send_title(conn, "Available routers");

  for (node = head; node; node = node->next)
  {
    sprintf(buffer, "<li /> <a href=\"?" SOAP_ADMIN_QUERY_ROUTER "=%s\"> %s",
	    node->context, node->context);
    http_output_stream_write_string(conn->out, buffer);
  }
  
  http_output_stream_write_string(conn->out, "</body></html>");
}

static void
_soap_admin_list_services(httpd_conn_t* conn, const char* routername)
{
  SoapRouter *router;
  SoapServiceNode *node;
  char buffer[1024];
  
  sprintf(buffer, "Listing Services for Router <b>%s</b>", routername);
  _soap_admin_send_title(conn, buffer);

  router = router_find(routername);
  if (!router) {
    http_output_stream_write_string(conn->out, "Router not found!");
    http_output_stream_write_string(conn->out, "</body></html>");
    return;
  }

  node = router->service_head;

  while (node) {
    sprintf(buffer, "<li /> [%s] (%s)",
	    node->service->urn,
	    node->service->method);
    http_output_stream_write_string(conn->out, buffer);
    node = node->next;
  }

  http_output_stream_write_string(conn->out, "</body></html>");
}



static void
_soap_admin_handle_get(httpd_conn_t * conn, hrequest_t * req)
{
  char *param;

  if ((param = hpairnode_get_ignore_case(req->query, SOAP_ADMIN_QUERY_ROUTERS))) {
    _soap_admin_list_routers(conn);
  } else if ((param = hpairnode_get_ignore_case(req->query, SOAP_ADMIN_QUERY_ROUTER))) {
    _soap_admin_list_services(conn, param);
  } else {
    _soap_admin_send_title(conn, "Welcome to the admin site");
    http_output_stream_write_string(conn->out,
     "<li /> <a href=\"?" SOAP_ADMIN_QUERY_ROUTERS "\"> Routers </a>");
    http_output_stream_write_string(conn->out,
     "</body></html>");
  }
}

static void
_soap_admin_entry(httpd_conn_t * conn, hrequest_t * req)
{
  if (req->method == HTTP_REQUEST_GET) {
    _soap_admin_handle_get(conn, req);
  } else {
    httpd_send_header(conn, 200, "OK");
    http_output_stream_write_string(conn->out,
              "<html>"
                  "<head>"
		  "</head>"
		  "<body>"
                      "<h1>Sorry!</h1>"
                      "<hr />"
                      "<div>POST Service is not implemented now. Use your browser</div>"
                  "</body>"
              "</html>");
  }
}
