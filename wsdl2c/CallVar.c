/** Generated by xsd2c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CallVar.h"


struct CallVar* CallVar_Create()
{
	struct CallVar* _res;
	_res = (struct CallVar*)malloc(sizeof(struct CallVar));

	_res->name = NULL;
	_res->type = NULL;

	return _res;
}

void CallVar_Free(struct CallVar* obj)
{
	if (obj == NULL) return;
	if (obj->name != NULL) 
	{
		free(obj->name);
		obj->name = NULL;
	}
	if (obj->type != NULL) 
	{
		free(obj->type);
		obj->type = NULL;
	}
	free(obj);
}

void CallVar_Sax_Serialize(struct CallVar* obj,
		 const char *root_element_name,
		 void (*OnStartElement)(const char* element_name, int attr_count, char **keys, char **values, void* userData),
		 void (*OnCharacters)(const char* element_name, const char* chars, void* userData),
		 void (*OnEndElement)(const char* element_name, void* userData),
		 void* userData)
{
	int attrCount, curCount;
	char **keys;
	char **values;
	char buffer[255];


	attrCount = 0;

	keys = (char**)malloc(sizeof(char*)*attrCount);
	values = (char**)malloc(sizeof(char*)*attrCount);

	curCount = 0;


	OnStartElement(root_element_name, attrCount, keys, values, userData);

	OnStartElement("name", 0, NULL, NULL, userData);
	if (obj->name != NULL)
		OnCharacters("name", obj->name, userData);
	OnEndElement("name", userData);

	OnStartElement("type", 0, NULL, NULL, userData);
	if (obj->type != NULL)
		OnCharacters("type", obj->type, userData);
	OnEndElement("type", userData);

	OnEndElement(root_element_name, userData);
}

#ifndef _DESERIALIZER_DISABLED_

struct CallVar* CallVar_Deserialize(xmlNodePtr xmlRoot)
{
	xmlNodePtr cur;
	xmlChar *key;
	struct CallVar* obj;
	obj = CallVar_Create();
	cur = xmlRoot->xmlChildrenNode;
	while (cur != NULL) {
		if (cur->type != XML_ELEMENT_NODE) {
			cur = cur->next;
			continue;
		}
		printf("CallVar->%s\n", cur->name);
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name"))){
			key = xmlNodeListGetString(cur->doc, cur->xmlChildrenNode, 1);
			CallVar_Set_name(obj, (const char*)key);
			xmlFree(key);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"type"))){
			key = xmlNodeListGetString(cur->doc, cur->xmlChildrenNode, 1);
			CallVar_Set_type(obj, (const char*)key);
			xmlFree(key);
		}
	// TODO: 
		cur = cur->next;
	}
	return obj;
}

#endif
char* CallVar_Get_name(struct CallVar* obj)
{
	return obj->name;
}

char* CallVar_Get_type(struct CallVar* obj)
{
	return obj->type;
}

void CallVar_Set_name(struct CallVar* obj, const char* name)
{
	if (obj->name != NULL) free(obj->name);
	obj->name = (char*)malloc(strlen(name)+1);
	strcpy(obj->name, name);
}

void CallVar_Set_type(struct CallVar* obj, const char* type)
{
	if (obj->type != NULL) free(obj->type);
	obj->type = (char*)malloc(strlen(type)+1);
	strcpy(obj->type, type);
}
