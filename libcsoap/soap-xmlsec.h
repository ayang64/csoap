/******************************************************************
 *  $Id: soap-xmlsec.h,v 1.1 2006/11/24 11:22:55 m0gg Exp $
 *
 * CSOAP Project:  A SOAP client/server library in C
 * Copyright (C) 2006 Heiko Ronsdorf
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
 * Email: hero@persua.de
 ******************************************************************/
#ifndef __csoap_xmlsec_h
#define __csoap_xmlsec_h

#define CSOAP_ENABLE_XMLSEC	"-CSOAPxmlsec"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Initializes the XML security subsystem.
 *
 * @param argc commandline arg count
 * @param argv commandline arg vector
 *
 * @returns H_OK on success
 *
 */
extern herror_t soap_xmlsec_init_args(int argc, char **argv);

/**
 *
 * Sign a XML document contained in a SOAP Envelope
 *
 */
extern herror_t soap_xmlsec_sign(struct SoapEnv *envelope);

/**
 *
 * Encrypt a XML document contained in a SOAP envelope.
 *
 */
extern herror_t soap_xmlsec_encrypt(struct SoapEnv *envelope);

/**
 *
 * Frees the resources needed by the XML security subsystem.
 *
 */
extern void soap_xmlsec_destroy(void);

#ifdef __cplusplus
}
#endif

#endif
