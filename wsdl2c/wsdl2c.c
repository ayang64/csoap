/******************************************************************
 *  $Id: wsdl2c.c,v 1.5 2004/06/03 08:53:34 snowdrop Exp $
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
 */
#include <libxml/xpath.h>
#include <xsd2c/xsd2c.h>
#include <xsd2c/util.h> /* parseNS */
#include <xsd2c/formatter.h> /* formatter sax serializer */
#include <xsd2c/obj.h> /* formatter sax serializer */
#include <string.h>

xmlXPathObjectPtr xpath_eval(xmlDocPtr doc, const char *xpath)
{
  xmlXPathContextPtr context;
  xmlXPathObjectPtr result;
  xmlNodePtr node;
  xmlNsPtr default_ns;

  node = xmlDocGetRootElement(doc);
  if (node == NULL) {
    fprintf(stderr,"empty document\n");
    return NULL;
  }

  
  context = xmlXPathNewContext(doc);

  context->namespaces = xmlGetNsList(doc, node);
  context->nsNr = 0;
  if (context->namespaces != NULL) {
     while (context->namespaces[context->nsNr] != NULL) {
/*      fprintf(stdout, "NS: '%s' - '%s'\n", context->namespaces[context->nsNr]->href, 
  context->namespaces[context->nsNr]->prefix); */
      context->nsNr++;
     }
  }

  /* search default namespace */
  default_ns = xmlSearchNs(doc, node, NULL);
  if (default_ns != NULL && default_ns->href != NULL) {
    fprintf(stdout, "Register default namespace '%s'\n", (const char*)default_ns->href);
    xmlXPathRegisterNs(context, "wsdl", default_ns->href ); /* default_ns : "http://schemas.xmlsoap.org/wsdl/" */
  } 
  


  result = xmlXPathEvalExpression((xmlChar*)xpath, context);  
  if (result == NULL) {
    fprintf(stderr, "ERROR: XPath error!\n");
    return NULL;
  }

  if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
    /* no result */
    return NULL;
  }

  xmlXPathFreeContext(context);
  return result;  
}

xmlNodePtr findPortType(xmlDocPtr doc, const char *name)
{
  xmlNodePtr root;
  xmlNodePtr cur;
  xmlChar *attr_name;

  root = xmlDocGetRootElement(doc);
  if (root == NULL) {
    fprintf(stderr, "Empty document!\n");
    return;
  }

  cur = root->xmlChildrenNode;

  while (cur != NULL) {
    
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (xmlStrcmp(cur->name, (const xmlChar*)"portType")) {
      cur = cur->next;
      continue;
    }

    attr_name = xmlGetProp(cur, "name");  
    if (attr_name == NULL) {
      cur = cur->next;
      continue;
    }

    if (!xmlStrcmp(attr_name, (const xmlChar*)name)) {
      xmlFree(attr_name);
      return cur;
    }
    
    xmlFree(attr_name);
    cur = cur->next;
  }

  return NULL;
}


xmlNodePtr findMessage(xmlDocPtr doc, const char *name)
{
  xmlNodePtr root;
  xmlNodePtr cur;
  xmlChar *attr_name;

  root = xmlDocGetRootElement(doc);
  if (root == NULL) {
    fprintf(stderr, "Empty document!\n");
    return;
  }

  cur = root->xmlChildrenNode;

  while (cur != NULL) {
    
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (xmlStrcmp(cur->name, (const xmlChar*)"message")) {
      cur = cur->next;
      continue;
    }

    attr_name = xmlGetProp(cur, "name");  
    if (attr_name == NULL) {
      cur = cur->next;
      continue;
    }

    if (!xmlStrcmp(attr_name, (const xmlChar*)name)) {
      xmlFree(attr_name);
      return cur;
    }
    
    xmlFree(attr_name);
    cur = cur->next;
  }

  return NULL;
}


xmlNodePtr findSubNode(xmlNodePtr root, const char *element_name)
{
  xmlNodePtr cur;

  cur = root->xmlChildrenNode;

  while (cur != NULL) {
    
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (!xmlStrcmp(cur->name, (const xmlChar*)element_name)) {
      return cur;
    }

    cur = cur->next;
  }

  return NULL;
}


void handleInputParameters(xmlDocPtr doc, const char *name)
{
  char ns[10];
  char message_name[255];
  xmlNodePtr node;
  xmlNodePtr cur;

  xmlChar *var_name, *var_type;

  parseNS(name, ns, message_name); /* check why to pase ns? */

  node = findMessage(doc, message_name);
  if (node == NULL) {
    fprintf(stderr, "ERROR: Can not find message '%s'\n", message_name);
    return;
  }

  cur = node->xmlChildrenNode;
  while (cur != NULL) {
    
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (xmlStrcmp(cur->name, (const xmlChar*)"part")) {
      cur = cur->next;
      continue;
    }

    var_name = xmlGetProp(cur, "name");  
    if (var_name == NULL) {
      fprintf(stderr, "ERROR: Found <part> without attrbute 'name'! (message:'%s')\n", 
        message_name);
      cur = cur->next;
      continue;
    }

    var_type = xmlGetProp(cur, "type");  
    if (var_type == NULL) {
      var_type = xmlGetProp(cur, "element");  
    } 

    if (var_type == NULL) {
      fprintf(stderr, "ERROR: Found <part> without attrbute 'type' or 'element'! (message:'%s')\n", 
        message_name);
      xmlFree(var_name);
      cur = cur->next;
      continue;
    } 


    fprintf(stdout, "\t\t(%s,%s)\n", trXSD2C((const char*)var_type), (const char*)var_name);

    xmlFree(var_name);
    xmlFree(var_type);

    cur = cur->next;
  }

}

void handlePortType(xmlDocPtr doc, const char *name)
{
  xmlNodePtr node;
  xmlNodePtr input;
  xmlNodePtr output;
  xmlNodePtr cur;
  xmlChar *attr_name;
  xmlChar *message;

  char opname[255];

  node = findPortType(doc, name);
  if (node == NULL) {
    fprintf(stderr, "PortType '%s' not found!\n", name);
    return;
  }

  cur = node->xmlChildrenNode;
  while (cur != NULL) {
    
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (xmlStrcmp(cur->name, (const xmlChar*)"operation")) {
      cur = cur->next;
      continue;
    }

    attr_name = xmlGetProp(cur, "name");  
    if (attr_name == NULL) {
      cur = cur->next;
      continue;
    }

    strcpy(opname, (const char*)attr_name);
    xmlFree(attr_name);
    fprintf(stdout, "Operation -> '%s'\n", opname);

    /* handle input */
    input = findSubNode(cur, "input");
    if (input == NULL) {
      fprintf(stderr, "WARNING: No input for operation '%'\n", opname);
      cur = cur->next;
      continue;
    }

    message = xmlGetProp(input, "message");  
    if (message == NULL) {
      fprintf(stderr, "ERROR: No message attribute for input operation '%'\n", opname);
      cur = cur->next;
      continue;
    }
    
    fprintf(stdout, "\tinput  = '%s'\n", (const char*)message);
    
    handleInputParameters(doc, (const char*)message);

    xmlFree(message);
  
    /* handle output */
    output = findSubNode(cur, "output");
    if (output == NULL) { /* one way operation is ok */
      cur = cur->next;
      continue;
    }

    message = xmlGetProp(output, "message");  
    if (message == NULL) {
      fprintf(stderr, "ERROR: No message attribute for output operation '%'\n", opname);
      cur = cur->next;
      continue;
    }
    
    fprintf(stdout, "\toutput = '%s'\n", (const char*)message);
    xmlFree(message);
  
    cur = cur->next;
  }

}

void handleBinding(xmlDocPtr doc, const char *type)
{
  
  int i;
  xmlChar *name;
  xmlChar *btype;
  xmlNodeSetPtr nodeset; 
  xmlXPathObjectPtr xpathObj;
  char query[255];
  char port_name[255], port_ns[10];
  char binding_type[255], binding_ns[10];

  parseNS(type, binding_ns, binding_type);

  /* Set xpath  query */
  sprintf(query, "//wsdl:definitions/wsdl:binding[@name=\"%s\"]", binding_type);

  /* Find Bindings */
  xpathObj = xpath_eval(doc, query);
  
  if (xpathObj == NULL) {
    fprintf(stderr, "No Soap Binding found!\n");
    return;
  }

  /* Check if found nodes */
  nodeset = xpathObj->nodesetval;
  if (nodeset == NULL) {
    fprintf(stderr, "No Soap Binding found! nodeset empty!\n");
    return;
  }

  /* Iterate all soap services */
  for (i=0;i < nodeset->nodeNr; i++) {
    
    name = xmlGetProp(nodeset->nodeTab[i], "name");

    if (name == NULL) {
      fprintf(stderr, "WARNING: no attribute name found!\n");
    } else {
      fprintf(stdout, "Found Binding -> (name: '%s')\n", 
        (const char*)name);
    }

    btype = xmlGetProp(nodeset->nodeTab[i], "type");

    if (btype == NULL) {
      fprintf(stderr, "WARNING: no attribute type found!\n");
    } else {
      parseNS(btype, port_ns, port_name);
      handlePortType(doc, port_name);
    }

    if (name != NULL) xmlFree(name);
    if (btype != NULL) xmlFree(btype);  
  }
  
}

void wsdlParse(xmlDocPtr doc)
{
  int i;
  xmlChar *name, *binding;
  xmlNodeSetPtr nodeset; 
  xmlXPathObjectPtr xpathObj;


  /* Find Soap Service */
  xpathObj = xpath_eval(doc, "//wsdl:definitions/wsdl:service/wsdl:port[soap:address]");
  
  if (xpathObj == NULL) {
    fprintf(stderr, "No Soap Service (soap:address) found!\n");
    return;
  }

  /* Check if found nodes */
  nodeset = xpathObj->nodesetval;
  if (nodeset == NULL) {
    fprintf(stderr, "No Soap Service (soap:address) found! nodeset empty!\n");
    return;
  }

  /* Iterate all soap services */
  for (i=0;i < nodeset->nodeNr; i++) {
    
    name = xmlGetProp(nodeset->nodeTab[i], "name");
    binding = xmlGetProp(nodeset->nodeTab[i], "binding");

    if (name == NULL) {
      fprintf(stderr, "WARNING: no attribute type found!\n");
    } if (binding == NULL) {
      fprintf(stderr, "WARNING: no attribute binding found!\n");
    } else {
      fprintf(stdout, "Found SOAP port -> (type: '%s') (binding: '%s')\n", 
        (const char*)name, (const char*)binding);
    }

    handleBinding(doc, binding);

    if (name != NULL) xmlFree(name);
    if (binding != NULL) xmlFree(binding);
  }
  

}


static
void usage(const char* execname);

int main(int argc, char *argv[])
{
  xmlDocPtr doc;
  xmlNodePtr cur;
  xmlNodePtr node;
  xmlNodePtr xsdRoot;
  xmlNsPtr default_ns;
  int i;
  char outDir[1054];
  char fname[255];

  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }


  
 strcpy(outDir, ".");

  for (i=1;i<argc;i++) 
  {
    if (!strcmp(argv[i], "-d"))
      if (i==argc-1) usage(argv[0]);
      else strcpy(outDir, argv[++i]);
    else if (!strcmp(argv[i], "-S"))
      formatter_generate_sax_serializer = 1;
    else strcpy(fname, argv[i]);
  }
  

  xsdRoot = wsdlLoadFile(fname);
  
  if (xsdRoot == NULL ) 
  {
    fprintf(stderr,"No Schema data found\n");
    doc = xmlParseFile(fname);
    if (doc == NULL) 
    {
	    fprintf(stderr,"Can not parse document\n");
	    return 1;
    }
  } 
  else 
  {
	  doc = xsdRoot->doc;
  }
  

  node = xmlDocGetRootElement(doc);
  if (node == NULL) {
    fprintf(stderr,"empty document\n");
    xmlFreeDoc(doc);
    return 1;
  }

  /* search default namespace */
  default_ns = xmlSearchNs(doc, node, NULL);
  if (default_ns == NULL || ( default_ns != NULL && default_ns->href == NULL)) {
    fprintf(stdout, "Adding default namespace 'http://schemas.xmlsoap.org/wsdl/'\n");
    xmlNewDocProp(doc,  "xmlns", "http://schemas.xmlsoap.org/wsdl/");
  } 

  if (!xsdInitTrModule(xsdRoot))
    return 1;

  if (!xsdInitObjModule(xsdRoot))
    return 1;

  
	
  if (xsdEngineRun(xsdRoot, outDir)) {
   fprintf(stderr, "xsd2c engine error\n");
  	return 1;
  }

  wsdlParse(doc);

  trFreeModule();
  objFreeModule();

  xmlFreeDoc(doc);
  return 0;
}



static
void usage(const char* execname)
{
	fprintf(stderr, "usage: %s -d dest <wsdl file>\n", execname);	
}

