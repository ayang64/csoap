/******************************************************************
*  $Id: nanohttp-common.c,v 1.10 2004/08/30 13:28:58 snowdrop Exp $
*
* CSOAP Project:  A http client/server library in C
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

#include <nanohttp/nanohttp-common.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef WIN32
#include <string.h>
static char *strtok_r (char *s, const char *delim, char **save_ptr)
{
	char *token;

	if (s == NULL)
		s = *save_ptr;

	/* Scan leading delimiters.  */
	s += strspn (s, delim);
	if (*s == '\0')
		return NULL;

	/* Find the end of the token.  */
	token = s;
	s = strpbrk (token, delim);
	if (s == NULL)
		/* This token finishes the string.  */
		*save_ptr = strchr (token, '\0');
	else
	{
		/* Terminate the token and make *SAVE_PTR point past it.  */
		*s = '\0';
		*save_ptr = s + 1;
	}
	return token;
}
#endif

static log_level_t loglevel = HLOG_DEBUG;

log_level_t log_set_level(log_level_t level)
{
	log_level_t old = loglevel;
	loglevel = level;
	return old;
}


log_level_t log_get_level()
{
	return loglevel;
}

static
void log_write(log_level_t level, const char *prefix,
			   const char* func, const char *format, va_list ap)
{
	char buffer[1054];
	char buffer2[1054];

	if (level < loglevel) return;

	sprintf(buffer, "*%s*: [%s] %s\n", prefix,  func, format);
	vsprintf(buffer2, buffer, ap);
	printf(buffer2);
	fflush(stdout); 
}

void log_verbose(const char* FUNC, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	log_write(HLOG_VERBOSE, "VERBOSE", FUNC, format, ap);
	va_end(ap);
}

void log_debug(const char* FUNC, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	log_write(HLOG_DEBUG, "DEBUG", FUNC, format, ap);
	va_end(ap);
}

void log_info(const char* FUNC, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	log_write(HLOG_INFO, "INFO", FUNC, format, ap);
	va_end(ap);
}

void log_warn(const char* FUNC, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	log_write(HLOG_WARN, "WARN", FUNC, format, ap);
	va_end(ap);
}

void log_error(const char* FUNC, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	log_write(HLOG_ERROR, "ERROR", FUNC, format, ap);
	va_end(ap);
}


/* -----------------------------------------
FUNCTION: strcmpigcase
------------------------------------------ */
int strcmpigcase(const char *s1, const char *s2)
{
	int l1, l2, i;

	if (s1 == NULL && s2 == NULL) return 1;
	if (s1 == NULL || s2 == NULL) return 0;

	l1 = strlen(s1);
	l2 = strlen(s2);

	if (l1 != l2) return 0;

	for (i=0;i<l1;i++) 
		if (toupper(s1[i]) != toupper(s2[i]))
			return 0;

	return 1;
}


hpair_t *hpairnode_new(const char* key, const char* value, hpair_t *next)
{
	hpair_t *pair;

	log_verbose3("new pair ('%s','%s')", SAVE_STR(key), SAVE_STR(value));
	pair = (hpair_t*)malloc(sizeof(hpair_t));

	if (key != NULL) {
		pair->key = (char*)malloc(strlen(key)+1);
		strcpy(pair->key, key);
	} else {
		pair->key = NULL;
	}

	if (value != NULL) {
		pair->value = (char*)malloc(strlen(value)+1);
		strcpy(pair->value, value);
	} else {
		pair->value = NULL;
	}

	pair->next = next;

	return pair;
}

hpair_t *hpairnode_parse(const char *str, const char *delim, hpair_t *next)
{
	hpair_t *pair;
	char *key, *value;
	int c;

	pair = (hpair_t*)malloc(sizeof(hpair_t));
	pair->key = "";
	pair->value = "";
	pair->next = next;

	key = strtok_r((char *)str, delim, &value);

	if (key != NULL) {   
		pair->key = (char*)malloc(strlen(key)+1);
		strcpy(pair->key, key);
	}

	if (value != NULL) {
		for (c=0;value[c]==' ';c++); /* skip white space */
		pair->value = (char*)malloc(strlen(&value[c])+1);
		strcpy(pair->value, &value[c]);
	}

	return pair;
}


hpair_t* hpairnode_copy(const hpair_t *src)
{
	hpair_t *pair;

	if (src == NULL) return NULL;

	pair = hpairnode_new(src->key, src->value, NULL);
	return pair;
}


hpair_t* hpairnode_copy_deep(const hpair_t *src)
{
	hpair_t *pair, *result, *next;

	if (src == NULL) return NULL;

	result = hpairnode_copy(src);

	next = src->next;
	pair = result;

	while (next != NULL) {
		pair->next = hpairnode_copy(next); 
		pair = pair->next;
		next = next->next;
	}

	return result;
}


void hpairnode_dump(hpair_t *pair)
{
	if (pair == NULL) {
		log_verbose1("(NULL)[]");
		return;
	}

	log_verbose5("(%p)['%s','%s','%p']", pair, 
		SAVE_STR(pair->key), SAVE_STR(pair->value), 
		pair->next);
}


void hpairnode_dump_deep(hpair_t *pair)
{
	hpair_t *p;
	p = pair;

	log_verbose1("-- BEGIN dump hpairnode_t --");

	while (p != NULL) {
		hpairnode_dump(p);
		p = p->next;
	}

	log_verbose1("-- END dump hpairnode_t --\n");
}


void hpairnode_free(hpair_t *pair)
{
	if (pair == NULL) return;

	free(pair->key);
	free(pair->value);

	free(pair);
}


void hpairnode_free_deep(hpair_t *pair)
{
	hpair_t *tmp;

	while (pair != NULL) {
		tmp = pair->next;
		hpairnode_free(pair);
		pair=tmp;
	}
}

char *hpairnode_get_ignore_case(hpair_t *pair, const char* key)
{
	if (key == NULL) {
		log_error1("key is NULL");
		return NULL;
	}

	while (pair != NULL) {
		if (pair->key != NULL) {
			if (strcmpigcase(pair->key, key)) {
				return pair->value;
			}
		}
		pair = pair->next;
	}

	return NULL;
}

char *hpairnode_get(hpair_t *pair, const char* key)
{
	if (key == NULL) {
		log_error1("key is NULL");
		return NULL;
	}

	while (pair != NULL) {
		if (pair->key != NULL) {
			if (!strcmp(pair->key, key)) {
				return pair->value;
			}
		}
		pair = pair->next;
	}

	return NULL;
}

static
void hurl_dump(const hurl_t *url)
{

	if (url == NULL) {
		log_error1("url is NULL!");
		return ;
	}

	log_verbose2("PROTOCOL : %s", SAVE_STR(url->protocol));
	log_verbose2("    HOST : %s", SAVE_STR(url->host));
	log_verbose2("    PORT : %d", url->port);
	log_verbose2(" CONTEXT : %s", SAVE_STR(url->context));
}


hurl_t* hurl_new(const char* urlstr)
{
	int iprotocol;
	int ihost;
	int iport;
	int len;
	int size;
	hurl_t *url;
	char tmp[8];

	iprotocol = 0;
	len = strlen(urlstr);

	/* find protocol */
	while (urlstr[iprotocol] != ':' && urlstr[iprotocol] != '\0')
	{
		iprotocol++;
	}

	if (iprotocol == 0) {
		log_error1("no protocol");
		return NULL;
	}

	if (iprotocol + 3 >= len) {
		log_error1("no host");
		return NULL;
	}

	if ( urlstr[iprotocol] != ':' 
		&& urlstr[iprotocol+1] != '/' 
		&& urlstr[iprotocol+2] != '/')
	{
		log_error1("no protocol");
		return NULL;
	}

	/* find host */
	ihost = iprotocol + 3;
	while (urlstr[ihost] != ':'
		&& urlstr[ihost] != '/'
		&& urlstr[ihost] != '\0')
	{
		ihost++;
	}

	if (ihost == iprotocol + 1) {
		log_error1("no host");
		return NULL;
	}

	/* find port */
	iport = ihost;
	if (ihost + 1 < len) {
		if (urlstr[ihost] == ':') {
			while (urlstr[iport] != '/' && urlstr[iport] != '\0')  {
				iport++;
			}
		}
	}

	url = (hurl_t*)malloc(sizeof(hurl_t));

	url->protocol = (char*)malloc(sizeof(char)*iprotocol+1);
	strncpy(url->protocol, urlstr, iprotocol);
	url->protocol[iprotocol] = '\0';

	size = ihost - iprotocol - 3;
	url->host = (char*)malloc(sizeof(char)*size + 1);
	strncpy(url->host, &urlstr[iprotocol+3], size);
	url->host[size] = '\0';

	if (iport > ihost)
	{
		size = iport - ihost;
		strncpy(tmp, &urlstr[ihost+1], size);
		url->port = atoi(tmp);
	} else {
		url->port = 80;
	}

	len = strlen(urlstr);
	if (len > iport )
	{
		size = len - iport;
		url->context = (char*)malloc(sizeof(char)*size+1);
		strncpy(url->context, &urlstr[iport], size);
		url->context[size]='\0';
	} else {
		url->context = NULL;
	}

	hurl_dump(url);

	return url;
}


void hurl_free(hurl_t *url)
{
	if (url == NULL) return;

	free(url->protocol);
	free(url->host);
	free(url->context);

	free(url);
}


/* request stuff */

/* ----------------------------------------------------- 
FUNCTION: hrequest_new_from_buffer
----------------------------------------------------- */
hrequest_t *hrequest_new_from_buffer(char *data)
{
	hrequest_t *req;
	hpair_t *hpair = NULL, *qpair = NULL, *tmppair = NULL;

	char *tmp;
	char *tmp2;
	char *saveptr;
	char *saveptr2;
	char *saveptr3;
	char *result;
	char *key;
	char *value;
	char *opt_key;
	char *opt_value;
	int firstline = 1;

	req =  (hrequest_t*)malloc(sizeof(hrequest_t));

	req->method = NULL;
	req->spec = NULL;
	req->path = NULL;
	req->query = NULL;
	req->header = NULL;

	tmp = data;

	for (;;) {
		result = (char*)strtok_r(tmp, "\n", &saveptr);
		tmp=saveptr;

		if (result == NULL) 
			break;

		if (firstline) {
			firstline=0;
			tmp2 = result;

			/* parse [GET|POST] [PATH] [SPEC] */
			key = (char*)strtok_r(tmp2," ", &saveptr2);

			/* save method (get or post) */
			tmp2 = saveptr2;
			if (key != NULL) {
				req->method = (char*)malloc(strlen(key)+1);
				strcpy(req->method, key);
			}

			/* below is key the path and tmp2 the spec */
			key = (char*)strtok_r(tmp2," ", &saveptr2);

			/* save spec */
			tmp2 = saveptr2;
			if (tmp2 != NULL) {
				req->spec = (char*)malloc(strlen(tmp2)+1);
				strcpy(req->spec, tmp2);
			}

			/* parse and save path+query 
			parse: /path/of/target?key1=value1&key2=value2...
			*/

			if (key != NULL) {
				tmp2 = key;
				key = (char*)strtok_r(tmp2,"?", &saveptr2);
				tmp2 = saveptr2;

				/* save path */
				req->path = (char*)malloc(strlen(key)+1);
				strcpy(req->path, key);

				/* parse options */
				for (;;) {
					key = (char*)strtok_r(tmp2,"&",&saveptr2);
					tmp2 = saveptr2;

					if (key == NULL)
						break;

					opt_key = (char*)strtok_r(key,"=", &saveptr3);
					opt_value = saveptr3;

					if (opt_value == NULL)
						opt_value = "";

					/* create option pair */
					if (opt_key != NULL) {
						tmppair = (hpair_t*)malloc(sizeof(hpair_t));

						if (req->query == NULL) {
							req->query = qpair = tmppair;
						} else {
							qpair->next = tmppair;
							qpair = tmppair;
						}

						/* fill hpairnode_t struct */
						qpair->next = NULL;
						qpair->key = (char*)malloc(strlen(opt_key)+1);
						qpair->value = (char*)malloc(strlen(opt_value)+1);

						strcpy(qpair->key, opt_key);
						strcpy(qpair->value, opt_value);

					}
				}
			}

		} else {

			/* parse "key: value" */
			tmp2 = result;
			key = (char*)strtok_r(tmp2, ": ", &saveptr2);
			value = saveptr2;

			/* create pair */
			tmppair = (hpair_t*)malloc(sizeof(hpair_t));

			if (req->header == NULL) {
				req->header = hpair = tmppair;
			} else {
				hpair->next = tmppair;
				hpair = tmppair;
			}

			/* fill pairnode_t struct */
			hpair->next = NULL;
			hpair->key = (char*)malloc(strlen(key)+1);
			hpair->value = (char*)malloc(strlen(value)+1);

			strcpy(hpair->key, key);
			strcpy(hpair->value, value);
		}
	}

	return req;
}


void hrequest_free(hrequest_t *req)
{
	if (req == NULL) return;

	free(req->method);
	free(req->path);
	free(req->spec);

	hpairnode_free_deep(req->header);
	hpairnode_free_deep(req->query);

}

/* response stuff */


/* -------------------------------------------
FUNCTION: hresponse_new
---------------------------------------------*/
hresponse_t *hresponse_new()
{
	hresponse_t *res;

	/* create response object */
	res = (hresponse_t*)malloc(sizeof(hresponse_t));
	res->spec[0] = '\0';
	res->errcode = -1;
	res->desc = NULL;
	res->header = NULL;
	res->body = NULL;
	res->bodysize = 0;

	return res;
}


/* -------------------------------------------
FUNCTION: hresponse_new_from_buffer
---------------------------------------------*/
hresponse_t *hresponse_new_from_buffer(const char* buffer)
{
	hresponse_t *res;
	char *s1, *s2, *str;

	/* create response object */
	res = hresponse_new();

	/* *** parse spec *** */
	/* [HTTP/1.1 | 1.2] [CODE] [DESC] */

	/* stage 1: HTTP spec */
	str = (char*)strtok_r((char *)buffer, " ", &s2);
	s1 = s2;
	if (str == NULL) { log_error1("Parse error"); return NULL; }

	strncpy(res->spec, str, 10);

	/* stage 2: http code */
	str = (char*)strtok_r(s1, " ", &s2);
	s1 = s2;
	if (str == NULL) { log_error1("Parse error"); return NULL; }

	res->errcode = atoi(str);

	/* stage 3: description text */
	str = (char*)strtok_r(s1, "\r\n", &s2);
	s1 = s2;
	if (str == NULL) { log_error1("Parse error"); return NULL; }

	res->desc = (char*)malloc(strlen(str)+1);
	strcpy(res->desc, str);
	res->desc[strlen(str)] = '\0';

	/* *** parse header *** */
	/* [key]: [value] */
	for (;;) {
		str = strtok_r(s1, "\n", &s2);
		s1 = s2;

		/* check if header ends without body */
		if (str == NULL) {
			return res;
		}

		/* check also for end of header */
		if (!strcmp(str, "\r")) {
			break;
		}

		str[strlen(str)-1] = '\0';
		res->header = hpairnode_parse(str, ":", res->header);
	}

	/* *** Save body *** */
	res->body = s1;

	/* return response object */
	return res;
}


void hresponse_free(hresponse_t *res)
{
	/* not implemented yet!*/
}
