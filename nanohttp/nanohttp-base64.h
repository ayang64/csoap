/******************************************************************
*  $Id: nanohttp-base64.h,v 1.4 2006/12/13 08:18:53 m0gg Exp $
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
* Email: hero@persua.de
******************************************************************/
#ifndef __nanohttp_base64_h
#define __nanohttp_base64_h

/** @file
 *
 * \section base64_data_enc_sec Base64 data encoding
 *
 * Base encoding of data is used in many situations to store or transfer data in
 * environments that, perhaps for legacy reasons, are restricted to US-ASCII
 * data. Base64 encoding can also be used in new applications that do not have
 * legacy restrictions, simply because it makes it possible to manipulate objects
 * with text editors.
 *
 * In the past, different applications have had different requirements and thus
 * sometimes implemented base encodings in slightly different ways. Today,
 * protocol specifications sometimes use base encodings in general, and "base64"
 * in particular, without a precise description or reference.  Multipurpose
 * Internet Mail Extensions (MIME) is often used as a reference for base64
 * without considering the consequences for line-wrapping or non-alphabet
 * characters.
 *
 * @author	H. Ronsdorf
 * @version	$Revision: 1.4 $
 *
 * @see http://www.ietf.org/rfc/rfc4648.txt
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Base64 encodes a NUL terminated array of characters.
 *
 * @param instr		Pointer to the input string.
 * @param outstr	Pointer to the output destination.
 *
 * @see base64_decode_string
 *
 */
extern void base64_encode_string(const unsigned char *instr, unsigned char *outstr);

/**
 *
 * Base64 decodes a NUL terminated array of characters.
 *
 * @param instr		Pointer to the input string.
 * @param outstr	Pointer to the output destination.
 *
 * @see base64_encode_string
 *
 */
extern void base64_decode_string(const unsigned char *instr, unsigned char *outstr);

#ifdef __cplusplus
}
#endif

#endif
