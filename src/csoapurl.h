/******************************************************************
 *  $Id: csoapurl.h,v 1.1 2003/03/25 22:17:30 snowdrop Exp $
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
#ifndef CSOAP_URL_H 
#define CSOAP_URL_H 


/* Include csoap headers */


typedef struct SOAPURL* HSOAPURL;


struct SOAPURL
{
  char *m_protocol;
  char *m_host;
  int m_port;
  char *m_context;
};


/**
 * Creates a new url object
 *
 * @param url the url string
 *
 * @return A soap url object if it was 
 *  successfull, NULL otherwise.
 */
HSOAPURL SoapUrlCreate(const char* url);


/**
 * Free a url object
 *
 *  @param url the url to free
 */
void SoapUrlFree(HSOAPURL url); 


/**
 * Dump the url data (for debugging)
 *
 *  @param url the url to show its content
 */
void SoapUrlDump(HSOAPURL url); 


#endif 
