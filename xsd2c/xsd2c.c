/******************************************************************
 *  $Id: xsd2c.c,v 1.1 2004/06/02 11:17:03 snowdrop Exp $
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
 * Email: ayaz@jprogrammet.net
 ******************************************************************/

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


#include <stdio.h>
#include "obj.h"
#include "tr.h"
#include "formatter.h"


#define NODE_NAME_EQUALS(xmlnode, text) \
  (!xmlStrcmp(xmlnode->name, (const xmlChar *)text))


#define   XSD_ALL_STR   "all"
#define   XSD_ANNOTATION_STR		"annotation"
#define   XSD_ANY_STR		""
#define   XSD_ANY_ATTRIBUTE_STR		"any"
#define   XSD_APPINFO_STR		"appInfo"
#define   XSD_ATTRIBUTE_STR		"attribute"
#define   XSD_ATTRIBUTE_GROUP_STR		"attributeGroup"
#define   XSD_CHOICE_STR		"choice"
#define   XSD_COMPLEX_TYPE_STR		"complexType"
#define   XSD_COMPLEX_CONTENT_STR		"complexContent"
#define   XSD_DOCUMENTATION_STR		"documentation"
#define   XSD_ELEMENT_STR		"element"
#define   XSD_EXTENSION_STR		"extension"
#define   XSD_FIELD_STR		"field"
#define   XSD_GROUP_STR		"group"
#define   XSD_IMPORT_STR		"import"
#define   XSD_INCLUDE_STR		"include"
#define   XSD_KEY_STR		"key"
#define   XSD_KEYREF_STR		"keyref"
#define   XSD_LIST_STR		"list"
#define   XSD_NOTATION_STR		"notation"
#define   XSD_REDEFINE_STR		"redefine"
#define   XSD_RESTRICTION_STR		"restriction"
#define   XSD_SCHEMA_STR		"schema"
#define   XSD_SELECTOR_STR		"selector"
#define   XSD_SEQUENCE_STR		"sequence"
#define   XSD_SIMPLE_CONTENT_STR		"simpleContent"
#define   XSD_SIMPLE_TYPE_STR		"simpleType"
#define   XSD_UNION_STR		"union"
#define   XSD_UNIQUE_STR		"unique"

#define   ATTR_TYPE_STR "type"
#define   ATTR_NAME_STR "name"
#define   ATTR_BASE_STR "base"
#define   ATTR_MIN_OCCURS_STR "minOccurs"
#define   ATTR_MAX_OCCURS_STR "maxOccurs"

#define   ATTR_VALUE_UNBOUNDED "unbounded"

enum _xsdAttr
{
  ATTR_UNKNOWN,
  ATTR_TYPE,
  ATTR_NAME,
  ATTR_BASE,
  ATTR_MIN_OCCURS,
  ATTR_MAX_OCCURS
};

enum _xsdKeyword 
{
  XSD_UNKNOWN,
  XSD_ALL,
  XSD_ANNOTATION, 
  XSD_ANY, 
  XSD_ANY_ATTRIBUTE, 
  XSD_APPINFO, 
  XSD_ATTRIBUTE, 
  XSD_ATTRIBUTE_GROUP, 
  XSD_CHOICE, 
  XSD_COMPLEX_TYPE, 
  XSD_COMPLEX_CONTENT, 
  XSD_DOCUMENTATION, 
  XSD_ELEMENT, 
  XSD_EXTENSION, 
  XSD_FIELD, 
  XSD_GROUP, 
  XSD_IMPORT, 
  XSD_INCLUDE, 
  XSD_KEY, 
  XSD_KEYREF, 
  XSD_LIST, 
  XSD_NOTATION, 
  XSD_REDEFINE, 
  XSD_RESTRICTION, 
  XSD_SCHEMA, 
  XSD_SELECTOR, 
  XSD_SEQUENCE, 
  XSD_SIMPLE_CONTENT, 
  XSD_SIMPLE_TYPE, 
  XSD_UNION, 
  XSD_UNIQUE
};

typedef enum _xsdKeyword xsdKeyword; 
typedef enum _xsdAttr xsdAttr; 

static xmlNodePtr _xmlGetChild(xmlNodePtr node);
static xmlNodePtr _xmlGetNext(xmlNodePtr node);
static HCOMPLEXTYPE xsdProcComplexType(xmlNodePtr node, const char* type);

static 
xsdKeyword xsdGetKeyword(xmlNodePtr node)
{
  if (node == NULL) return XSD_UNKNOWN;

  if (NODE_NAME_EQUALS(node, XSD_ALL_STR)) return XSD_ALL;
  if (NODE_NAME_EQUALS(node, XSD_ANNOTATION_STR)) return XSD_ANNOTATION;
  if (NODE_NAME_EQUALS(node, XSD_ANY_STR)) return XSD_ANY;
  if (NODE_NAME_EQUALS(node, XSD_ANY_ATTRIBUTE_STR)) return XSD_ANY_ATTRIBUTE;
  if (NODE_NAME_EQUALS(node, XSD_APPINFO_STR)) return XSD_APPINFO;
  if (NODE_NAME_EQUALS(node, XSD_ATTRIBUTE_STR)) return XSD_ATTRIBUTE;
  if (NODE_NAME_EQUALS(node, XSD_ATTRIBUTE_GROUP_STR)) return XSD_ATTRIBUTE_GROUP;
  if (NODE_NAME_EQUALS(node, XSD_CHOICE_STR)) return XSD_CHOICE;
  if (NODE_NAME_EQUALS(node, XSD_COMPLEX_TYPE_STR)) return XSD_COMPLEX_TYPE;
  if (NODE_NAME_EQUALS(node, XSD_COMPLEX_CONTENT_STR)) return XSD_COMPLEX_CONTENT;
  if (NODE_NAME_EQUALS(node, XSD_DOCUMENTATION_STR)) return XSD_DOCUMENTATION;
  if (NODE_NAME_EQUALS(node, XSD_ELEMENT_STR)) return XSD_ELEMENT;
  if (NODE_NAME_EQUALS(node, XSD_EXTENSION_STR)) return XSD_EXTENSION;
  if (NODE_NAME_EQUALS(node, XSD_FIELD_STR)) return XSD_FIELD;
  if (NODE_NAME_EQUALS(node, XSD_GROUP_STR)) return XSD_GROUP;
  if (NODE_NAME_EQUALS(node, XSD_IMPORT_STR)) return XSD_IMPORT;
  if (NODE_NAME_EQUALS(node, XSD_INCLUDE_STR)) return XSD_INCLUDE;
  if (NODE_NAME_EQUALS(node, XSD_KEY_STR)) return XSD_KEY;
  if (NODE_NAME_EQUALS(node, XSD_KEYREF_STR)) return XSD_KEYREF;
  if (NODE_NAME_EQUALS(node, XSD_LIST_STR)) return XSD_LIST;
  if (NODE_NAME_EQUALS(node, XSD_NOTATION_STR)) return XSD_NOTATION;
  if (NODE_NAME_EQUALS(node, XSD_REDEFINE_STR)) return XSD_REDEFINE;
  if (NODE_NAME_EQUALS(node, XSD_RESTRICTION_STR)) return XSD_RESTRICTION;
  if (NODE_NAME_EQUALS(node, XSD_SCHEMA_STR)) return XSD_SCHEMA;
  if (NODE_NAME_EQUALS(node, XSD_SELECTOR_STR)) return XSD_SELECTOR;
  if (NODE_NAME_EQUALS(node, XSD_SEQUENCE_STR)) return XSD_SEQUENCE;
  if (NODE_NAME_EQUALS(node, XSD_SIMPLE_CONTENT_STR)) return XSD_SIMPLE_CONTENT;
  if (NODE_NAME_EQUALS(node, XSD_SIMPLE_TYPE_STR)) return XSD_SIMPLE_TYPE;
  if (NODE_NAME_EQUALS(node, XSD_UNION_STR)) return XSD_UNION;
  if (NODE_NAME_EQUALS(node, XSD_UNIQUE_STR)) return XSD_UNIQUE;

  return XSD_UNKNOWN;
}


static char outDir[1054];



static 
void usage();


static 
xmlNodePtr xmlFindSubElement(xmlNodePtr root, const char* element_name)
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

static
xmlNodePtr xsdLoadFile(const char* filename)
{
  xmlDocPtr doc;
  xmlNodePtr cur;

  doc = xmlParseFile(filename);
  if (doc == NULL) return NULL;

  cur = xmlDocGetRootElement(doc);

  return cur;
}


static
xmlNodePtr wsdlLoadFile(const char* filename)
{
  xmlDocPtr doc;
  xmlNodePtr cur;
  xmlNodePtr sub;
  xsdKeyword keyword;

  doc = xmlParseFile(filename);
  if (doc == NULL) return NULL;

  cur = xmlDocGetRootElement(doc);
  if (cur == NULL) {
    return NULL;
  }

  cur = xmlFindSubElement(cur, "types");
  if (cur == NULL) return NULL;
/*
  sub = xmlFindSubElement(cur, "schema");
  if (sub != NULL)
    return sub;
*/
  /* some wsdl's defines xsd without root <schema> element */
  sub = _xmlGetChild(cur);
  keyword = xsdGetKeyword(sub);
  switch (keyword) 
  {
    case XSD_ELEMENT:
    case XSD_COMPLEX_TYPE:
    case XSD_SIMPLE_TYPE:
    case XSD_SCHEMA:
      return sub;
  }

  return NULL;
}


static 
xmlNodePtr _xmlGetChild(xmlNodePtr node)
{
  xmlNodePtr cur = NULL;
  cur = node->xmlChildrenNode;
  while (cur != NULL) {
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }
    return cur;
  }
  return cur;
}

static 
xmlNodePtr _xmlGetNext(xmlNodePtr node)
{
  xmlNodePtr cur = NULL;
  cur = node->next;
  while (cur != NULL) {
    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }
    return cur;
  }
  return cur;
}

static 
void xsdProcAttribute(HCOMPLEXTYPE parent, xmlNodePtr node)
{
  char *name, *type;
  xmlNodePtr cur;
  char buffer[1054];
  
  name = xmlGetProp(node, ATTR_NAME_STR);
  type = xmlGetProp(node, ATTR_TYPE_STR);

 /* printf("  %s: %s\n", type?type:"(null)",
    name?name:"(null)");
*/
  if (name == NULL)
  {
    fprintf(stderr, "WARNING: Attribute without name!\n");
    return;
  }

  if (type == NULL)
  {
    fprintf(stderr, "WARNING: Attribute '%s' has no type\n", name);
  }

/*  sprintf(buffer, "attr_%s", name); */

  objAddAttribute(parent, name, type, 0);
}

static 
void xsdProcElement(HCOMPLEXTYPE parent, xmlNodePtr node)
{
  char *name, *type, *minostr, *maxostr;
  xmlNodePtr cur;
  xsdKeyword keyword;
  char buffer[1054];
  HCOMPLEXTYPE ct;
  int mino, maxo;
  
  name = xmlGetProp(node, ATTR_NAME_STR);
  type = xmlGetProp(node, ATTR_TYPE_STR);
  minostr = xmlGetProp(node, ATTR_MIN_OCCURS_STR);
  maxostr = xmlGetProp(node, ATTR_MAX_OCCURS_STR);

/*  printf("  %s: %s\n", type?type:"(null)",
    name?name:"(null)");
*/
  if (minostr == NULL) mino = 1;
  else mino = atoi(minostr);

  if (maxostr == NULL) maxo = 1;
  else {
    if (!strcmp(maxostr, ATTR_VALUE_UNBOUNDED))
      maxo = -1;
    else
      maxo = atoi(maxostr);
  }
  

  if (type == NULL)
  {
    /* check for complexType */
    cur = _xmlGetChild(node);
    if (cur == NULL)
    {
      fprintf(stderr, "WARNING: Element '%s' has no childs\n", name);
      return;
    }

    do
    {
      keyword = xsdGetKeyword(cur);

      switch (keyword)
      {
        case XSD_COMPLEX_TYPE:
          /*
          type = xmlGetProp(cur, ATTR_NAME_STR);
          if (type == NULL) 
          {
            fprintf(stderr, "WARNING: Type name not found\n");
            break;
          }
          */
          
          sprintf(buffer, "%s_%s", parent->type, (const char*)name);
          ct = xsdProcComplexType(cur, (const char*)buffer);
          if (ct != NULL)
          {
            objAddElement(parent, name, buffer,0, mino, maxo);
          }
          break;
      }

    } while ((cur = _xmlGetNext(cur)) != NULL);
  }
  else
  {
    objAddElement(parent, name, type,0, mino, maxo);
  }

  if (name) xmlFree(name);
  if (type) xmlFree(type);
}

static
void xsdProcSequence(HCOMPLEXTYPE ct, xmlNodePtr node)
{
  xmlNodePtr cur = NULL;
  xsdKeyword keyword;

  cur = _xmlGetChild(node);
  if (cur == NULL) {
    fprintf(stderr, "WARNING: Empty sequence\n");
    return;
  }

  do {
    keyword = xsdGetKeyword(cur);

    switch (keyword) 
    {
      case XSD_ANNOTATION:
        /* nothing to do*/
        break;

      case XSD_GROUP:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_GROUP_STR);
        break;

      case XSD_CHOICE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_CHOICE_STR);
        break;

      case XSD_SEQUENCE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_SEQUENCE_STR);
        break;

      case XSD_ANY:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ANY_STR);
        break;

      case XSD_ELEMENT:
        xsdProcElement(ct, cur);
        break;

      default:
         fprintf(stderr, "WARNING: Unknown child ('%s')!\n", (char*)cur->name);
    };
   } while ((cur = _xmlGetNext(cur)) != NULL);
}


static
void xsdProcExtension(HCOMPLEXTYPE ct, xmlNodePtr node, const char* type)
{
  xmlNodePtr cur = NULL;
  xsdKeyword keyword;
  char * base;


  base = xmlGetProp(node, ATTR_BASE_STR);
  if (base == NULL) {
    fprintf(stderr, "WARNING: No base defined\n");
    return;
  }
  
  printf(" =[Base] -> %s\n", base);
  objSetBaseType(ct, base);
  xmlFree(base);

  cur = _xmlGetChild(node);
  if (cur == NULL) {
    fprintf(stderr, "WARNING: Empty node\n");
    return;
  }

  do {
    keyword = xsdGetKeyword(cur);

    switch (keyword) 
    {
      case XSD_ANNOTATION:
        /* nothing to do*/
        break;

      case XSD_ALL:
        fprintf(stderr, " WARNING: %s not supported\n", XSD_ALL_STR);
        break;

      case XSD_GROUP:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_GROUP_STR);
        break;

      case XSD_CHOICE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_CHOICE_STR);
        break;

      case XSD_ATTRIBUTE:
        xsdProcAttribute(ct, cur);
        break;

      case XSD_ATTRIBUTE_GROUP:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ATTRIBUTE_GROUP_STR);
        break;

      case XSD_ANY_ATTRIBUTE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ANY_ATTRIBUTE_STR);
        break;

       case XSD_SEQUENCE:
         xsdProcSequence(ct, cur);
         break;
        
       default:
         fprintf(stderr, "WARNING: Unknown child ('%s')!\n", (char*)cur->name);
    };
    
  } while ((cur = _xmlGetNext(cur)) != NULL);
}


static
void xsdProcComplexContent(HCOMPLEXTYPE ct, xmlNodePtr node, const char* type)
{
  xmlNodePtr cur = NULL;
  xsdKeyword keyword;

  cur = _xmlGetChild(node);
  if (cur == NULL) {
    fprintf(stderr, "WARNING: Empty sequence\n");
    return;
  }

  do {
    keyword = xsdGetKeyword(cur);

    switch (keyword) 
    {
      case XSD_ANNOTATION:
        /* nothing to do*/
        break;

      case XSD_EXTENSION:
        xsdProcExtension(ct, cur, type);
        break;

      case XSD_RESTRICTION:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_RESTRICTION_STR);
        break;

       default:
         fprintf(stderr, "WARNING: Unknown child ('%s')!\n", (char*)cur->name);
    };
    
  } while ((cur = _xmlGetNext(cur)) != NULL);
}

static 
HCOMPLEXTYPE xsdProcComplexType(xmlNodePtr node, const char* type)
{
  char *name;
  xmlNodePtr cur = NULL;
  xsdKeyword keyword;
  HCOMPLEXTYPE ct;

  if (!type)
    name = xmlGetProp(node, ATTR_NAME_STR);
  else {
    name = (char*)malloc(strlen(type)+1);
    strcpy(name, type);
  }

  if (!name)
  {
    fprintf(stderr, "\nWARNING: complexType has no typename!\n", name);
    return NULL;
  }
  
  ct = objCreateComplexType(name); 
  
  printf("\ncomplexType->%s\n", name);
  
  cur = _xmlGetChild(node);
  if (cur == NULL) {
    fprintf(stderr, "WARNING: Empty complexType\n");
    return ct;
  }



  do {
    keyword = xsdGetKeyword(cur);

    switch (keyword) 
    {
      case XSD_ANNOTATION:
        /* nothing to do*/
        break;

      case XSD_SIMPLE_CONTENT:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_SIMPLE_CONTENT_STR);
        break;

      case XSD_COMPLEX_CONTENT:
        xsdProcComplexContent(ct, cur, name);
      /*  fprintf(stderr, "WARNING: %s not supported\n", XSD_COMPLEX_CONTENT_STR); */
        break;

      case XSD_ALL:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ALL_STR);
        break;

      case XSD_GROUP:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_GROUP_STR);
        break;

      case XSD_CHOICE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_CHOICE_STR);
        break;

      case XSD_ATTRIBUTE:
        xsdProcAttribute(ct, cur);
        break;

      case XSD_ATTRIBUTE_GROUP:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ATTRIBUTE_GROUP_STR);
        break;

      case XSD_ANY_ATTRIBUTE:
        fprintf(stderr, "WARNING: %s not supported\n", XSD_ANY_ATTRIBUTE_STR);
        break;

       case XSD_SEQUENCE:
         xsdProcSequence(ct, cur);
         break;
        
       default:
         fprintf(stderr, "WARNING: Unknown child ('%s')!\n", (char*)cur->name);
    };
    
  } while ((cur = _xmlGetNext(cur)) != NULL);

  xmlFree(name);
  return ct;
}



static 
void runGenerator(xmlNodePtr xsdRoot)
{
  xmlNodePtr cur;
  xmlNodePtr node;
  xmlChar *type;

  cur = xsdRoot->xmlChildrenNode;
  while (cur != NULL) {

    if (cur->type != XML_ELEMENT_NODE) {
      cur = cur->next;
      continue;
    }

    if (xsdGetKeyword(cur) == XSD_COMPLEX_TYPE){
      
      xsdProcComplexType(cur, NULL);

		}  else if (xsdGetKeyword(cur) == XSD_ELEMENT) {
      
      type = xmlGetProp(cur, "name");
      if (type == NULL) {
        fprintf(stderr, "WARNING: Element found without name  ('%s')\n", cur->name);
      } else {
        
        node = xmlFindSubElement(cur, XSD_COMPLEX_TYPE_STR);
        if (node != NULL) {
          xsdProcComplexType(node, type);
        }

      }

    }

    cur = cur->next;
  }
}

int declareStructs(HCOMPLEXTYPE ct)
{
  char fname[255];
  FILE* f;

  sprintf(fname, "%s/%s_xsd.h", outDir, ct->type);
  printf("Generating file '%s' ...\n", fname);
  f = fopen(fname, "w");
  if (f == NULL)
  {
    fprintf(stderr, "Can not open '%s'\n", fname);
    return 0;
  }

  writeComplexTypeHeaderFile(f, ct);
  fclose(f);

  return 1;
}

int writeSource(HCOMPLEXTYPE ct)
{
  char fname[255];
  FILE* f;

  sprintf(fname, "%s/%s_xsd.c", outDir, ct->type);
  printf("Generating file '%s' ...\n", fname);
  f = fopen(fname, "w");
  if (f == NULL)
  {
    fprintf(stderr, "Can not open '%s'\n", fname);
    return 0;
  }

  writeComplexTypeSourceFile(f, ct);
  fclose(f);

  return 1;
}


int xsdInitTrModule(xmlNodePtr xsdNode)
{
  xmlNsPtr ns;

  ns = xmlSearchNsByHref(xsdNode->doc, xsdNode, "http://www.w3.org/2001/XMLSchema");
  if (ns == NULL) {
    fprintf(stderr, "XML Schema namespace not found!\n");
    return 0;
  } 

  if (ns->prefix == NULL) {
    fprintf(stderr, "XML Schema namespace not found!\n");
    return 0;
  }

  fprintf(stdout, "XMLSchema namespace prefix: '%s'\n", ns->prefix);
  trInitModule(ns->prefix);

  return 1;
}


int xsdInitObjModule(xmlNodePtr xsdNode)
{
  xmlChar *tns;
  xmlNsPtr ns;

  tns = xmlGetProp(xsdNode, (const xmlChar*)"targetNamespace");

  if (tns == NULL) {
   
    objInitModule(NULL);

  } else {

    ns = xmlSearchNsByHref(xsdNode->doc, xsdNode, tns);
    if (ns == NULL) {
      fprintf(stderr, "WARNING: Target namespace not found!\n");
      return 0;
    } 

    if (ns->prefix == NULL) {
      fprintf(stderr, "WARNING: Target namespace not found!\n");
      return 0;
    }

    fprintf(stdout, "Target namespace ('%s') prefix: '%s'\n", tns, ns->prefix);
    objInitModule(ns->prefix);

  }


  return 1;
}


int main(int argc, char *argv[])
{
  int i;
  xmlNodePtr xsdNode = NULL;
  char fname[255];
  int wsdl = 0;

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
    else if (!strcmp(argv[i], "-wsdl"))
      wsdl = 1;
    else strcpy(fname, argv[i]);
  }
  
  mkdir(outDir, 000);

  if (wsdl)
    xsdNode = wsdlLoadFile(fname);
  else
    xsdNode = xsdLoadFile(fname);

  if (xsdNode == NULL) {
    fprintf(stderr, "can not load xsd file!\n");
    return 1;
  }

  if (!xsdInitTrModule(xsdNode))
    return 1;

  if (!xsdInitObjModule(xsdNode))
    return 1;

  runGenerator(xsdNode);
  objRegistryEnumComplexType(declareStructs);
  objRegistryEnumComplexType(writeSource);

  xmlFreeDoc(xsdNode->doc);
  trFreeModule();
  objFreeModule();

  return 0;
}



void usage(const char* execName) 
{
  printf("usage: %s [-d <destdir> -S -D] <xsd filename>\n", execName);
}




